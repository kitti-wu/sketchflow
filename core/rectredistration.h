#ifndef RECTREDISTRATION_H
#define RECTREDISTRATION_H
#include <map>
#include <vector>
#include <utility>
#include <cmath>
#include <opencv2/opencv.hpp>

class rectredistration
{
public:
    rectredistration();
    cv::Point2f getWeightedAveragePosition(const std::vector<cv::Point2f>& points, const std::vector<double>& weights);
    std::vector<cv::Rect> divideIntoGrids(const cv::Mat& img, int gridSize);
    double calculateWeight(const cv::Point2f& pt, const cv::Point2f& center);
    cv::Mat delEmpty(const cv::Mat& img1, const std::vector<std::pair<cv::Rect, cv::Rect>>& gridMappings);
    std::vector<std::pair<cv::Point2f, cv::Point2f>> findNearestPoints(const cv::Point2f& center, const std::map<std::pair<int, int>, std::pair<int, int>>& matches, int numPoints);
    bool isNonEmptyGrid(const cv::Mat& img, const cv::Rect& grid, int threshold) ;
    std::vector<std::pair<cv::Rect, cv::Rect>> getGridMappings(cv::Mat& img1, cv::Mat& img2, std::map<std::pair<int, int>, std::pair<int, int>> matches, int gridSize, int nonEmptyThreshold);
};

#endif // RECTREDISTRATION_H
