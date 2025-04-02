#include "arap.h"
#include <opencv2/imgproc.hpp>


ARAP::ARAP(){}

std::vector<cv::Rect> ARAP::divideIntoGrids(const cv::Mat& img, int cellSize) {
    std::cout<<"开始分割"<<std::endl;
    std::vector<cv::Rect> grids;
    for (int y = 0; y < img.rows; y += cellSize) {
        for (int x = 0; x < img.cols; x += cellSize) {
            grids.push_back(cv::Rect(x, y, cellSize, cellSize));
            //std::cout<<"x:"<<x<<"y:"<<y<<std::endl;
            //std::cout<<"cellsize:"<<cellSize<<std::endl;

        }
    }
    return grids;
}

std::vector<cv::Point2f> ARAP::divideBlockIntoGrid(const cv::Rect& block,int cellSize) {
    std::vector<cv::Point2f> points;
    float stepX = static_cast<float>(block.width) / (cellSize - 1);
    float stepY = static_cast<float>(block.height) / (cellSize - 1);

    for (int i = 0; i < cellSize; ++i) {
        for (int j = 0; j < cellSize; ++j) {
            points.emplace_back(block.x + j * stepX, block.y + i * stepY);
        }
    }

    return points;
}

cv::Mat ARAP::deform(const cv::Mat& img, int cellSize, std::map<std::pair<int, int>, std::pair<int, int>> matches) {
    cv::Mat deformedImg = img.clone();
    std::vector<cv::Rect> grids = divideIntoGrids(img,cellSize);

    for (const auto& srcBlock : grids) {
        // 寻找每个块的匹配点
        std::vector<cv::Point2f> srcPoints, dstPoints;

        // std::cout<<"srcBlock:"<<srcBlock<<std::endl;
        // std::cout<<":"<<matches.size()<<std::endl;
        std::pair<int, int> match;
        for (auto& match : matches) {
            cv::Point2f srcPoint(match.first.first, match.first.second);
            cv::Point2f dstPoint(match.second.first, match.second.second);
            // std::cout<<"srcPoints:"<<srcPoints<<std::endl;
            // std::cout<<"dstPoints:"<<dstPoints<<std::endl;

            if (srcBlock.contains(srcPoint)) {
                srcPoints.push_back(srcPoint);
                dstPoints.push_back(dstPoint);
                std::cout<<"有匹配点"<<std::endl;
            }
            else{
                // std::cout<<"没有匹配点"<<std::endl;
            }
        }
        if (srcPoints.empty()) continue;

        // 将原始块分成晶格
        //std::vector<cv::Point2f> gridSrcPoints = divideBlockIntoGrid(srcBlock,cellSize);
        // std::vector<cv::Point2f> deformedPoints = performARAP(srcPoints, dstPoints);
        std::vector<cv::Point2f> deformedPoints = dstPoints;
        cv::Rect dstBlock = cv::boundingRect(dstPoints);
        cv::Mat deformedBlock = applyDeformationToBlock(img, srcPoints, deformedPoints, srcBlock);
        cv::Rect validDstRect = dstBlock & cv::Rect(0, 0, deformedImg.cols, deformedImg.rows);
        if (validDstRect.width > 0 && validDstRect.height > 0) {
            if (validDstRect.size() == deformedBlock.size()) {
                deformedBlock.copyTo(deformedImg(validDstRect));
            } else {
                cv::Mat resizedDeformedBlock;
                cv::resize(deformedBlock, resizedDeformedBlock, validDstRect.size());
                resizedDeformedBlock.copyTo(deformedImg(validDstRect));
            }
        }
    }

    return deformedImg;
}

