#ifndef COMMONDEFORM_H
#define COMMONDEFORM_H
#include <iostream>
#include <opencv2/opencv.hpp>

class commondeform
{
public:
    commondeform();
    // 拖拽
    std::vector<cv::Point2f> deform(const std::vector<cv::Point2f>& srcPoints, const std::vector<cv::Point2f>& dstPoints) ;


};

#endif // COMMONDEFORM_H
