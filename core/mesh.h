#ifndef MESH_H
#define MESH_H
#include <opencv2/opencv.hpp>
#include <opencv2/core/types.hpp>
#include <opencv2/core.hpp>
#include <opencv2/imgproc.hpp>
#include <opencv2/shape/shape_transformer.hpp>
#include <vector>
#include <numeric>
#include <iostream>
#include <eigen3/Eigen/Dense>
#include <eigen3/Eigen/SparseCore>
#include <eigen3/Eigen/SparseLU>

class mesh
{
public:
    mesh();


    void warpMeshGrid(const cv::Mat& img1, const cv::Mat& img2,
                      std::map<std::pair<int, int>, std::pair<int, int>> & featurePairs) ;
    std::vector<cv::Point2f> divideIntoGrids(const cv::Mat& img, int cols,int rows);
    std::vector<cv::Vec6f> generateDelaunayTriangles(cv::Rect rect, std::vector<cv::Point2f>& points);
    bool isInWhiteArea(const cv::Point2f& point, const cv::Mat& img,int cellSize);
    Eigen::SparseMatrix<float> computeWeightMatrix(const std::vector<cv::Point2f>& points);
    void applyAffineTransform(cv::Mat& warpImage, const cv::Mat& src, std::vector<cv::Point2f> srcTri, std::vector<cv::Point2f> dstTri) ;
    void drawMeshGrid(cv::Mat& image, const std::vector<cv::Vec6f>& triangles, const std::vector<cv::Point2f>& points, const std::vector<cv::Point2f>& transformedPoints);
    void applyThinPlateSplineTransform(cv::Mat& warpImage, const cv::Mat& src, const std::vector<cv::Point2f>& srcPts, const std::vector<cv::Point2f>& dstPts);
    void buildWeightMatrix(const std::vector<cv::Point2f>& srcPoints, Eigen::SparseMatrix<float>& Weight);
    std::vector<Eigen::Matrix2f> estimateRotations(const std::vector<cv::Point2f>& srcPoints, const std::vector<cv::Point2f>& dstPoints, const Eigen::SparseMatrix<float>& Weight);
    void deformMesh(const std::vector<cv::Point2f>& srcPoints, std::vector<cv::Point2f>& dstPoints, const Eigen::SparseMatrix<float>& Weight, int iterations, cv::Mat& img) ;
    void printWeightMatrixStats(const Eigen::SparseMatrix<float>& Weight);
    float linearInterpolate(float v0, float v1, float t);
    double calculateDistance(const cv::Point2d& a, const cv::Point2d& b);
    cv::Point2f interpolateOffset(std::map<double,cv::Point2d>& neighbors, const std::map<std::pair<int, int>, std::pair<int, int>>& offsetMap,float x, float y) ;
    std::map<double, cv::Point2d> findNearestNeighbors(const std::map<std::pair<int, int>, std::pair<int, int>>& offsetMap, const cv::Point2d& pt, int k ) ;
    void visualizeWeightMatrix(const Eigen::SparseMatrix<float>& Weight, int size);
    cv::Point2f interpolateOffset(std::vector<cv::Point2d>& neighbors, float x, float y) ;
    void calculateOffsets(
        const std::vector<cv::Point2f>& srcPoints,
        const std::map<std::pair<int, int>, std::pair<int, int>> & offsetMap,
        std::vector<cv::Point2f>& dstPoints);
    double calculateRigidityEnergy(const std::vector<cv::Point2f>& srcPoints, const std::vector<cv::Point2f>& dstPoints, const std::vector<Eigen::Matrix2f>& rotations, const Eigen::SparseMatrix<float>& Weight);
};



#endif // MESH_H