std::vector<cv::Point2f> ARAP::performARAP(const std::vector<cv::Point2f>& srcPoints, const std::vector<cv::Point2f>& dstPoints) {
    // 初始化矩阵
    int P_Num = srcPoints.size();
    Eigen::MatrixXf P0(2, P_Num),P1(2, P_Num),P(2, P_Num),
        P_Prime(2, P_Num), P_Primerecords(2, P_Num);
    std::vector<Eigen::Matrix2f> R(P_Num, Eigen::Matrix2f::Identity());

    std::cout<<"形变前dstPoints:"<<dstPoints<<std::endl;

    // 填充矩阵，将原始点填入P，目标点填入P_Prime，R为旋转矩阵
    for (int i = 0; i < P_Num; ++i) {
        P0.col(i) << srcPoints[i].x, srcPoints[i].y;
        P1.col(i) << srcPoints[i].x, srcPoints[i].y;
        P_Prime.col(i) << dstPoints[i].x, dstPoints[i].y;
        P_Primerecords.col(i) << dstPoints[i].x, dstPoints[i].y;
    }

    // 稀疏矩阵Weight表示点间权重关系
    Eigen::SparseMatrix<float> Weight(P_Num, P_Num);
    buildWeightMatrix(dstPoints, Weight);

    for (int iter = 0; iter < 1; ++iter) {
        updateR(P, P_Prime, R, Weight); // 构造最佳刚性变换旋转矩阵R
        updateP_Prime(P, P_Prime, R, Weight); // 根据R更新P_Prime
    }

    std::vector<cv::Point2f> deformedPoints(P_Num);
    for (int i = 0; i < P_Num; ++i) {
        deformedPoints[i] = cv::Point2f(P_Prime(0, i), P_Prime(1, i));
    }
    std::cout<<"形变后dstPoints:"<<deformedPoints<<std::endl;

    return deformedPoints;
}

std::vector<cv::Point2f> ARAP::applyTransformation(const std::vector<cv::Point2f>& srcPoints, const std::vector<cv::Point2f>& dstPoints, int cols) {
    std::vector<cv::Point2f> optimizedDstPoints(dstPoints.size());

    for (size_t i = 0; i < srcPoints.size(); ++i) {

        // 初始化局部点集
        std::vector<cv::Point2f> srcSubset, dstSubset;

        // 计算行号和列号
        int row = i / cols;
        int col = i % cols;

        // 添加当前点
        srcSubset.push_back(srcPoints[i]);
        dstSubset.push_back(dstPoints[i]);
        std::cout << "当前点为：" << srcPoints[i] << std::endl;

        // 添加右边的点
        if (col + 1 < cols) {
            srcSubset.push_back(srcPoints[i + 1]);
            dstSubset.push_back(dstPoints[i + 1]);
            std::cout << "右边点为：" << srcPoints[i + 1] << std::endl;
        }

        // 添加右下角的点
        if (col + 1 < cols && row + 1 < srcPoints.size() / cols) {
            srcSubset.push_back(srcPoints[i + cols + 1]);
            dstSubset.push_back(dstPoints[i + cols + 1]);
            std::cout << "右下点为：" << srcPoints[i + cols + 1] << std::endl;
        }

        // 检查是否有足够的点进行计算
        if (srcSubset.size() < 3 || dstSubset.size() < 3) {
            continue;
        }

        // 计算旋转矩阵
        Eigen::Matrix2f R = computeLocalTransformation(srcSubset, dstSubset);

        // 计算质心
        cv::Point2f c(0, 0);
        for (const auto& pt : srcSubset) {
            c += pt;
        }
        c.x /= srcSubset.size();
        c.y /= srcSubset.size();
        std::cout << "质心为：" << c << std::endl;

        // 指向质心的向量（从全局坐标系中转化到局部坐标系中）
        Eigen::Vector2f p(srcSubset[0].x - c.x, srcSubset[0].y - c.y);
        Eigen::Vector2f p_prime = R * p;
        // 转回全局坐标系
        optimizedDstPoints[i] = cv::Point2f(p_prime(0) + c.x, p_prime(1) + c.y);
        std::cout << "变换后的点为：" << optimizedDstPoints[i] << std::endl;
    }

    return optimizedDstPoints;
}

