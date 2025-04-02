#include "laplaciandeform.h"

laplaciandeform::laplaciandeform() {

}

bool laplaciandeform::isInWhiteArea(const cv::Point2f& point, const cv::Mat& img,int cellSize) {
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
    return whiteRatio > 0.94;
}

std::vector<cv::Point2f> laplaciandeform::initializeGrids(const cv::Mat& img, int gridSize) {
    std::vector<cv::Point2f> points;
    for (int i = 0; i<=img.rows; i+=gridSize) {
        for (int j = 0; j <= img.cols; j+=gridSize) {
            // if(!isInWhiteArea(cv::Point2f(j, i),img,gridSize)){points.emplace_back(j, i);}
            // else{continue;}
            points.emplace_back(j, i);
        }
    }
    return points;
}

float laplaciandeform::bilinearInterpolation(const cv::Mat& img, int x, int y) {

    // 获取四个相邻像素的坐标
    int x1 = static_cast<int>(std::floor(x));
    int x2 = x1 + 1;
    int y1 = static_cast<int>(std::floor(y));
    int y2 = y1 + 1;

    // 边界处理
    x1 = std::max(0, std::min(x1, img.cols - 1));
    x2 = std::max(0, std::min(x2, img.cols - 1));
    y1 = std::max(0, std::min(y1, img.rows - 1));
    y2 = std::max(0, std::min(y2, img.rows - 1));

    int pixelValue1 = static_cast<int>(img.at<uchar>(y1, x1));
    int pixelValue2 = static_cast<int>(img.at<uchar>(y1, x2));
    int pixelValue3 = static_cast<int>(img.at<uchar>(y2, x1));
    int pixelValue4 = static_cast<int>(img.at<uchar>(y2, x2));


    return (pixelValue1+pixelValue2+pixelValue3+pixelValue4)/4;
}

// 实现双线性插值法
cv::Mat laplaciandeform::applybilinearDeform(const cv::Mat& img, const int gridSize, const std::vector<cv::Point2f>& srcPoints, const std::vector<cv::Point2f>& dstPoints) {
    cv::Mat transformedImage(img.size(), img.type());
    cv::Mat resultImage = cv::Mat::zeros(img.size(), CV_8UC1);

    for (size_t i = 0; i < srcPoints.size(); ++i) {
        // 获取当前目标点在原图中的坐标
        float x = srcPoints[i].x;
        float y = srcPoints[i].y;
        float dstx = dstPoints[i].x;
        float dsty = dstPoints[i].y;

        // 划分当前小块为更小的网格单元
        int subGridSize = 64; // 每个小块的网格细分大小
        for (int dy = 0; dy < subGridSize; ++dy) {
            for (int dx = 0; dx < subGridSize; ++dx) {
                int subX = x + dx * (gridSize / subGridSize);
                int subY = y + dy * (gridSize / subGridSize);

                // 双线性插值计算在原始图像中的像素值

                float pixelValue = bilinearInterpolation(img, subX, subY);
                dstx+=gridSize;
                dsty+=gridSize;
                std::cout<<pixelValue<<std::endl;

                // 在结果图像中填入像素值
                resultImage.at<uchar>(dsty, dstx) = pixelValue;
            }
        }
    }

    return resultImage;
}

// 计算仿射变换矩阵
cv::Mat laplaciandeform::computeAffineTransformLeastSquares(std::vector<cv::Point2f>& srcPoints,std::vector<cv::Point2f>& dstPoints){
    if (srcPoints.size() < 3 || dstPoints.size() < 3) {
        throw std::invalid_argument("需要至少三个源点和目标点来计算仿射变换矩阵");
    }
    return cv::estimateAffine2D(srcPoints, dstPoints);
}

cv::Mat laplaciandeform::applyAffineTransform(const cv::Mat& srcImage, const cv::Mat& transformMatrix, const cv::Size& outputSize) {
    cv::Mat dstImage;
    cv::warpAffine(srcImage, dstImage, transformMatrix, outputSize);
    return dstImage;
}

