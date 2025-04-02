#include "rectredistration.h"

rectredistration::rectredistration() {}

std::vector<cv::Rect> rectredistration::divideIntoGrids(const cv::Mat& img, int gridSize) {
    std::vector<cv::Rect> grids;
    for (int y = 0; y < img.rows; y += gridSize) {
        for (int x = 0; x < img.cols; x += gridSize) {
            grids.push_back(cv::Rect(x, y, gridSize, gridSize));
        }
    }
    return grids;
}

std::vector<std::pair<cv::Point2f, cv::Point2f>> rectredistration::findNearestPoints(const cv::Point2f& center, const std::map<std::pair<int, int>, std::pair<int, int>>& matches, int numPoints) {
    std::vector<std::pair<double, std::pair<cv::Point2f, cv::Point2f>>> distances;

    for (const auto& match : matches) {
        cv::Point2f pt1(match.first.first, match.first.second);
        cv::Point2f pt2(match.second.first, match.second.second);
        double distance = cv::norm(pt1 - center);
        distances.push_back({distance, {pt1, pt2}});
    }

    std::sort(distances.begin(), distances.end(), [](const std::pair<double, std::pair<cv::Point2f, cv::Point2f>>& a, const std::pair<double, std::pair<cv::Point2f, cv::Point2f>>& b) {
        return a.first < b.first;
    });

    std::vector<std::pair<cv::Point2f, cv::Point2f>> nearestPoints;
    for (int i = 0; i < std::min(numPoints, (int)distances.size()); ++i) {
        nearestPoints.push_back(distances[i].second);
    }

    return nearestPoints;
}

double rectredistration::calculateWeight(const cv::Point2f& pt, const cv::Point2f& center) {
    double distance = cv::norm(pt - center);
    return 1.0 / (distance + 1e-6); // 距离越近权重越大
}

// Helper function to get the weighted average position
cv::Point2f rectredistration::getWeightedAveragePosition(const std::vector<cv::Point2f>& points, const std::vector<double>& weights) {
    double sumWeight = 0.0;
    cv::Point2f weightedPos(0, 0);

    for (size_t i = 0; i < points.size(); ++i) {
        weightedPos += weights[i] * points[i];
        sumWeight += weights[i];
    }

    if (sumWeight > 0) {
        weightedPos /= sumWeight;
    }

    return weightedPos;
}

bool rectredistration::isNonEmptyGrid(const cv::Mat& img, const cv::Rect& grid, int threshold) {
    cv::Rect validRect = grid & cv::Rect(0, 0, img.cols, img.rows);
    cv::Mat gridRegion = img(validRect);
    int nonZeroCount = cv::countNonZero(gridRegion);
    std::cout << nonZeroCount << std::endl;
    return nonZeroCount > threshold;
}

std::vector<std::pair<cv::Rect, cv::Rect>> rectredistration::getGridMappings(cv::Mat& img1, cv::Mat& img2, std::map<std::pair<int, int>, std::pair<int, int>> matches, int gridSize, int nonEmptyThreshold = 20) {
    std::vector<cv::Rect> grids1 = divideIntoGrids(img1, gridSize);
    std::vector<cv::Rect> grids2 = divideIntoGrids(img2, gridSize);
    std::vector<std::pair<cv::Rect, cv::Rect>> gridMappings;
    std::vector<bool> mapped(grids1.size(), false);

    for (size_t i = 0; i < grids1.size(); ++i) {
        if (!isNonEmptyGrid(img1, grids1[i], nonEmptyThreshold)) {
            continue;
        }

        cv::Point2f center1(grids1[i].x + grids1[i].width / 2.0f, grids1[i].y + grids1[i].height / 2.0f);

        std::vector<cv::Point2f> points1, points2;
        std::vector<double> weights;
        for (const auto& match : matches) {
            cv::Point2f pt1(match.first.first, match.first.second);
            cv::Point2f pt2(match.second.first, match.second.second);

            if (grids1[i].contains(pt1)) {
                points1.push_back(pt1);
                points2.push_back(pt2);
                weights.push_back(calculateWeight(pt1, center1));
            }
        }

        if (!points1.empty() && !points2.empty()) {
            cv::Point2f avgPos1 = getWeightedAveragePosition(points1, weights);
            cv::Point2f avgPos2 = getWeightedAveragePosition(points2, weights);
            cv::Point2f offset = avgPos2 - avgPos1;
            cv::Rect translatedRect(grids1[i].x + offset.x, grids1[i].y + offset.y, grids1[i].width, grids1[i].height);
            translatedRect &= cv::Rect(0, 0, img2.cols, img2.rows);

            if (translatedRect.width > 0 && translatedRect.height > 0) {
                gridMappings.emplace_back(grids1[i], translatedRect);
                mapped[i] = true;
            }
        }
    }

    for (size_t i = 0; i < mapped.size(); ++i) {
        if (!mapped[i]) {
            std::cout << "Grid " << i << " at " << grids1[i] << " has not been mapped." << std::endl;
        }
    }

    return gridMappings;
}

cv::Rect adjustRectToFit(const cv::Rect& rect, const cv::Size& imgSize) {
    int x = std::max(0, rect.x);
    int y = std::max(0, rect.y);
    int width = std::min(rect.width, imgSize.width - x);
    int height = std::min(rect.height, imgSize.height - y);
    return cv::Rect(x, y, width, height);
}

cv::Mat rectredistration::delEmpty(const cv::Mat& img1, const std::vector<std::pair<cv::Rect, cv::Rect>>& gridMappings) {
    cv::Mat intermediateFrame = cv::Mat::zeros(img1.size(), img1.type());

    for (const auto& mapping : gridMappings) {
        cv::Rect srcRect = adjustRectToFit(mapping.first, img1.size());
        cv::Rect dstRect = adjustRectToFit(mapping.second, intermediateFrame.size());

        if (srcRect.width > 0 && srcRect.height > 0 && dstRect.width > 0 && dstRect.height > 0) {
            cv::Mat grid1 = img1(srcRect);
            cv::Mat roiDst = intermediateFrame(dstRect);
            grid1.copyTo(roiDst);
        }
    }

    return intermediateFrame;
}
