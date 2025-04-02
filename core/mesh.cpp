#include "mesh.h"

mesh::mesh() {}

// 可视化权值矩阵
void mesh::visualizeWeightMatrix(const Eigen::SparseMatrix<float>& Weight, int size) {
    cv::Mat weightMat(size, size, CV_32F, cv::Scalar(0));

    for (int k = 0; k < Weight.outerSize(); ++k) {
        for (Eigen::SparseMatrix<float>::InnerIterator it(Weight, k); it; ++it) {
            weightMat.at<float>(it.row(), it.col()) = it.value();
        }
    }

    cv::normalize(weightMat, weightMat, 0, 255, cv::NORM_MINMAX, CV_32F);
    cv::imshow("Weight Matrix", weightMat);
    cv::waitKey(0);
}

// 打印权值矩阵状态
void mesh::printWeightMatrixStats(const Eigen::SparseMatrix<float>& Weight) {
    std::vector<float> values;
    for (int k = 0; k < Weight.outerSize(); ++k) {
        for (Eigen::SparseMatrix<float>::InnerIterator it(Weight, k); it; ++it) {
            values.push_back(it.value());
        }
    }

    float maxVal = *std::max_element(values.begin(), values.end());
    float minVal = *std::min_element(values.begin(), values.end());
    float sum = std::accumulate(values.begin(), values.end(), 0.0f);
    float mean = sum / values.size();
    float sq_sum = std::inner_product(values.begin(), values.end(), values.begin(), 0.0f);
    float stdev = std::sqrt(sq_sum / values.size() - mean * mean);

    std::cout << "Weight Matrix Stats:" << std::endl;
    std::cout << "Max: " << maxVal << std::endl;
    std::cout << "Min: " << minVal << std::endl;
    std::cout << "Mean: " << mean << std::endl;
    std::cout << "Standard Deviation: " << stdev << std::endl;
}

// 绘制网格
void mesh::drawMeshGrid(cv::Mat& image, const std::vector<cv::Vec6f>& triangles, const std::vector<cv::Point2f>& points, const std::vector<cv::Point2f>& transformedPoints) {
    for (const auto& t : triangles) {
        std::vector<cv::Point2f> srcTri(3), dstTri(3);
        for (int i = 0; i < 3; ++i) {
            srcTri[i] = cv::Point2f(t[i * 2], t[i * 2 + 1]);
            auto it = std::find(points.begin(), points.end(), srcTri[i]);
            if (it != points.end()) {
                dstTri[i] = transformedPoints[std::distance(points.begin(), it)];
            }
        }
        // 在图像上绘制三角形
        cv::line(image, dstTri[0], dstTri[1], cv::Scalar(0, 255, 0), 1);
        cv::line(image, dstTri[1], dstTri[2], cv::Scalar(0, 255, 0), 1);
        cv::line(image, dstTri[2], dstTri[0], cv::Scalar(0, 255, 0), 1);
    }
}

// 检查三角形是否在图像的白色区域内
bool mesh::isInWhiteArea(const cv::Point2f& point, const cv::Mat& img,int cellSize) {
    // 计算区域的边界
    int startX = std::max(0, static_cast<int>(point.x - cellSize ));
    int startY = std::max(0, static_cast<int>(point.y - cellSize ));
    int endX = std::min(img.cols - 1, static_cast<int>(point.x + cellSize ));
    int endY = std::min(img.rows - 1, static_cast<int>(point.y + cellSize ));
    int totalPixels = 0;
    int whitePixels = 0;

    // 统计区域内像素的数量和白色像素的数量
    for (int y = startY; y <= endY; ++y) {
        for (int x = startX; x <= endX; ++x) {
            totalPixels++;
            if (img.at<uchar>(y, x) == 255) {
                whitePixels++;
            }
        }
    }

    // 计算白色像素比例
    double whiteRatio = static_cast<double>(whitePixels) / totalPixels;

    // 返回是否达到阈值
    return whiteRatio > 0.9;
}

