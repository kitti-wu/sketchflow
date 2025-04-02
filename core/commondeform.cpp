#include "commondeform.h"

commondeform::commondeform() {}

std::vector<cv::Point2f> commondeform::deform(const std::vector<cv::Point2f>& srcPoints, const std::vector<cv::Point2f>& dstPoints) {
    std::vector<cv::Point2f> optimizedDstPoints(dstPoints.size());

    // 定义参数，例如权重的衰减系数等
    float decayFactor = 0.5; // 权重衰减系数
    int neighborhoodSize = 3; // 周围点数量

    for (size_t i = 0; i < dstPoints.size(); ++i) {
        cv::Point2f dstPoint = dstPoints[i];

        // 计算每个原始网格点对目标点的影响
        cv::Point2f newPoint(0, 0);
        float totalWeight = 0.0;
        for (size_t j = 0; j < srcPoints.size(); ++j) {
            float dist = cv::norm(dstPoint - srcPoints[j]);
            float weight = 1.0 / (1.0 + decayFactor * dist); // 根据距离计算权重，距离越远权重越小
            newPoint += srcPoints[j] * weight;
            totalWeight += weight;
        }

        // 归一化权重
        if (totalWeight > 0) {
            newPoint /= totalWeight;
        }

        optimizedDstPoints[i] = newPoint;
    }

    return optimizedDstPoints;
}

