#ifndef PAINTAREA_H
#define PAINTAREA_H

#include <QWidget>
#include <QPainter>
#include <QMouseEvent>
#include <QWidget>
#include <QPixmap>
#include <QMessageBox>
#include <QDialog>
#include <QVBoxLayout>
#include <QHBoxLayout>
#include <QLabel>
#include <QPushButton>
#include <QIcon>
#include <opencv2/opencv.hpp>
#include <unistd.h>
#include "shape.h"
#include "line.h"
#include "rect.h"
#include "tri.h"
#include "rubber.h"
#include "drawcommand.h"
#include "laplaciandeform.h"
#include "siftmap.h"
#include "arap.h"
#include "commondeform.h"

class PaintArea : public QWidget
{
    Q_OBJECT

public:
    explicit PaintArea(QWidget *parent = nullptr);
    void setImage(const QImage &image);
    QImage getImage();
    void setbg(const QPixmap &pixmap);
    void setKeyframe1(const QPixmap &pixmap);
    void setKeyframe2(const QPixmap &pixmap);
    QImage cvMat2QImage(const cv::Mat &mat);
    cv::Mat QImageToCvMat(const QImage &image);
    void generateInbetweening(); // 根据所给关键帧自动生成中间帧
    void showKeyframe();
    void clearDragRes();
    QPixmap getDragRes();
    void setGridSize(int size);
    void initalizeGrid();
    void setResList(std::vector<QPixmap> dragResList);
    void clearResList();
    void setCanPlay();
    void setPlayRate(int rate);
    void setDiameter(int d);
    void setDeformAlg(int type);
    void setOpacity1(int o);
    void setOpacity2(int o);
    void setOpacity3(int o);
    void setKeyframeNum(int num);
    void clearDrawItem();   // 清除线条和用于重做的undocommand
    std::vector<QPixmap> getAutoResList();

protected:
    void paintEvent(QPaintEvent *event) override;
    void mousePressEvent(QMouseEvent *event) override;
    void mouseMoveEvent(QMouseEvent *event) override;
    void mouseReleaseEvent(QMouseEvent *event) override;
    void updateImg();   // 拖拽计算形变
    double calculateDistance(const cv::Point2f& pt1, const cv::Point2f& pt2);

private:
    int canvasStatus;
    std::vector<cv::Point2f> gridPoints;
    bool isDragging;
    int draggedIndex;
    std::vector<QPixmap> dragResList;   // 图片列表
    std::vector<QPixmap> autoResList;
    QPointF lastMousePos;
    Shape::Code currShapeCode;
    QColor currentPenColor;
    int currentPenWidth;
    Shape *shape;
    bool perm;
    QList<Shape*> shapeList;
    QTimer *timer;  // 用于控制定时绘图的时间控制器
    int currentImageIndex = 0;
    QUndoStack* undoStack = new QUndoStack(this);
    std::vector<cv::Point2f> points;        // 网格点
    std::vector<std::pair<int, int>> edges; // 网格线（边）
    int gridSize;
    QPoint mousePos;
    QImage image;
    QPixmap dragRes;
    QPixmap keyframe1;
    QPixmap keyframe2;
    QPixmap matchRes;
    QPixmap generatedInbetweening;
    QLabel* imageLabel;
    bool canPlay;
    int playRate ;
    float opacity1;
    float opacity2;
    float opacity3;
    int deformAlg;
    int diameter;   //  拖拽直径
    bool isFirstPress;
    int inbetweeningNum;// 生成的关键帧个数

    QPoint lastPoint;   // 按下的点
    bool isDrawing;     // 判断是否绘制圆（用于提示直径范围）


signals:
    void gridPointsUpdated(const std::vector<cv::Point2f> &points);

public slots:
    void setCanvasStatus(int i){
        if(i!=canvasStatus){
            canvasStatus=i;
        }
    }
    void setCurrentShape(Shape::Code s)
    {
        if(s != currShapeCode) {
            currShapeCode = s;
            qDebug() << s;
        }
    }
    void setCurrentColor(QColor c)
    {
        if(c != currentPenColor) {
            currentPenColor = c;
        }
    }
    void setCurrentWidth(int w)
    {
        if(w != currentPenWidth) {
            currentPenWidth = w;
        }
    }
    void undo();
    void redo();
    void clear();
    void downLoadWidget(){
        QPixmap pixmap(this->size());
        this->render(&pixmap);
        QImage image = pixmap.toImage();
        image.save("/Users/wuketian/Documents/src/");
    }
    void playResList();
};

#endif // PAINTAREA_H