// 将原图划分为方格
std::vector<cv::Point2f> mesh::divideIntoGrids(const cv::Mat& img, int cols,int rows) {
    std::vector<cv::Point2f> points;
    float stepX = static_cast<float>(img.cols) / cols ;
    float stepY = static_cast<float>(img.rows) / rows ;
    for (int i = 0; i < rows; ++i) {
        for (int j = 0; j < cols; ++j) {
            if(!isInWhiteArea(cv::Point2f(j * stepX, i * stepY),img,rows)){points.emplace_back(j * stepX, i * stepY);}
            else{continue;}
        }
    }
    return points;
}

// 应用仿射变换
void mesh::applyAffineTransform(cv::Mat& warpImage, const cv::Mat& src,
                          std::vector<cv::Point2f> srcTri, std::vector<cv::Point2f> dstTri) {
    cv::Mat warpMat = cv::getAffineTransform(srcTri, dstTri);
    cv::warpAffine(src, warpImage, warpMat, warpImage.size(), cv::INTER_LINEAR, cv::BORDER_REFLECT_101);
}


// 估计旋转矩阵
std::vector<Eigen::Matrix2f> mesh::estimateRotations(const std::vector<cv::Point2f>& srcPoints, const std::vector<cv::Point2f>& dstPoints, const Eigen::SparseMatrix<float>& Weight) {

    std::vector<Eigen::Matrix2f> Rs;

    for (size_t i = 0; i < srcPoints.size(); i++) {
        Eigen::Vector2f srcOffset(srcPoints[i].x, srcPoints[i].y);
        Eigen::Vector2f dstOffset(dstPoints[i].x, dstPoints[i].y);

        // Compute the rotation matrix
        Eigen::Matrix2f H = dstOffset * srcOffset.transpose();
        Eigen::JacobiSVD<Eigen::Matrix2f> svd(H, Eigen::ComputeFullU | Eigen::ComputeFullV);
        Eigen::Matrix2f R = svd.matrixU() * svd.matrixV().transpose();

        Rs.push_back(R);
    }

    return Rs;
    /*

    std::vector<Eigen::Matrix2f> rotationMatrices(srcPoints.size());
    for (int i = 0; i < srcPoints.size(); i++) { // Iterate over the vertices
        const long numNeighbors = Weight.outerIndexPtr()[i+1] - Weight.outerIndexPtr()[i];
        // 当前点和邻居点的坐标差
        Eigen::MatrixXd P = Eigen::MatrixXd::Zero(2, numNeighbors);
        // 对角线元素为权值
        Eigen::MatrixXd D = Eigen::MatrixXd::Zero(numNeighbors, numNeighbors);
        // 当前目标点和邻居点的坐标差
        Eigen::MatrixXd P_prime = Eigen::MatrixXd::Zero(2, numNeighbors);

        int k = 0;
        double weight=0;
        for (Eigen::SparseMatrix<float>::InnerIterator it(Weight, i); it; ++it, ++k) { // Iterate over the neighbors

            P.col(k) = Eigen::Vector2d(srcPoints[i].x - srcPoints[it.index()].x, srcPoints[i].y - srcPoints[it.index()].y);
            D(k, k) = it.value();
            weight+=it.value();
            P_prime.col(k) = Eigen::Vector2d(dstPoints[i].x - dstPoints[it.index()].x, dstPoints[i].y - dstPoints[it.index()].y);
            D(k, k) = it.value();

        }

        // Normalize the diagonal of D
        double temp=0;
        for (int j = 0; j < numNeighbors; ++j) {
            D(j, j) /= weight;

        }
        std::cout << "P:\n" << P << std::endl;
        std::cout << "P_prime:\n" << P_prime << std::endl;


        // S, the covariance matrix
        Eigen::Matrix2d S = P *  D* P_prime.transpose();
        std::cout << "S (before symmetrization):\n" << S << std::endl;

        // SVD
        Eigen::JacobiSVD<Eigen::MatrixXd> svd(S, Eigen::ComputeThinU | Eigen::ComputeThinV);
        const Eigen::Matrix2d& U = svd.matrixU();
        const Eigen::Matrix2d& V = svd.matrixV();

        Eigen::Matrix2d I = Eigen::Matrix2d::Identity();
        I(1, 1) = (U * V.transpose()).determinant();

        // Add the rotation matrix R to the list
        Eigen::Matrix2d R = U * I * V.transpose();
        rotationMatrices[i] = R.cast<float>();
    }

    return rotationMatrices;*/
}

