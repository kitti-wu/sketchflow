#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include <QToolBar>
#include <QActionGroup>
#include <QMenu>
#include <QObject>
#include <QPushButton>
#include <QColorDialog>
#include <QFileDialog>
#include <QDebug>
#include <QInputDialog>
#include <QUndoStack>
#include <QApplication>
#include <QMessageBox>
#include <opencv2/opencv.hpp>
#include <QRadioButton>
#include <QButtonGroup>
#include <QListWidget>
#include "shape.h"
#include "paintarea.h"
#include "siftmap.h"
#include "rectredistration.h"
#include "laplaciandeform.h"
#include "switchbtn.h"

class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    MainWindow(QWidget *parent = nullptr);
    ~MainWindow();
    QPushButton *btn1;
    QPushButton *btn2;
    QPushButton *btn3;
    QPushButton *btn4;
    QColor penColor;
    QLabel *txt;
    int lineWidth ;
    PaintArea *paintWidget = nullptr;
    cv::Mat keyframe1;
    cv::Mat keyframe2;
    cv::Mat moveableKeyframe;
    int canvasStatus;
    int a;  //  使用的形变算法
    QButtonGroup *buttonGroup = nullptr;
    QWidget *gridWidget = nullptr;    // 选择网格大小
    QWidget *deformWidget = nullptr;  // 选择形变算法
    QWidget *sliderWidget1 = nullptr; // 播放率滑块
    QWidget *sliderWidget2 = nullptr; // 关键帧透明度滑块
    QWidget *sliderRWidget = nullptr;
    QListWidget *listWidget = nullptr;
    QWidget *inputFrameNumWidget = nullptr;   // 自动生成中间帧帧数
    QWidget *switchModeWidget = nullptr;
    QWidget *btnWidget = nullptr;
    std::vector<QPixmap> dragResList;
    std::vector<QString> deletedTexts;
    QLineEdit *t;   // 自动生成中间帧的帧数

private:
    int height=560;
    int width=800;
    void setGridSize(); // 网格大小单选框
    void setImgList();  // 历史生成图列表
    void txtDlg(QString tittle,QString content);    //  文本形式对话框
    void savePixmaps(const std::vector<QPixmap>& pixmaps, const QString& fileName);
    void savePixmapsAsGif(const std::vector<QPixmap>& pixmaps, const QString& fileName, int delay);// 将图片列表保存为gif图

signals:
    bool clicked();
    void changeLineShape(Shape::Code newShape);// 线条变化信号
    void changeLineWidth(int newWidth);
    void changeColor(QColor newColor);// 改变画笔颜色
    void changeCanvasStatus(int status); // 设置绘图区当前状态

private slots:
    void change2cirline();
    void change2recline();
    void change2triline();
    void change2rubber();
    void openWidthDlg();// 改变画笔粗细
    void openColorBox();
    void downLoadWidget();
    void uploadKeyframe1();
    void uploadKeyframe2();
    void uploadmoveableKeyframe();
    void switch2DragMode();
    void delDragMode();
    void switch2AutoMode();
    void delAutoMode();
    void switch2DrawMode();
    void delDrawMode();
    void generateInbetweening();
    void clearKeyframe();
    void onButtonClicked() ;
    void createListBtn();
    void createRadiusSlider();
    void createPlayRateSlider();
    void createDeformAlgBtn();
    void delDeformAlgBtn();
    void add2list();        // 将当前工作区结果添加到图列表
    void delSelectedList(); // 删除指定项
    void saveSelectedList();// 保存选中项
    void playSelectedList();// 播放选中项
    void changeRate(int);
    void changeOpacity1(int);
    void changeOpacity2(int);
    void changeOpacity3(int);
    void changeRadius(int r);
    void changeDeformAlg0();
    void changeDeformAlg1();
    void setKeyframeNum();
    void saveSelectDlg();
};
#endif // MAINWINDOW_H
