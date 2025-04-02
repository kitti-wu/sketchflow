#ifndef ARAP_H
#define ARAP_H

#define EIGEN_DONT_VECTORIZE
#include <eigen3/Eigen/Dense>
#include <eigen3/Eigen/SparseCore>
#include <eigen3/Eigen/SparseLU>
#include <opencv2/opencv.hpp>
#include <eigen3/Eigen/Dense>
#include <vector>
#include <utility>



class ARAP {
public:
    ARAP();
    cv::Mat deform(const cv::Mat& img, int cellSize, std::map<std::pair<int, int>, std::pair<int, int>> matches);
    std::vector<cv::Point2f> deform(std::vector<cv::Point2f> srcPoints);
    Eigen::Matrix2f computeLocalTransformation(const std::vector<cv::Point2f>& srcPoints, const std::vector<cv::Point2f>& dstPoints) ;
    std::vector<cv::Point2f> applyTransformation(const std::vector<cv::Point2f>& srcPoints, const std::vector<cv::Point2f>& dstPoints,int cols);

private:
    std::vector<cv::Rect> divideIntoGrids(const cv::Mat& img, int cellSize);
    std::vector<cv::Point2f> divideBlockIntoGrid(const cv::Rect& block,int cellSize);
    std::vector<cv::Point2f> performARAP(const std::vector<cv::Point2f>& srcPoints, const std::vector<cv::Point2f>& dstPoints);
    void buildWeightMatrix(const std::vector<cv::Point2f>& srcPoints, Eigen::SparseMatrix<float>& Weight);
    void updateR(const Eigen::MatrixXf& P, const Eigen::MatrixXf& P_Prime, std::vector<Eigen::Matrix2f>& R, const Eigen::SparseMatrix<float>& Weight);
    void updateP_Prime(const Eigen::MatrixXf& P, Eigen::MatrixXf& P_Prime, const std::vector<Eigen::Matrix2f>& R, const Eigen::SparseMatrix<float>& Weight);
    cv::Mat applyDeformationToBlock(const cv::Mat& img, const std::vector<cv::Point2f>& srcPoints, const std::vector<cv::Point2f>& dstPoints, const cv::Rect& rect);
    std::vector<cv::Point2f> getBlockCorners(const cv::Rect& rect);
    cv::Point2f calculateNewPosition(const cv::Point2f& pixelPos, const std::vector<cv::Point2f>& srcPoints, const std::vector<cv::Point2f>& dstPoints);
    void drawPointsAndWeights(const cv::Mat& img, const std::vector<cv::Point2f>& srcPoints, const Eigen::SparseMatrix<float>& Weight);
};
#endif // ARAP_H