// 应用形变
cv::Mat laplaciandeform::applyDeform(const cv::Mat& img,const int gridSize,const std::vector<cv::Point2f>& srcPoints,const std::vector<cv::Point2f>& dstPoints){

    // 得到4个对应点
    cv::Mat transformedImage(img.size(), img.type());
    int numBlocksX = img.cols / gridSize;// 列块数
    int numBlocksY  = img.rows / gridSize;// 行块数
    std::vector<cv::Mat> blocks;
    cv::Mat resultImage = cv::Mat::zeros(800,800, CV_8UC1);

    int index=0;

    for (int y = 0; y < numBlocksY; ++y) {
        for (int x = 0; x < numBlocksX; ++x) {
            cv::Mat blockSrc = img(cv::Rect(x * gridSize, y * gridSize, gridSize, gridSize));
            cv::Mat blockDst = transformedImage(cv::Rect(x * gridSize, y * gridSize, gridSize, gridSize));
            cv::Point2f rightPoint;// 初始化其余三个目标点
            cv::Point2f bottomPoint;
            cv::Point2f brPoint;
            cv::Point2f rightdstPoint;
            cv::Point2f bottomdstPoint;
            cv::Point2f brdstPoint;
            std::vector<cv::Point2f> srcpointsV;// 四个点的集合
            srcpointsV.clear();
            std::vector<cv::Point2f> dstpointsV;
            dstpointsV.clear();
            // 右侧点
            rightPoint = srcPoints[index + 1];
            rightdstPoint = dstPoints[index+1];
            // 下方点
            bottomPoint = srcPoints[(y + 1) * (numBlocksX + 1) + x];
            bottomdstPoint = dstPoints[(y + 1) * (numBlocksX + 1) + x];

            std::cout<<srcPoints[index]<<std::endl;
            // 右下方点
            brPoint = srcPoints[(y + 1) * (numBlocksX + 1) + x+1];
            brdstPoint = dstPoints[(y + 1) * (numBlocksX + 1) + x+1];

            // 计算当前小块的仿射变换矩阵 ,rightPoint,bottomPoint
            srcpointsV.push_back(srcPoints[index]);
            srcpointsV.push_back(rightPoint);
            srcpointsV.push_back(bottomPoint);
            srcpointsV.push_back(brPoint);

            dstpointsV.push_back(dstPoints[index]);
            dstpointsV.push_back(rightdstPoint);
            dstpointsV.push_back(bottomdstPoint);
            dstpointsV.push_back(brdstPoint);
            std::cout<<"X："<<x<<"Y:"<<y<<std::endl;
            std::cout<<"当前块是："<<index<<",坐标是"<<srcPoints[index]<<","<<rightPoint<<","<<bottomPoint<<","<<brPoint<<","<<std::endl;
            // 计算透视变换
            //for(auto& s:srcpointsV){std::cout<<"原点是："<<s<<std::endl;}
            //for(auto& s:dstpointsV){std::cout<<"目标点是："<<s<<std::endl;}
            cv::Mat transformMatrix = getPerspectiveTransform(srcpointsV,dstpointsV,cv::DECOMP_LU);
            cv::warpPerspective(blockSrc, blockDst, transformMatrix,blockSrc.size(),cv::INTER_LINEAR,0,1);

            // 填入原图
            cv::Rect roi(dstPoints[index].x,dstPoints[index].y, gridSize, gridSize);
            // 将当前小块复制到原始图像中对应的位置
            blockDst.copyTo(resultImage(roi));

            cv::imshow("Transformed Image",blockDst);
            cv::waitKey(0);
            index++;
        }
    }


}