Eigen::Matrix2f ARAP::computeLocalTransformation(const std::vector<cv::Point2f>& srcPoints, const std::vector<cv::Point2f>& dstPoints) {
    // 计算质心
    cv::Point2f c(0, 0), c_prime(0, 0);

    for (const auto& pt : srcPoints) {
        c += pt;
    }
    for (const auto& pt : dstPoints) {
        c_prime += pt;
    }
    c.x /= static_cast<float>(srcPoints.size());
    c.y /= static_cast<float>(srcPoints.size());
    c_prime.x /= static_cast<float>(dstPoints.size());
    c_prime.y /= static_cast<float>(dstPoints.size());

    // 计算偏移
    Eigen::MatrixXf P(2, srcPoints.size());
    Eigen::MatrixXf P_prime(2, dstPoints.size());
    for (size_t i = 0; i < srcPoints.size(); ++i) {
        P(0, i) = srcPoints[i].x - c.x;
        P(1, i) = srcPoints[i].y - c.y;
        P_prime(0, i) = dstPoints[i].x - c_prime.x;
        P_prime(1, i) = dstPoints[i].y - c_prime.y;
    }

    // 计算SVD
    Eigen::Matrix2f S = P_prime * P.transpose();
    Eigen::JacobiSVD<Eigen::Matrix2f> svd(S, Eigen::ComputeFullU | Eigen::ComputeFullV);
    Eigen::Matrix2f U = svd.matrixU();
    Eigen::Matrix2f V = svd.matrixV();

    // 计算旋转矩阵
    Eigen::Matrix2f R = U * V.transpose();

    if (R.determinant() < 0) {
        U.col(1) *= -1;
        R = U * V.transpose();
    }

    return R;
}


void ARAP::buildWeightMatrix(const std::vector<cv::Point2f>& srcPoints, Eigen::SparseMatrix<float>& Weight) {
    int P_Num = srcPoints.size();

    std::vector<Eigen::Triplet<float>> triplets;
    for (int i = 0; i < P_Num; ++i) {
        for (int j = 0; j < P_Num; ++j) {
            if (i != j) {
                float wij = 1.0f / (norm(srcPoints[i] - srcPoints[j]) + 1e-6f);
                if (wij > 1e-3) { // 避免权重过小
                    triplets.emplace_back(i, j, wij);
                }
            }
        }
    }
    Weight.setFromTriplets(triplets.begin(), triplets.end());
}

void ARAP::updateR(const Eigen::MatrixXf& P, const Eigen::MatrixXf& P_Prime, std::vector<Eigen::Matrix2f>& R, const Eigen::SparseMatrix<float>& Weight) {
    int P_Num = P.cols();

    for (int i = 0; i < P_Num; ++i) {
        Eigen::Matrix2f Si = Eigen::Matrix2f::Zero();

        // 计算协方差矩阵
        for (Eigen::SparseMatrix<float>::InnerIterator it(Weight, i); it; ++it) {
            int j = it.index();
            Eigen::Vector2f diff_P = P.col(i) - P.col(j);
            Eigen::Vector2f diff_P_Prime = P_Prime.col(i) - P_Prime.col(j);
            Si += it.value() * diff_P * diff_P_Prime.transpose();
        }

        // 使用 SVD 分解协方差矩阵
        Eigen::JacobiSVD<Eigen::Matrix2f> svd(Si, Eigen::ComputeFullU | Eigen::ComputeFullV);
        Eigen::Matrix2f U = svd.matrixU();
        Eigen::Matrix2f V = svd.matrixV();
        Eigen::Matrix2f R_i = V * U.transpose();

        // 确保旋转矩阵的正交性
        if (R_i.determinant() < 0) {
            U.col(1) *= -1;
            R_i = V * U.transpose();
        }

        R[i] = R_i;
    }
}

void ARAP::updateP_Prime(const Eigen::MatrixXf& P, Eigen::MatrixXf& P_Prime, const std::vector<Eigen::Matrix2f>& R, const Eigen::SparseMatrix<float>& Weight) {
    int P_Num = P.cols();
    Eigen::MatrixXf b = Eigen::MatrixXf::Zero(2, P_Num);

    for (int i = 0; i < P_Num; ++i) {
        for (Eigen::SparseMatrix<float>::InnerIterator it(Weight, i); it; ++it) {
            int j = it.index();
            b.col(i) += it.value() * (R[i] * (P.col(i).head<2>() - P.col(j).head<2>()) + R[j] * (P_Prime.col(j).head<2>() - P.col(i).head<2>()));
        }
    }

    // 构造拉普拉斯矩阵L
    Eigen::SparseMatrix<float> L = Weight;
    for (int k = 0; k < Weight.outerSize(); ++k) {
        float rowSum = 0;
        for (Eigen::SparseMatrix<float>::InnerIterator it(Weight, k); it; ++it) {
            if (it.row() != k) {
                rowSum += it.value();
            }
        }
        L.coeffRef(k, k) = -rowSum;
    }

    // 确保矩阵L的对角元素
    for (int i = 0; i < L.outerSize(); ++i) {
        for (Eigen::SparseMatrix<float>::InnerIterator it(L, i); it; ++it) {
            if (it.row() == it.col()) {
                L.coeffRef(it.row(), it.col()) = -L.row(it.row()).sum();
            }
        }
    }

    // 求解方程
    Eigen::SparseLU<Eigen::SparseMatrix<float>> solver;
    solver.compute(L.transpose() * L);
    if (solver.info() != Eigen::Success) {
        std::cerr << "Decomposition failed with info: " << solver.info() << std::endl;
        return;
    }

    for (int i = 0; i < 2; ++i) {
        Eigen::VectorXf P_Prime_i = solver.solve(L.transpose() * b.row(i).transpose());
        if (solver.info() != Eigen::Success) {
            std::cerr << "Solving failed with info: " << solver.info() << std::endl;
            return;
        }
        P_Prime.row(i) = P_Prime_i.transpose();
    }
}

