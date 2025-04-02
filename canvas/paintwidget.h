#ifndef PAINTWIDGET_H
#define PAINTWIDGET_H
#include <QtGui>
#include <QDebug>
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
#include <iostream>
#include "shape.h"
#include "line.h"
#include "rect.h"
#include "tri.h"
#include "rubber.h"
#include "drawcommand.h"
#include "laplaciandeform.h"


class ImageWindow : public QDialog {
public:
    ImageWindow(const QImage& image, QWidget *parent = nullptr) : QDialog(parent) {
        setWindowTitle("Image Preview");
        QLabel *label = new QLabel(this);
        label->setPixmap(QPixmap::fromImage(image));
        setFixedSize(image.size()); // 设置窗口大小为图像大小
    }
};


class PaintWidget : public QWidget
{
    Q_OBJECT

public:
    explicit PaintWidget(QWidget *parent = nullptr);



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

protected:
    void paintEvent(QPaintEvent *event) override;
    void mousePressEvent(QMouseEvent *event) override;
    void mouseMoveEvent(QMouseEvent *event) override;
    void mouseReleaseEvent(QMouseEvent *event) override;
    void showImage(const QImage& image);

private:
    std::vector<cv::Point2f> gridPoints;
    bool isDragging;
    int draggedIndex;
    QPointF lastMousePos;
    Shape::Code currShapeCode;
    QColor currentPenColor;
    int currentPenWidth;
    Shape *shape;
    bool perm;
    QList<Shape*> shapeList;
    QUndoStack* undoStack = new QUndoStack(this);
    int canvasStatus;
    std::vector<cv::Point2f> points;// 网格点
    int gridSize;
    QPoint mousePos;
    QImage image;

};

#endif // PAINTWIDGET_H

