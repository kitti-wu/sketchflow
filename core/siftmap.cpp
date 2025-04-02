#include "siftmap.h"
#include <opencv2/features2d.hpp>
#include <opencv2/highgui.hpp>
#include <iostream>


siftmap::siftmap() {}

std::map<std::pair<int, int>, std::pair<int, int>> siftmap::usingSIFT(cv::Mat& img1, cv::Mat& img2) {
    // 初始化 SIFT 检测器
    cv::Ptr<cv::SIFT> detector = cv::SIFT::create();

    // 寻找关键点和描述符
    std::vector<cv::KeyPoint> keypoints1, keypoints2;
    cv::Mat descriptors1, descriptors2;
    detector->detectAndCompute(img1, cv::noArray(), keypoints1, descriptors1);
    detector->detectAndCompute(img2, cv::noArray(), keypoints2, descriptors2);

    // 创建匹配器
    cv::BFMatcher matcher(cv::NORM_L2);
    std::vector<std::vector<cv::DMatch>> knn_matches;
    matcher.knnMatch(descriptors1, descriptors2, knn_matches, 2);  // KNN 匹配
    // 使用 Lowe's ratio test 筛选好的匹配
    const float ratio_thresh = 0.7f;
    std::vector<cv::DMatch> good_matches;
    for (const auto& match : knn_matches) {
        if (match[0].distance < ratio_thresh * match[1].distance) {
            good_matches.push_back(match[0]);
        }
    }

    // 使用 GMS 筛选好的匹配
    std::vector<cv::DMatch> gms_matches;
    cv::xfeatures2d::matchGMS(img1.size(), img2.size(), keypoints1, keypoints2, good_matches, gms_matches);

    // 存储匹配对的关键点坐标
    std::map<std::pair<int, int>, std::pair<int, int>> matched_points_map;
    for (const auto& match : gms_matches) {
        cv::Point2f pt1 = keypoints1[match.queryIdx].pt;
        cv::Point2f pt2 = keypoints2[match.trainIdx].pt;
        matched_points_map[std::make_pair(static_cast<int>(pt1.x), static_cast<int>(pt1.y))] =
            std::make_pair(static_cast<int>(pt2.x), static_cast<int>(pt2.y));
    }


    cv::Mat img_matches;
    drawMatches(img1, keypoints1, img2, keypoints2, good_matches, img_matches,
                cv::Scalar::all(-1),cv::Scalar::all(-1), std::vector<char>(),
                cv::DrawMatchesFlags::NOT_DRAW_SINGLE_POINTS);

    // 显示匹配结果
    std::cout<<"img_matches"<<std::endl;
    // drawMatchedKeypoints(img1, img2, keypoints1, keypoints2, good_matches);


    return matched_points_map;
}

void siftmap::drawKeypoints(cv::Mat& img, const std::vector<cv::KeyPoint>& keypoints) {
    cv::Mat img_with_keypoints;
    cv::drawKeypoints(img, keypoints, img_with_keypoints, cv::Scalar::all(-1), cv::DrawMatchesFlags::DEFAULT);

    // 显示带有特征点的图像
    // imshow("Keypoints", img_with_keypoints);
    // waitKey(0);
}

// 计算偏移量
std::map<std::pair<int, int>, std::pair<int, int>>  siftmap::offset(std::map<std::pair<int, int>, std::pair<int, int>> map){
    std::map<std::pair<int, int>, std::pair<int, int>>  offsets;
    for (const auto& match : map) {
        std::pair<int, int> point1 = match.first;
        std::pair<int, int> point2 = match.second;

        int offsetX = point2.first - point1.first;
        int offsetY = point2.second - point1.second;

        offsets[std::make_pair(static_cast<int>(point1.first), static_cast<int>(point1.second))]=
            (std::make_pair(static_cast<int>(offsetX), static_cast<int>(offsetY)));
    }
    return offsets ;
}

void siftmap::drawMatchedKeypoints(cv::Mat& img1, cv::Mat& img2, const std::vector<cv::KeyPoint>& keypoints1, const std::vector<cv::KeyPoint>& keypoints2, const std::vector<cv::DMatch>& matches) {
    cv::Mat img1_with_keypoints, img2_with_keypoints;

    // 复制图像以绘制特征点
    img1.copyTo(img1_with_keypoints);
    img2.copyTo(img2_with_keypoints);

    // 绘制匹配的特征点
    for (const auto& match : matches) {
        cv::KeyPoint kp1 = keypoints1[match.queryIdx];
        cv::KeyPoint kp2 = keypoints2[match.trainIdx];
        circle(img1_with_keypoints, kp1.pt, 5, cv::Scalar(0, 255, 0), cv::FILLED); // 在img1上绘制绿色圆点
        circle(img2_with_keypoints, kp2.pt, 5, cv::Scalar(0, 255, 0), cv::FILLED); // 在img2上绘制绿色圆点
    }
    // 显示带有匹配特征点的图像
    // imshow("Keypoints 1", img1_with_keypoints);

    // imshow("Keypoints 2", img2_with_keypoints);
    // waitKey(0);
}