cv::Mat ARAP::applyDeformationToBlock(const cv::Mat& img, const std::vector<cv::Point2f>& srcPoints, const std::vector<cv::Point2f>& dstPoints, const cv::Rect& rect) {
    cv::Mat deformedBlock = cv::Mat::zeros(rect.size(), img.type());

    std::vector<cv::Point2f> srcCorners = getBlockCorners(rect);
    std::vector<cv::Point2f> dstCorners(srcCorners.size());

    for (size_t i = 0; i < srcCorners.size(); ++i) {
        dstCorners[i] = calculateNewPosition(srcCorners[i], srcPoints, dstPoints) - cv::Point2f(rect.x, rect.y);
    }

    cv::Mat warpMat = getPerspectiveTransform(srcCorners, dstCorners);
    warpPerspective(img(rect), deformedBlock, warpMat, deformedBlock.size(), cv::INTER_LINEAR, cv::BORDER_REFLECT_101);

    cv::imshow("变形后方块",deformedBlock);
    cv::waitKey(0);
    return deformedBlock;
}

std::vector<cv::Point2f> ARAP::getBlockCorners(const cv::Rect& rect) {
    std::vector<cv::Point2f> corners;
    corners.emplace_back(rect.x, rect.y);
    corners.emplace_back(rect.x + rect.width, rect.y);
    corners.emplace_back(rect.x, rect.y + rect.height);
    corners.emplace_back(rect.x + rect.width, rect.y + rect.height);
    return corners;
}

cv::Point2f ARAP::calculateNewPosition(const cv::Point2f& pixelPos, const std::vector<cv::Point2f>& srcPoints, const std::vector<cv::Point2f>& dstPoints) {
    float totalWeight = 0.0f;
    cv::Point2f newPos(0, 0);

    for (size_t i = 0; i < srcPoints.size(); ++i) {
        float distance = norm(pixelPos - srcPoints[i]);
        float weight = 1.0f / (distance + 1e-6f);
        totalWeight += weight;
        newPos += weight * dstPoints[i];
    }

    if (totalWeight > 0) {
        newPos /= totalWeight;
    }

    return newPos;
}

void ARAP::drawPointsAndWeights(const cv::Mat& img, const std::vector<cv::Point2f>& srcPoints, const Eigen::SparseMatrix<float>& Weight) {
    cv::Mat imgWithPoints = img.clone();

    // 画出所有点
    for (size_t i = 0; i < srcPoints.size(); ++i) {
        cv::circle(imgWithPoints, srcPoints[i], 5, cv::Scalar(0, 0, 255), -1);
    }

    // 遍历所有点对，输出权重并绘制线条
    for (int i = 0; i < srcPoints.size(); ++i) {
        for (Eigen::SparseMatrix<float>::InnerIterator it(Weight, i); it; ++it) {
            int j = it.index();
            if (i < j) {
                float weight = it.value();
                std::cout << "Weight between points " << i << " and " << j << " = " << weight << std::endl;
                cv::line(imgWithPoints, srcPoints[i], srcPoints[j], cv::Scalar(255, 0, 0), 1);
                cv::putText(imgWithPoints, std::to_string(weight), (srcPoints[i] + srcPoints[j]) * 0.5, cv::FONT_HERSHEY_SIMPLEX, 0.4, cv::Scalar(0, 255, 0), 1);
            }
        }
    }
    // 显示图像
    cv::imshow("Points and Weights", imgWithPoints);
    cv::waitKey(0);
}

