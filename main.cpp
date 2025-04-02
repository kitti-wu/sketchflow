#include <opencv2/core.hpp>
#include <opencv2/highgui.hpp>
#include <opencv2/features2d.hpp>
#include <opencv2/imgproc.hpp>
#include <opencv2/imgproc.hpp>
#include <opencv2/highgui.hpp>
#include <opencv2/imgcodecs.hpp>
#include <opencv2/calib3d.hpp>
#include <QApplication>
#include "mainwindow.h"
#include <iostream>
#include "siftmap.h"
#include "rectredistration.h"
#include "arap.h"
#include "mesh.h"
#include "laplaciandeform.h"


int main(int argc, char *argv[]) {

    QApplication a(argc, argv);
    MainWindow w;
    w.show();
    return a.exec();

    /*
    cv::Mat img1 = cv::imread("/Users/wuketian/project15/images/src3.png", CV_8UC1);
    cv::Mat img2 = cv::imread("/Users/wuketian/project15/images/src4.png", CV_8UC1);

    siftmap SIFT;
    std::map<std::pair<int,int>,std::pair<int,int>> pointMap;
    pointMap = SIFT.usingSIFT(img1,img2);
    std::cout << "Offsets:" << std::endl;

    std::map<std::pair<int, int>, std::pair<int, int>>  offset = SIFT.offset(pointMap);

    //创建 mesh 对象并执行形变
    //mesh mesh;
    //mesh.warpMeshGrid(img1,img2,offset);

    // 初始化网格点
    laplaciandeform deform;
    int gridSize=32;
    std::vector<cv::Point2f> points = deform.initializeGrids(img1, gridSize);
    // 初始化边
    std::vector<std::pair<int, int>> edges = deform.initializeEdges(points, img1.rows, img1.cols, gridSize);

    std::vector<cv::Point2f> dstPoints = deform.deform(img1,gridSize,offset);
    // std::cout<<dstPoints<<std::endl;
    // 绘制初始网格点和边
    cv::Mat deformImg=img1.clone();
    drawGridPoints(img1, points);
    drawEdges(img1, points, edges);

    drawGridPoints(deformImg, dstPoints);
    drawEdges(deformImg, dstPoints, edges);

    // 显示初始图像
    cv::imshow("Initial Grid", img1);
    cv::imshow("Dst Grid", deformImg);

    cv::waitKey(0);

    return 0;
*/
}
