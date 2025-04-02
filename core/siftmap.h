#ifndef SIFTMAP_H
#define SIFTMAP_H
#include <opencv2/opencv.hpp>
#include <opencv2/features2d.hpp>
#include <opencv2/xfeatures2d.hpp>
#include <opencv2/features2d.hpp>
#include <map>


class siftmap
{
public:
    siftmap();
    std::map<std::pair<int, int>, std::pair<int, int>> usingSIFT(cv::Mat& m1,cv::Mat& m2);
    void drawKeypoints(cv::Mat& img, const std::vector<cv::KeyPoint>& keypoints);
    std::map<std::pair<int, int>, std::pair<int, int>> offset(std::map<std::pair<int, int>, std::pair<int, int>> map);
    void drawMatchedKeypoints(cv::Mat& img1,cv::Mat& img2, const std::vector<cv::KeyPoint>& keypoints1, const std::vector<cv::KeyPoint>& keypoints2, const std::vector<cv::DMatch>& matches);
};

#endif // SIFTMAP_H
