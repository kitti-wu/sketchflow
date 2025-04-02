#ifndef LAPLACIANDEFORM_H
#define LAPLACIANDEFORM_H
#include <iostream>
#include <vector>
#include <eigen3/Eigen/Sparse>
#include <eigen3/Eigen/Eigenvalues>
#include <opencv2/opencv.hpp>
#include <opencv2/core/types.hpp>
#include <opencv2/core.hpp>
#include <opencv2/imgproc.hpp>
#include <opencv2/shape/shape_transformer.hpp>

class laplaciandeform
{
public:
    laplaciandeform();

    // 初始化
    std::vector<cv::Point2f> initializeGrids(const cv::Mat& img, int gridSize);
    std::vector<std::pair<int, int>> initializeEdges(const std::vector<cv::Point2f>& points, int imgRows, int imgCols, int gridSize);
    Eigen::SparseMatrix<double> createLaplacianMatrix(const std::vector<cv::Point2f>& points, const std::vector<std::pair<int, int>>& edges);

    // 检测区域是否为白色
    bool isInWhiteArea(const cv::Point2f& point, const cv::Mat& img,int cellSize);

    // 将匹配关系映射到网格上的相关函数
    double calculateDistance(const cv::Point2d& a, const cv::Point2d& b);
    cv::Point2f interpolateOffset(std::map<double,cv::Point2d>& neighbors, const std::map<std::pair<int, int>, std::pair<int, int>>& offsetMap,float x, float y);
    std::map<double, cv::Point2d> findNearestNeighbors(const std::map<std::pair<int, int>, std::pair<int, int>>& offsetMap, const cv::Point2d& pt, int k );
    void calculateOffsets(
        const std::vector<cv::Point2f>& srcPoints,
        const std::map<std::pair<int, int>, std::pair<int, int>> & offsetMap,
        std::vector<cv::Point2f>& dstPoints);
    std::unordered_map<int, cv::Point2f> setFixedPoint(cv::Mat& img,const std::vector<cv::Point2f> points,int gridSize);
    std::unordered_map<int, cv::Point2f> setMovePoint(const std::vector<cv::Point2f> srcPoints,std::map<std::pair<int, int>, std::pair<int, int>> offsetMap);
    std::unordered_map<int, cv::Point2f> setMovePoint(const std::vector<cv::Point2f> dstPoints);
    // 形变
    std::vector<cv::Point2f> deformGrid(const std::vector<cv::Point2f>& points,
                                        const std::vector<std::pair<int, int>>& edges,
                                        const std::unordered_map<int, cv::Point2f>& fixedPoints,
                                        const std::unordered_map<int, cv::Point2f>& movePoints);
    std::vector<cv::Point2f> deform(cv::Mat& img,int gridSize,std::map<std::pair<int, int>, std::pair<int, int>> matchMap);
    cv::Mat computeAffineTransformLeastSquares(std::vector<cv::Point2f>& srcPoints,std::vector<cv::Point2f>& dstPoints);
    cv::Mat applyAffineTransform(const cv::Mat& srcImage, const cv::Mat& transformMatrix, const cv::Size& outputSize) ;
    cv::Mat dstImage (cv::Mat& srcImage,cv::Mat& transformMatrix, int size);
    cv::Mat applyDeform(const cv::Mat& img,const int girdSize,const std::vector<cv::Point2f>& srcPoints,const std::vector<cv::Point2f>& dstPoints);
    cv::Mat applybilinearDeform(const cv::Mat& img, const int gridSize, const std::vector<cv::Point2f>& srcPoints, const std::vector<cv::Point2f>& dstPoints);
    float bilinearInterpolation(const cv::Mat& img, int x,int y);
    std::map<std::pair<int, int>, std::pair<int, int>> offset(const std::vector<cv::Point2f> srcPoints,
                                                              const std::vector<cv::Point2f> dstPoints);
    void applyThinPlateSplineTransform(cv::Mat& warpImage, const cv::Mat& src, const std::vector<cv::Point2f>& srcPts, const std::vector<cv::Point2f>& dstPts) ;



};

#endif // LAPLACIANDEFORM_H