// 计算权重
Eigen::SparseMatrix<float> mesh::computeWeightMatrix(const std::vector<cv::Point2f>& points) {
    int sigma =80;
    int P_Num = points.size();
    Eigen::SparseMatrix<float> Weight(P_Num, P_Num);
    std::vector<Eigen::Triplet<float>> triplets;

    std::vector<float> rowSums(P_Num, 0.0f);
    for (int i = 0; i < P_Num; ++i) {

        for (int j = 0; j < P_Num; ++j) {
            if (i != j) {
                float dist = norm(points[i] - points[j]);
                float wij = std::exp(-dist * dist / (2 * sigma * sigma));
                if (wij>0.5){
                    triplets.emplace_back(i, j, wij);
                }
            }
        }
    }

    Weight.setFromTriplets(triplets.begin(), triplets.end());

/*
    //绘制权值矩阵及打印矩阵信息
    cv::Mat image = cv::imread("/Users/wuketian/project15/images/src3.png", cv::IMREAD_COLOR);
    for (int i = 0; i < P_Num; ++i) {
        for (int j = 0; j < P_Num; ++j) {
            if (i != j) {
                int index_i = i;
                int index_j = j;
                for (Eigen::SparseMatrix<float>::InnerIterator it(Weight, index_i); it; ++it) {
                    if (it.index() == index_j) {
                        std::cout << "Weight between point " << points[i].x << "," <<  points[i].y
                                  << " and point " << points[j].x << "," <<  points[j].y << ": " << it.value() << std::endl;
                        for (size_t i = 0; i < points.size(); ++i) {
                            cv::Mat img=image.clone();
                            cv::circle(img, points[i], 2, cv::Scalar(255 * (1 - Weight.coeff(i, i)), 0, 255 * Weight.coeff(i, i)), -1);
                            for (Eigen::SparseMatrix<float>::InnerIterator it(Weight, i); it; ++it) {
                                int j = it.index();
                                cv::line(img, points[i], points[j], cv::Scalar(0,0, 255, 1));
                                cv::circle(img, points[j], 7*it.value(), cv::Scalar(0, 0, 255 ,1 ),-1); // 使用白色绘制目标点
                            }
                            cv::imshow("weightPoints",img);
                            cv::waitKey(0);
                        }

                        break; // Exit the inner loop once the weight is found
                    }
                }
            }
        }
    }
    std::cout << "非零元素数量: " << Weight.nonZeros() << std::endl;
    // 查看权值范围
    float minCoeff = std::numeric_limits<float>::max();
    float maxCoeff = std::numeric_limits<float>::min();
    for (int k = 0; k < Weight.outerSize(); ++k) {
        for (Eigen::SparseMatrix<float>::InnerIterator it(Weight, k); it; ++it) {
            float val = it.value();
            if (val < minCoeff) minCoeff = val;
            if (val > maxCoeff) maxCoeff = val;
        }
    }
    std::cout << "Weight range: [" << minCoeff << ", " << maxCoeff << "]" << std::endl;


    // 打印邻居节点
    std::vector<int> neighborCounts(Weight.rows(), 0);
    for (int k = 0; k < Weight.outerSize(); ++k) {
        for (Eigen::SparseMatrix<float>::InnerIterator it(Weight, k); it; ++it) {
            neighborCounts[it.row()]++;
        }
    }

    double mean = std::accumulate(neighborCounts.begin(), neighborCounts.end(), 0.0) / neighborCounts.size();
    double variance = 0.0;
    for (const auto& count : neighborCounts) {
        variance += (count - mean) * (count - mean);
    }
    variance /= neighborCounts.size();

    std::cout << "邻居节点状态:" << std::endl;
    std::cout << "每个点的邻居节点数量均值: " << mean << std::endl;
    std::cout << "方差: " << variance << std::endl;
*/
    return Weight;

}