void laplaciandeform::applyThinPlateSplineTransform(cv::Mat& warpImage, const cv::Mat& src, const std::vector<cv::Point2f>& srcPts, const std::vector<cv::Point2f>& dstPts) {
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


// 创建拉普拉斯矩阵
Eigen::SparseMatrix<double> laplaciandeform::createLaplacianMatrix(const std::vector<cv::Point2f>& points, const std::vector<std::pair<int, int>>& edges) {
    int n = points.size();
    Eigen::SparseMatrix<double> L(n, n);

    std::vector<Eigen::Triplet<double>> tripletList;

    for (const auto& edge : edges) {
        int i = edge.first;
        int j = edge.second;
        tripletList.push_back(Eigen::Triplet<double>(i, j, -1));
        tripletList.push_back(Eigen::Triplet<double>(j, i, -1));
        tripletList.push_back(Eigen::Triplet<double>(i, i, 1));
        tripletList.push_back(Eigen::Triplet<double>(j, j, 1));
    }

    L.setFromTriplets(tripletList.begin(), tripletList.end());

    return L;
}

// 初始化边
std::vector<std::pair<int, int>> laplaciandeform::initializeEdges(const std::vector<cv::Point2f>& points, int imgRows, int imgCols, int gridSize) {
    std::vector<std::pair<int, int>> edges;
    int numCols = imgCols / gridSize;
    int numRows = imgRows / gridSize;

    // 创建点的索引映射
    std::unordered_map<int, std::unordered_map<int, int>> pointIndexMap;
    for (int idx = 0; idx < points.size(); ++idx) {
        int x = points[idx].x;
        int y = points[idx].y;
        pointIndexMap[y][x] = idx;
    }

    // 生成边
    for (const auto& point : points) {
        int x = point.x;
        int y = point.y;

        if (pointIndexMap[y].count(x + gridSize)) { // 右边的点
            edges.push_back({pointIndexMap[y][x], pointIndexMap[y][x + gridSize]});
        }
        if (pointIndexMap.count(y + gridSize) && pointIndexMap[y + gridSize].count(x)) { // 下边的点
            edges.push_back({pointIndexMap[y][x], pointIndexMap[y + gridSize][x]});
        }
    }

    return edges;
}

// 计算两个点之间的欧几里得距离
double laplaciandeform::calculateDistance(const cv::Point2d& a, const cv::Point2d& b) {
    return std::sqrt(std::pow(a.x - b.x, 2) + std::pow(a.y - b.y, 2));
}

// 找到最近的三个点
std::map<double, cv::Point2d> laplaciandeform::findNearestNeighbors(const std::map<std::pair<int, int>, std::pair<int, int>>& offsetMap, const cv::Point2d& pt, int k = 3) {
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
cv::Point2f laplaciandeform::interpolateOffset(std::map<double,cv::Point2d>& neighbors, const std::map<std::pair<int, int>, std::pair<int, int>>& offsetMap,float x, float y) {
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
        else if(distance>100){continue;}
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
        return cv::Point2f(offsetX / weightSum, offsetY / weightSum);
    }
}

// 根据特征点映射到目标网格上 返回对应点dstPoints的坐标
void laplaciandeform::calculateOffsets(
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

// 应用形变
std::vector<cv::Point2f> laplaciandeform::deformGrid(const std::vector<cv::Point2f>& points,
                                                     const std::vector<std::pair<int, int>>& edges ,
                                                     const std::unordered_map<int, cv::Point2f>& fixedPoints,
                                                     const std::unordered_map<int, cv::Point2f>& movePoints) {

    int n = points.size();
    Eigen::SparseMatrix<double> L = createLaplacianMatrix(points, edges);
    Eigen::SparseMatrix<double> A = L.transpose() * L;
    Eigen::MatrixXd B = Eigen::MatrixXd::Zero(n, 2);


    // 处理移动锚点
    for (const auto& ma : movePoints) {
        int i = ma.first;
        cv::Point2f pos = ma.second;

        // 添加约束，使移动锚点向目标位置移动
        A.coeffRef(i, i) += 1;
        B(i, 0) += pos.x;
        B(i, 1) += pos.y;
    }


    // 处理固定锚点
    for (const auto& fp : fixedPoints) {
        int i = fp.first;
        cv::Point2f pos = fp.second;

        // 将固定点的行和列清零
        for (Eigen::SparseMatrix<double>::InnerIterator it(A, i); it; ++it) {
            A.coeffRef(it.row(), i) = 0;
        }
        for (Eigen::SparseMatrix<double>::InnerIterator it(A, i); it; ++it) {
            A.coeffRef(i, it.col()) = 0;
        }

        // 设置对角线元素为1
        A.coeffRef(i, i) = 1;

        // 设置B中的对应值
        B(i, 0) = pos.x;
        B(i, 1) = pos.y;
    }


    Eigen::SimplicialCholesky<Eigen::SparseMatrix<double>> solver(A);
    Eigen::MatrixXd X = solver.solve(B);

    std::vector<cv::Point2f> deformedPoints(points.size());
    for (int i = 0; i < n; ++i) {
        deformedPoints[i] = cv::Point2f(X(i, 0), X(i, 1));
    }

    return deformedPoints;
}

std::unordered_map<int, cv::Point2f> laplaciandeform::setMovePoint(const std::vector<cv::Point2f> srcPoints,std::map<std::pair<int, int>, std::pair<int, int>> offsetMap){
    int i = 0;// 索引值
    std::unordered_map<int, cv::Point2f> movePoints;
    for (const auto& pt : srcPoints) {
        float x = pt.x;
        float y = pt.y;
        double dist;
        std::map<double, cv::Point2d> neighbors = findNearestNeighbors(offsetMap,cv::Point2d(x,y));
        cv::Point2f offset = interpolateOffset(neighbors, offsetMap,x, y);
        // std::cout<<"当前点为："<<pt<<std::endl;
        // std::cout<<"偏移量为："<<offset<<std::endl;
        movePoints[i]=(cv::Point2f(x + 0.6*offset.x,y + 0.6*offset.y));
        i++;
    }
    return movePoints;
}

std::unordered_map<int, cv::Point2f> laplaciandeform::setMovePoint(const std::vector<cv::Point2f> dstPoints){
    int i = 0;// 索引值
    std::unordered_map<int, cv::Point2f> movePoints;
    for (const auto& pt : dstPoints) {
        movePoints[i]=(cv::Point2f(pt.x ,pt.y));
        i++;
    }
    return movePoints;
}




// 计算偏移量
std::map<std::pair<int, int>, std::pair<int, int>>  laplaciandeform::offset(const std::vector<cv::Point2f> srcPoints,const std::vector<cv::Point2f> dstPoints){
    std::map<std::pair<int, int>, std::pair<int, int>>  offsets;
    for (int i=0;i<srcPoints.size();i++) {

        int offsetX = srcPoints[i].x - dstPoints[i].x;
        int offsetY = srcPoints[i].y - dstPoints[i].y;

        offsets[std::make_pair(static_cast<int>(srcPoints[i].x), static_cast<int>(srcPoints[i].y))]=
            (std::make_pair(static_cast<int>(offsetX), static_cast<int>(offsetY)));
    }
    return offsets ;
}


std::unordered_map<int, cv::Point2f> laplaciandeform::setFixedPoint(cv::Mat& img,const std::vector<cv::Point2f> points,int gridSize){
    int rows = img.rows;
    int cols = img.cols;
    std::cout<<"rows:"<<rows<<std::endl;
    std::cout<<"cols:"<<cols<<std::endl;
    std::unordered_map<int, cv::Point2f> fixedPoints;
    int i = 0;

    for (const auto& pt : points) {
        if (pt.x == 0 || pt.x == cols  || pt.y == 0 || pt.y == rows ) {
            fixedPoints[i] = cv::Point2f(pt.x, pt.y);
            std::cout<<"检测到的边界点：("<<pt.x<<","<<pt.y<<")"<<std::endl;
        }
        std::cout<<"非边界点：("<<pt.x<<","<<pt.y<<")"<<std::endl;
        i++;
    }

    return fixedPoints;
}

std::vector<cv::Point2f> laplaciandeform::deform(cv::Mat& img,int gridSize,std::map<std::pair<int, int>, std::pair<int, int>> matchMap){
    std::vector<cv::Point2f> points = initializeGrids(img, gridSize);
    std::vector<std::pair<int, int>> edges = initializeEdges(points, img.rows, img.cols, gridSize);

    const std::unordered_map<int, cv::Point2f>& movePoints = setMovePoint(points,matchMap);
    std::unordered_map<int, cv::Point2f> fixedPoints;
    int i=0;
    std::vector<cv::Point2f> dstPoints = deformGrid(points,edges,fixedPoints,movePoints);
    /***
     * cv::Mat warpImg = applybilinearDeform(img, gridSize,  points,  dstPoints);
    cv::Mat warpImg;
    cv::Mat img1 = cv::imread("/Users/wuketian/project15/images/src3.png",  cv::IMREAD_COLOR);
    applyThinPlateSplineTransform(warpImg,img1,dstPoints,points);
    cv::imshow("res",warpImg);
    cv::waitKey(0);
***/


    return deformGrid(points,edges,fixedPoints,movePoints);
}