// 计算刚性值
double mesh::calculateRigidityEnergy(const std::vector<cv::Point2f>& srcPoints, const std::vector<cv::Point2f>& dstPoints, const std::vector<Eigen::Matrix2f>& rotations, const Eigen::SparseMatrix<float>& Weight) {
    double energy = 0.0;

    for (int i = 0; i < srcPoints.size(); ++i) {
        for (Eigen::SparseMatrix<float>::InnerIterator it(Weight, i); it; ++it) {
            int j = it.index();
            Eigen::Vector2f p_i(srcPoints[i].x, srcPoints[i].y);
            Eigen::Vector2f p_j(srcPoints[j].x, srcPoints[j].y);
            Eigen::Vector2f q_i(dstPoints[i].x, dstPoints[i].y);
            Eigen::Vector2f q_j(dstPoints[j].x, dstPoints[j].y);

            Eigen::Vector2f p_ij = p_i - p_j;
            Eigen::Vector2f q_ij = q_i - q_j;
            Eigen::Vector2f diff = q_ij - rotations[i] * p_ij;
            energy += it.value() * diff.squaredNorm();
        }
    }

    return energy;
}

void mesh::deformMesh(const std::vector<cv::Point2f>& srcPoints, std::vector<cv::Point2f>& dstPoints, const Eigen::SparseMatrix<float>& Weight, int iterations, cv::Mat& img1) {
    int P_Num = srcPoints.size();
    std::vector<Eigen::Matrix2f> rotations = estimateRotations(srcPoints, dstPoints, Weight);
    // 绘制旋转矩阵结果
    for (size_t i = 0; i < dstPoints.size(); ++i) {
        // 计算旋转后的点
        Eigen::Vector2f p_i(srcPoints[i].x, srcPoints[i].y);
        Eigen::Vector2f rotatedVec = rotations[i] * p_i;
        cv::Point rotated_p_i(rotatedVec.x(), rotatedVec.y());
        // 绘制连线
        cv::line(img1, srcPoints[i],  rotated_p_i, cv::Scalar(0, 0, 255), 2);
        // 绘制原点
        circle(img1, dstPoints[i], 3, cv::Scalar(0, 255, 0), cv::FILLED);
        // 绘制旋转后的点
        circle(img1, rotated_p_i, 3, cv::Scalar(255, 0, 0), cv::FILLED);
    }
    imshow("Rotation Result", img1);
    cv::waitKey(0);
    for (int iter = 0; iter < iterations; ++iter) {
        // 计算旋转矩阵
        rotations = estimateRotations(srcPoints, dstPoints, Weight);
        Eigen::MatrixXf rhs = Eigen::MatrixXf::Zero(P_Num, 2);
        for (int i = 0; i < P_Num; ++i) {
            for (Eigen::SparseMatrix<float>::InnerIterator it(Weight, i); it; ++it) {
                int j = it.index();
                Eigen::Vector2f p_i(srcPoints[i].x, srcPoints[i].y);
                Eigen::Vector2f p_j(srcPoints[j].x, srcPoints[j].y);
                Eigen::Vector2f q_i(dstPoints[i].x, dstPoints[i].y);
                Eigen::Vector2f q_j(dstPoints[j].x, dstPoints[j].y);

                // 能量函数为
                // rhs.row(i) += it.value() * (rotations[i] * (p_i - p_j) - q_j)* (rotations[i] * (p_i - p_j) - q_j);
                rhs.row(i) += it.value() * (rotations[i] * (p_i - p_j) + q_j).transpose();
            }
        }

        // LU分解权值矩阵
        Eigen::SparseMatrix<float> L = Weight;
        for (int i = 0; i < P_Num; ++i) {
            float sum_w = 0.0;
            for (Eigen::SparseMatrix<float>::InnerIterator it(Weight, i); it; ++it) {
                sum_w += it.value();
            }
            L.coeffRef(i, i) = sum_w;
        }

        Eigen::SparseLU<Eigen::SparseMatrix<float>> solver;
        solver.compute(L);
        if (solver.info() != Eigen::Success) {
            std::cout << "Decomposition failed!" << std::endl;
            return;
        }

        Eigen::MatrixXf newDstPoints = solver.solve(rhs);
        if (solver.info() != Eigen::Success) {
            std::cout << "Solving failed!" << std::endl;
            return;
        }

        for (int i = 0; i < P_Num; ++i) {
            dstPoints[i] = cv::Point2f(newDstPoints(i, 0), newDstPoints(i, 1));
        }

        cv::Mat img = cv::imread("/Users/wuketian/project15/images/src3.png", cv::IMREAD_COLOR);


        // 计算并输出刚性能量
        double energy = calculateRigidityEnergy(srcPoints, dstPoints, rotations, Weight);
        std::cout << "Iteration " << iter << ": rigidity energy = " << energy << std::endl;

}


}

void mesh::buildWeightMatrix(const std::vector<cv::Point2f>& srcPoints, Eigen::SparseMatrix<float>& Weight) {
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

// 计算两个点之间的欧几里得距离
double mesh::calculateDistance(const cv::Point2d& a, const cv::Point2d& b) {
    return std::sqrt(std::pow(a.x - b.x, 2) + std::pow(a.y - b.y, 2));
}

// 找到最近的三个点
std::map<double, cv::Point2d> mesh::findNearestNeighbors(const std::map<std::pair<int, int>, std::pair<int, int>>& offsetMap, const cv::Point2d& pt, int k = 3) {
    std::map<double, cv::Point2d> neighbors;
    std::vector<std::pair<double, cv::Point2d>> distances;

    // 计算距离并将距离和位置插入到容器中
    for (const auto& entry : offsetMap) {
        cv::Point2d pos(entry.first.first, entry.first.second);
        double dist = calculateDistance(pt, pos);
        distances.emplace_back(dist, pos); // 距离，位置
    }

    // 对距离进行排序
    std::sort(distances.begin(), distances.end(), [](const auto& a, const auto& b) {
        return a.first < b.first;
    });

    // 取前 k 个最近的邻居
    for (int i = 0; i < k && i < distances.size(); ++i) {
        neighbors[distances[i].first] = distances[i].second;
    }

    return neighbors;
}

// 插值计算新的偏移量
cv::Point2f mesh::interpolateOffset(std::map<double,cv::Point2d>& neighbors, const std::map<std::pair<int, int>, std::pair<int, int>>& offsetMap,float x, float y) {
    float weightSum = 0.0;
    std::pair<int, int> offset ;
    float offsetX = 0.0;
    float offsetY = 0.0;

    for (const auto& neighbor : neighbors) {
        double distance = calculateDistance(cv::Point2d(x,y), neighbor.second);
        cv::Point2d neighborPos = neighbor.second;
        std::pair<int, int> posPair= std::make_pair(neighborPos.x,neighborPos.y);
        // 获取偏移量
        if (distance == 0){
            // std::cout << "没有偏移" <<std::endl;
            return cv::Point2d(0,0);
        }
        else{
            // std::cout << "可以加权" <<std::endl;
            // 使用反距离加权法计算权重
            float wij = 1.0f / (distance+(1e-6));
            weightSum += wij;
            // 在 map 中查找对应的值
            auto it = offsetMap.find(posPair);
            if (it != offsetMap.end()) {
                // 找到了对应的值
                std::pair<int, int> offsetPair = it->second;
                offsetX += offsetPair.first * wij;
                offsetY += offsetPair.second * wij;
            }

        }
    }
    if (offsetX == 0 && offsetY == 0){return cv::Point2d(0,0);}
    else{
    // std::cout << "最终计算偏移量为：" << offsetX / weightSum << "," <<  offsetY / weightSum<< std::endl << std::endl;
    // std::cout << "offset：" << offsetX << "," << offsetY << std::endl;
    // std::cout << "weightSum：" << weightSum << std::endl;
    return cv::Point2f(offsetX / weightSum, offsetY / weightSum);
    }
}

void mesh::calculateOffsets(
    const std::vector<cv::Point2f>& srcPoints,
    const std::map<std::pair<int, int>, std::pair<int, int>> & offsetMap,
    std::vector<cv::Point2f>& dstPoints)
{

    for (const auto& pt : srcPoints) {
        float x = pt.x;
        float y = pt.y;
        std::map<double, cv::Point2d> neighbors = findNearestNeighbors(offsetMap,cv::Point2d(x,y));
        cv::Point2f offset = interpolateOffset(neighbors, offsetMap,x, y);
        dstPoints.push_back(cv::Point2f(x + offset.x,y + offset.y));
    }
}

// 应用薄板样条变换
void mesh::applyThinPlateSplineTransform(cv::Mat& warpImage, const cv::Mat& src, const std::vector<cv::Point2f>& srcPts, const std::vector<cv::Point2f>& dstPts) {
    // 使用薄板样条变换进行图像变形
    // 创建Thin Plate Spline形状变换器
    cv::Ptr<cv::ThinPlateSplineShapeTransformer> tps = cv::createThinPlateSplineShapeTransformer();

    // 将匹配点对转换为DMatch
    std::vector<cv::DMatch> matches;
    for (size_t i = 0; i < srcPts.size(); ++i) {
        matches.emplace_back(i, i, 0);
    }

    // 估计变换
    tps->estimateTransformation(srcPts, dstPts, matches);

    // 应用变换到图像
    tps->warpImage(src, warpImage);
}

void mesh::warpMeshGrid(const cv::Mat& img1, const cv::Mat& img2,
             std::map<std::pair<int, int>, std::pair<int, int>> & featurePairs) {


    // 定义网格大小
    cv::Mat deformedImg = img1.clone();
    int rows = 16;
    int cols = 16;

    // 初始化原点
    std::vector<cv::Point2f> srcPoints = divideIntoGrids(img1, rows, cols);

    // 初始化目标点
    std::vector<cv::Point2f> dstPoints;
    calculateOffsets(srcPoints, featurePairs, dstPoints);
    std::cout<<dstPoints<<std::endl;

    // 计算权值矩阵
    Eigen::SparseMatrix<float> Weight = computeWeightMatrix(srcPoints);

    // 检查权重矩阵是否合理
    // visualizeWeightMatrix(Weight, srcPoints.size());
    // printWeightMatrixStats(Weight);

    // 计算刚性能量
    //dstPoints=srcPoints;
    //dstPoints[1].y+=20;

    int iterations = 6;
    cv::Mat imgWithPoints = img1.clone();
    deformMesh(srcPoints, dstPoints, Weight, iterations, imgWithPoints);// 更新dstPoints

    cv::Mat dst = img1.clone();
    cv::Mat warpedImage=img2.clone();
    std::cout<<dstPoints<<std::endl;
    cv::Mat img1WithGrid = img2.clone();


    for (int i = 0; i < srcPoints.size(); ++i) {
        cv::circle( dst, srcPoints[i], 2, cv::Scalar(0), -1);
        std::cout<<"原点是："<<dstPoints[i]<<std::endl;
        cv::circle(img1WithGrid, dstPoints[i], 2, cv::Scalar(0), -1);
        std::cout<<"对应点是："<<srcPoints[i]<<std::endl;
        cv::imshow("1",dst);
        cv::imshow("2",img1WithGrid);
        cv::waitKey(0);
    }

    cv::imshow("points",dst);
    cv::waitKey(0);

    // applyThinPlateSplineTransform(dst, img1, srcPoints, dstPoints);


    /*

    // 生成Delaunay三角剖分
    cv::Rect rect(0, 0, width, height);
    std::vector<cv::Vec6f> triangles = generateDelaunayTriangles(rect, gridPoints1);

    // 使用Thin Plate Spline进行形变
    cv::Mat tps = cv::findHomography(points1, points2, cv::RANSAC);

    // 应用形变到每个网格点
    for (auto& point : gridPoints2) {
        std::vector<cv::Point2f> src = {point};
        std::vector<cv::Point2f> dst;
        cv::perspectiveTransform(src, dst, tps);
        point = dst[0];
    }

    // 生成中间状态的网格点
    std::vector<cv::Point2f> midPoints(gridPoints1.size());
    float alpha = 0.5; // 控制中间状态的比例，0.0为初始状态，1.0为变形后状态
    for (size_t i = 0; i < gridPoints1.size(); ++i) {
        midPoints[i] = (1.0 - alpha) * gridPoints1[i] + alpha * gridPoints2[i];
    }


    // 在原图上绘制初始网格点
    cv::Mat img1WithGrid = img1.clone();
    for (const auto& point : gridPoints1) {
        cv::circle(img1WithGrid, point, 2, cv::Scalar(0), -1);
    }

    // 在原图上绘制中间状态的网格点
    cv::Mat imgMidWithGrid = img1.clone();
    for (const auto& point : midPoints) {
        cv::circle(imgMidWithGrid, point, 2, cv::Scalar(0), -1);
    }
    cv::Mat img1WithTriangles = img1.clone();
    for (auto& t : triangles) {
        if (!isTriangleInWhiteArea(t, img1)) {
            cv::line(img1WithTriangles, cv::Point2f(t[0], t[1]), cv::Point2f(t[2], t[3]), cv::Scalar(0, 255, 0), 1);
            cv::line(img1WithTriangles, cv::Point2f(t[2], t[3]), cv::Point2f(t[4], t[5]), cv::Scalar(0, 255, 0), 1);
            cv::line(img1WithTriangles, cv::Point2f(t[4], t[5]), cv::Point2f(t[0], t[1]), cv::Scalar(0, 255, 0), 1);
        }
    }

    // 生成形变后的图像
    cv::Mat warpedImg = cv::Mat::zeros(height, width, 1);

    for (auto& t : triangles) {
        if (!isTriangleInWhiteArea(t, img1)) {
            std::vector<cv::Point2f> srcTri(3), dstTri(3);
            for (int i = 0; i < 3; ++i) {
                srcTri[i] = cv::Point2f(t[i * 2], t[i * 2 + 1]);
                auto it = std::find(gridPoints1.begin(), gridPoints1.end(), srcTri[i]);
                if (it != gridPoints1.end()) {
                    dstTri[i] = gridPoints2[std::distance(gridPoints1.begin(), it)];
                }
            }
            // applyAffineTransform(warpedImg, img1, srcTri, dstTri);
            applyThinPlateSplineTransform(warpedImg, img1, srcTri, dstTri);

        }
    }

    // 绘制形变后的网格
    drawMeshGrid(warpedImg, triangles, gridPoints1, gridPoints2);

    // cv::imshow("img1WithTriangles", img1WithTriangles);
    cv::imshow("warpedImg", warpedImg);
    cv::waitKey(0);
*/
}

