#include "mainwindow.h"
#include "shape.h"
#include <QPushButton>

MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent){
    this->setWindowTitle("基于弹性形变技术的草图动画交互设计软件");
    resize(width,height);
    int canvasWidth = 448;
    int canvasHeight = 320;
    int cx = (width-canvasWidth)/2;
    int cy = (height-canvasHeight)/2.5;

    // 绘图区域
    paintWidget = new PaintArea(this);
    paintWidget->resize(canvasWidth,canvasHeight); // 位置居中
    paintWidget->move(cx,cy);

    // 开关
    // switchbtn *sBtn = new switchbtn(this);
    // sBtn -> move(50,50);
    //设置状态、样式
    // sBtn -> setToggled(true);

    //可设置槽函数控制按钮的开关状态
    // connect(MRSwitchControl,&SwitchControl::toggled,this,&SystemSetting::setMaskRecog);

    // 新建工具栏并将工具栏添加到这个窗口
    QToolBar *bar = new QToolBar;
    this->addToolBar(bar);
    QActionGroup *group = new QActionGroup(bar);

    // 文字提示
    txt = new QLabel("当前为绘图模式",this);
    txt->move(cx,cy-25);


    // 添加笔刷
    QMenu* menuLine = new QMenu("添加笔刷",this);
    QAction *changeLineAction = new QAction("Pen", bar);
    changeLineAction->setIcon(QIcon(":/new/prefix2/icons/pen.png"));
    changeLineAction->setToolTip(tr("选择画笔"));
    changeLineAction->setStatusTip(tr("选择画笔"));
    changeLineAction->setMenu(menuLine);
    QAction* action1 = new QAction(menuLine);
    QAction* action2 = new QAction(menuLine);
    QAction* action3 = new QAction(menuLine);
    action1->setText(tr("圆形画笔"));
    action2->setText(tr("方形画笔"));
    action3->setText(tr("椭圆画笔"));
    menuLine->addAction(action1);
    menuLine->addAction(action2);
    menuLine->addAction(action3);

    bar->addAction(changeLineAction);
    connect(action1, SIGNAL(triggered()), this, SLOT(change2cirline()));
    connect(action2, SIGNAL(triggered()), this, SLOT(change2recline()));
    connect(action3, SIGNAL(triggered()), this, SLOT(change2triline()));
    connect(this,SIGNAL(changeLineShape(Shape::Code)),paintWidget, SLOT(setCurrentShape(Shape::Code)));

    // 调整粗细
    QAction *changeWidthAction = new QAction("Width", bar);
    changeWidthAction->setIcon(QIcon(":/new/prefix2/icons/width.png"));
    changeWidthAction->setToolTip(tr("更改笔刷粗细"));
    changeWidthAction->setStatusTip(tr("更改笔刷粗细"));
    group->addAction(changeWidthAction);
    bar->addAction(changeWidthAction);
    connect(changeWidthAction,SIGNAL(triggered()),this,SLOT(openWidthDlg()));
    connect(this,SIGNAL(changeLineWidth(int)),paintWidget, SLOT(setCurrentWidth(int)));

    // 选择颜色
    QAction *selectColorAction = new QAction("Color", bar);
    selectColorAction->setIcon(QIcon(":/new/prefix2/icons/colorbox.png"));
    selectColorAction->setToolTip(tr("选择画笔颜色"));
    selectColorAction->setStatusTip(tr("选择画笔颜色"));
    group->addAction(selectColorAction);
    bar->addAction(selectColorAction);
    connect(selectColorAction,SIGNAL(triggered()), this, SLOT(openColorBox()));
    connect(this,SIGNAL(changeColor(QColor)),paintWidget,SLOT(setCurrentColor(QColor)));

    // 橡皮
    QAction *eraserAction = new QAction("Eraser", bar);
    eraserAction->setIcon(QIcon(":/new/prefix2/icons/earse.png"));
    eraserAction->setToolTip(tr("擦除线条"));
    eraserAction->setStatusTip(tr("擦除线条"));
    group->addAction(eraserAction);
    bar->addAction(eraserAction);
    connect(eraserAction, SIGNAL(triggered()), this, SLOT(change2rubber()));
    connect(this,SIGNAL(changeLineShape(Shape::Code)),paintWidget, SLOT(setCurrentShape(Shape::Code)));

    // 回退
    QAction *backAction = new QAction("Back", bar);
    backAction->setIcon(QIcon(":/new/prefix2/icons/back.png"));
    backAction->setToolTip(tr("回退"));
    backAction->setStatusTip(tr("回退"));
    group->addAction(backAction);
    bar->addAction(backAction);
    connect(backAction,SIGNAL(triggered()), paintWidget, SLOT(undo()));

    // 重做
    QAction *redoAction = new QAction("Redo", bar);
    redoAction->setIcon(QIcon(":/new/prefix2/icons/redo.png"));
    redoAction->setToolTip(tr("重做"));
    redoAction->setStatusTip(tr("重做"));
    group->addAction(redoAction);
    bar->addAction(redoAction);
    connect(redoAction,SIGNAL(triggered()), paintWidget, SLOT(redo()));

    // 清除
    QAction *clearAction = new QAction("Clear", bar);
    clearAction->setIcon(QIcon(":/new/prefix2/icons/clear.png"));
    clearAction->setToolTip(tr("清除"));
    clearAction->setStatusTip(tr("清除"));
    group->addAction(clearAction);
    bar->addAction(clearAction);
    connect(clearAction,SIGNAL(triggered()), paintWidget, SLOT(clear()));
    bar->addSeparator();

    // 上传文件
    QMenu* menuKeyframe = new QMenu("上传关键帧",this);
    QAction *uploadfileAction = new QAction("UploadFile", bar);
    uploadfileAction->setIcon(QIcon(":/new/prefix2/icons/file.png"));
    uploadfileAction->setToolTip(tr("上传文件"));
    uploadfileAction->setStatusTip(tr("上传文件"));
    uploadfileAction->setMenu(menuKeyframe);
    QAction* action4 = new QAction(menuLine);
    QAction* action5 = new QAction(menuLine);
    QAction* action6 = new QAction(menuLine);
    action4->setText(tr("上传关键帧1"));
    action5->setText(tr("上传关键帧2"));
    action6->setText(tr("上传需要拖拽实现的关键帧"));
    menuKeyframe->addAction(action4);
    menuKeyframe->addAction(action5);
    menuKeyframe->addAction(action6);

    bar->addAction(uploadfileAction);
    connect(action4, SIGNAL(triggered()), this, SLOT(uploadKeyframe1()));
    connect(action5, SIGNAL(triggered()), this, SLOT(uploadKeyframe2()));
    connect(action6, SIGNAL(triggered()), this, SLOT(uploadmoveableKeyframe()));

    // 下载
    QAction *downloadAction = new QAction("downloadFile", bar);
    downloadAction->setIcon(QIcon(":/new/prefix2/icons/dowload.png"));
    downloadAction->setToolTip(tr("下载"));
    downloadAction->setStatusTip(tr("下载"));
    downloadAction->setCheckable(true);
    group->addAction(downloadAction);
    bar->addAction(downloadAction);
    connect(downloadAction,SIGNAL(triggered()),this,SLOT(downLoadWidget()));

    // 按钮
    btn1 = new QPushButton("切换到自动模式",this);
    // btn2 = new QPushButton("清除上传关键帧",this);
    btn3 = new QPushButton("切换到拖拽模式",this);
    btn4 = new QPushButton("切换到绘图模式",this);
    btn1->setGeometry(0.43*width,0.77*height,0.15*width,0.05*height);// 左间距，上间距，宽，高
    // btn2->setGeometry(0.3*width,0.85*height,0.15*width,0.05*height);
    btn3->setGeometry(0.43*width,0.82*height,0.15*width,0.05*height);
    btn4->setGeometry(0.43*width,0.87*height,0.15*width,0.05*height);

    connect(btn1, SIGNAL(clicked()), this,SLOT(switch2AutoMode()));
    // connect(btn2, SIGNAL(clicked()), this,SLOT(clearKeyframe()));
    connect(btn3, SIGNAL(clicked()), this,SLOT(switch2DragMode()));
    connect(btn4, SIGNAL(clicked()), this,SLOT(switch2DrawMode()));
    connect(this,SIGNAL(changeCanvasStatus(int)),paintWidget, SLOT(setCanvasStatus(int)));

}

void MainWindow::change2cirline(){
    // 圆形
    emit changeLineShape(Shape::Line);
}

void MainWindow::change2recline(){
    // 方形
    emit changeLineShape(Shape::Rect);
}

void MainWindow::change2triline(){
    // 椭圆
    emit changeLineShape(Shape::Tri);
}

void MainWindow::change2rubber(){
    // 橡皮
    emit changeLineShape(Shape::Rubber);
}

void MainWindow::openWidthDlg(){
    bool ok;
    QString string = QInputDialog::getText(this, tr("设置线条宽度"),
                                           tr("请输入线宽："), QLineEdit::Normal,"", &ok);
    if(ok) emit changeLineWidth(string.toInt());

      }

void MainWindow::downLoadWidget(){
    QPixmap pixmap = paintWidget->grab();
    QString filePath = QFileDialog::getSaveFileName(this, tr("保存图片"), "", tr("PNG Image (*.png)"));
    if (!filePath.isEmpty()) {
        if (!filePath.endsWith(".png", Qt::CaseInsensitive)) {
            filePath += ".png";
        }
        pixmap.save(filePath, "PNG");
    }
}

void MainWindow::openColorBox() {

    QColor selectedColor = QColorDialog::getColor(QColor("red"), this, "请选择画笔颜色");
    // 检查用户是否选择了有效的颜色
    if (selectedColor.isValid()) {
        penColor = selectedColor;
        emit changeColor(penColor);
        qDebug("r[%d] - g[%d] - b[%d]", penColor.red(), penColor.green(), penColor.blue());
    }
}

void MainWindow::uploadKeyframe1(){

    QString filePath = QFileDialog::getOpenFileName(nullptr, "Open Image", "/Users/wuketian/project12_副本/images", "PNG Image (*.png)");
    if (!filePath.isEmpty()) {
        QPixmap m_pixmap;
        m_pixmap.load(filePath);
        // 确保存在pixmap
        if (!m_pixmap.isNull()) {
            // 计算缩放比例
            QPixmap scaledPixmap = m_pixmap.scaled(paintWidget->size(), Qt::KeepAspectRatio, Qt::SmoothTransformation);
            paintWidget->setKeyframe1(scaledPixmap);
        } else {
            QMessageBox::critical(nullptr, "加载失败", "无法加载图像。请检查文件是否损坏或支持的格式。");
        }
    }
    txt->setGeometry(txt->geometry().x(),txt->geometry().y(),250,20);
    // txt->setText("当前为根据所给关键帧自动生成中间帧模式");
    emit changeCanvasStatus(2);
}

void MainWindow::uploadKeyframe2(){

    QString filePath = QFileDialog::getOpenFileName(nullptr, "Open Image", "/Users/wuketian/project12_副本/images", "PNG Image (*.png)");
    if (!filePath.isEmpty()) {
        QPixmap m_pixmap;
        m_pixmap.load(filePath);
        // 确保存在pixmap
        if (!m_pixmap.isNull()) {
            // 计算缩放比例
            QPixmap scaledPixmap = m_pixmap.scaled(paintWidget->size(), Qt::KeepAspectRatio, Qt::SmoothTransformation);
            paintWidget->setKeyframe2(scaledPixmap);
        } else {
            QMessageBox::critical(nullptr, "加载失败", "无法加载图像。请检查文件是否损坏或支持的格式。");
        }
    }

    emit changeCanvasStatus(2);
}

void MainWindow::clearKeyframe(){

    // saveSelectDlg();
    paintWidget->setKeyframe1(QPixmap());
    paintWidget->setKeyframe2(QPixmap());
    paintWidget->setImage(QImage());
}

void MainWindow::uploadmoveableKeyframe(){
    QString filePath = QFileDialog::getOpenFileName(nullptr, "Open Image", "/Users/wuketian/project12_副本/images", "PNG Image (*.png)");
    if (!filePath.isEmpty()) {
        QPixmap m_pixmap;
        m_pixmap.load(filePath);
            // 确保存在pixmap
            if (!m_pixmap.isNull()) {
                // 计算缩放比例
                QPixmap scaledPixmap = m_pixmap.scaled(paintWidget->size(), Qt::KeepAspectRatio, Qt::SmoothTransformation);
                paintWidget->setImage(scaledPixmap.toImage());
        } else {
            QMessageBox::critical(nullptr, "加载失败", "无法加载图像。请检查文件是否损坏或支持的格式。");
        }
    }
    emit changeCanvasStatus(1);
}

void MainWindow::switch2DragMode(){
    if(txt->text()=="当前为绘图模式"){
        delDrawMode();
    }else if(txt->text()=="当前为自动模式"){
        delAutoMode();
    }

    paintWidget->initalizeGrid();
    setGridSize();
    setImgList();
    createListBtn();
    createPlayRateSlider();
    createDeformAlgBtn();
    createRadiusSlider();

    emit changeCanvasStatus(1);
    txt->setText("当前为拖拽模式");
    update();

}

void MainWindow::switch2DrawMode(){
    if(txt->text()=="当前为拖拽模式"){
        delDragMode();
    }else if(txt->text()=="当前为自动模式"){
        delAutoMode();
    }
    QImage image(paintWidget->width(), paintWidget->height(), QImage::Format_RGB32);
    image.fill(Qt::white); // 使用白色填充
    paintWidget->clearResList();
    paintWidget->setImage(image);

    emit changeCanvasStatus(0);
    update();
    txt->setText("当前为绘图模式");
}

void MainWindow::switch2AutoMode(){
    if(txt->text()=="当前为拖拽模式"){
        delDragMode();
    }else if(txt->text()=="当前为绘图模式"){
        delDrawMode();
    }

    // --创建调整关键帧透明度滑块
    sliderWidget2 = new QWidget(this);
    QVBoxLayout *layout = new QVBoxLayout(sliderWidget2);
    QLabel *l = new QLabel("关键帧1透明度：",sliderWidget2);
    QSlider *slider = new QSlider(sliderWidget2);
    QLabel *l2 = new QLabel("关键帧2透明度：",sliderWidget2);
    QSlider *slider2 = new QSlider(sliderWidget2);
    QLabel *l3 = new QLabel("中间帧透明度：",sliderWidget2);
    QSlider *slider3 = new QSlider(sliderWidget2);
    slider->setMaximum(10);
    slider->setMinimum(1);
    slider->setValue(5);
    slider->setPageStep(1);
    slider->setTracking(false);
    slider->setOrientation(Qt::Horizontal);
    slider2->setMaximum(10);
    slider2->setMinimum(1);
    slider2->setValue(5);
    slider2->setPageStep(1);
    slider2->setTracking(false);
    slider2->setOrientation(Qt::Horizontal);
    slider3->setMaximum(10);
    slider3->setMinimum(1);
    slider3->setValue(5);
    slider3->setPageStep(1);
    slider3->setTracking(false);
    slider3->setOrientation(Qt::Horizontal);

    layout->addWidget(l);
    layout->addWidget(slider);
    layout->addWidget(l2);
    layout->addWidget(slider2);
    layout->addWidget(l3);
    layout->addWidget(slider3);
    connect(slider, SIGNAL(sliderMoved(int)), this, SLOT(changeOpacity1(int)));
    connect(slider2, SIGNAL(sliderMoved(int)), this, SLOT(changeOpacity2(int)));
    connect(slider3, SIGNAL(sliderMoved(int)), this, SLOT(changeOpacity3(int)));

    sliderWidget2->setLayout(layout);
    sliderWidget2->setGeometry(this->width*0.03, this->height*0.34, 140, 150);
    sliderWidget2->show();

    // --输入生成结果窗口
    inputFrameNumWidget=new QWidget(this);
    QVBoxLayout *frameLayout = new QVBoxLayout(inputFrameNumWidget);
    QLabel *frameLabel = new QLabel("帧数：",inputFrameNumWidget);
    t = new QLineEdit(this);
    t->setText("1");
    QPushButton *btn = new QPushButton("确认",inputFrameNumWidget);
    connect(btn,SIGNAL(clicked()),this,SLOT(setKeyframeNum()));

    frameLayout->addWidget(frameLabel);
    frameLayout->addWidget(t);
    frameLayout->addWidget(btn);

    inputFrameNumWidget->setLayout(layout);
    inputFrameNumWidget->setGeometry(this->width*0.03, this->height*0.15,140,100);
    inputFrameNumWidget->show();

    // --切换状态
    switchModeWidget = new QWidget(this);
    QVBoxLayout *sLayout = new QVBoxLayout(switchModeWidget);
    QPushButton *btn1 = new QPushButton("开始生成关键帧",switchModeWidget);
    QPushButton *btn2 = new QPushButton("保存生成的关键帧",switchModeWidget);

    sLayout->addWidget(btn1);
    sLayout->addWidget(btn2);

    connect(btn1,SIGNAL(clicked()),this,SLOT(generateInbetweening()));
    connect(btn2,SIGNAL(clicked()),this,SLOT(saveSelectDlg()));

    switchModeWidget->setLayout(sLayout);
    switchModeWidget->setGeometry(this->width*0.03, this->height*0.62, 140, 80);
    switchModeWidget->show();

    // 更改绘图区状态
    emit changeCanvasStatus(2);
    txt->setText("当前为自动模式");
    update();

}

void MainWindow::delAutoMode(){

    delete sliderWidget2;
    sliderWidget2 = nullptr;


    delete inputFrameNumWidget;
    inputFrameNumWidget = nullptr;


    delete switchModeWidget;
    switchModeWidget = nullptr;

}

void MainWindow::delDrawMode(){
    paintWidget->clearDrawItem();
}

void MainWindow::createDeformAlgBtn(){
    deformWidget = new QWidget(this);

    QLabel *tittle = new QLabel("形变算法：");
    QRadioButton *radioButton1 = new QRadioButton("拉普拉斯形变", deformWidget);
    QRadioButton *radioButton2 = new QRadioButton("尽可能刚性形变", deformWidget);

    // 默认选择第一个选项
    radioButton1->setChecked(true);

    // 创建QVBoxLayout并添加QRadioButton
    QVBoxLayout *layout = new QVBoxLayout(this);
    layout->addWidget(tittle);
    layout->addWidget(radioButton1);
    layout->addWidget(radioButton2);


    // 连接每个按钮的clicked()信号到槽函数
    connect(radioButton1, SIGNAL(clicked()), this, SLOT(changeDeformAlg0()));
    connect(radioButton2, SIGNAL(clicked()), this, SLOT(changeDeformAlg1()));

    deformWidget->setLayout(layout);
    deformWidget->setGeometry(this->width*0.03, this->height*0.2, 150, 100);
    deformWidget->show();

}

void MainWindow::delDeformAlgBtn(){
    if (deformWidget&&deformWidget->parentWidget()) {
        deformWidget->parentWidget()->layout()->removeWidget(deformWidget);
    }
    delete deformWidget;
}

void MainWindow::changeDeformAlg0(){
    paintWidget->setDeformAlg(0);
}

void MainWindow::changeDeformAlg1(){
    paintWidget->setDeformAlg(1);
}

void MainWindow::setGridSize(){
    // 创建QRadioButton
    gridWidget=new QWidget(this);

    QLabel *tittle = new QLabel("网格大小：");
    QRadioButton *radioButton1 = new QRadioButton("小", gridWidget);
    QRadioButton *radioButton2 = new QRadioButton("中", gridWidget);
    QRadioButton *radioButton3 = new QRadioButton("大", gridWidget);
    QRadioButton *radioButton4 = new QRadioButton("特大", gridWidget);

    // 默认选择第一个选项
    radioButton3->setChecked(true);

    // 创建QVBoxLayout并添加QRadioButton
    QVBoxLayout *layout = new QVBoxLayout(this);
    layout->addWidget(tittle);
    layout->addWidget(radioButton1);
    layout->addWidget(radioButton2);
    layout->addWidget(radioButton3);
    layout->addWidget(radioButton4);

    // 创建QButtonGroup以管理QRadioButton
    buttonGroup = new QButtonGroup(this);
    buttonGroup->addButton(radioButton1, 1);
    buttonGroup->addButton(radioButton2, 2);
    buttonGroup->addButton(radioButton3, 3);
    buttonGroup->addButton(radioButton4, 4);

    // 连接每个按钮的clicked()信号到槽函数
    connect(radioButton1, SIGNAL(clicked()), this, SLOT(onButtonClicked()));
    connect(radioButton2, SIGNAL(clicked()), this, SLOT(onButtonClicked()));
    connect(radioButton3, SIGNAL(clicked()), this, SLOT(onButtonClicked()));
    connect(radioButton4, SIGNAL(clicked()), this, SLOT(onButtonClicked()));

    gridWidget->setLayout(layout);
    gridWidget->setGeometry(this->width*0.03, this->height*0.5, 100, 120);
    gridWidget->show();

}

void MainWindow::setKeyframeNum(){
    int num = t->text().toInt();
    paintWidget->setKeyframeNum(num);
}

// -- List编辑按钮

// 将当前结果添加到结果列表中
void MainWindow::add2list(){

    // 添加图片
    if(!paintWidget->getImage().isNull()){
        dragResList.push_back(QPixmap::fromImage(paintWidget->getImage()));
        qDebug()<<"成功添加到列表,大小为："<<dragResList.size();
    }

    // 刷新列表
    setImgList();
}

// 删除指定图片
void MainWindow::delSelectedList(){

    // 获得索引从图片列表中删除
    std::vector<int> selectedIndices;
    QList<QModelIndex> selectedIndexes = listWidget->selectionModel()->selectedIndexes();
    for (const QModelIndex &index : selectedIndexes) {
        selectedIndices.push_back(index.row());
    }

    // 复制索引列表并排序
    std::sort(selectedIndices.begin(), selectedIndices.end(), std::greater<int>());

    // 逆序遍历并删除元素
    for (int index : selectedIndices) {
        if (index >= 0 && index < dragResList.size()) {
            dragResList.erase(dragResList.begin() + index);
        } else {
            std::cerr << "Invalid index: " << index << std::endl;
        }
    }

    // 更新UI
    setImgList();
}

// 保存指定图片
void MainWindow::saveSelectedList(){

    // 获得索引从图片列表中删除
    QList selectedItems = listWidget->selectedItems();

    std::vector<int> selectedIndices;
    QList<QModelIndex> selectedIndexes = listWidget->selectionModel()->selectedIndexes();
    for (const QModelIndex &index : selectedIndexes) {
        selectedIndices.push_back(index.row());
    }

    // 复制索引列表并排序
    std::sort(selectedIndices.begin(), selectedIndices.end(), std::greater<int>());

    static QString filePath = ""; // 静态变量用于保存文件路径
    if (filePath.isEmpty()) {
        // 如果文件路径为空，即第一次保存，弹出对话框
        filePath = QFileDialog::getSaveFileName(this, "保存选中图片", "", "PNG Files (*.png);;JPEG Files (*.jpg *.jpeg);;All Files (*)");
        if (filePath.isEmpty()) {
             txtDlg("保存失败", "请设置保存路径");
            return;
        }
    }

    // 保存所有图片到同一位置
    for (const QPixmap& pixmap : dragResList) {
        if (!pixmap.save(filePath)) {
            txtDlg("保存失败", "结果保存失败，请重试！");
            return;
        }
    }
    txtDlg("保存成功", "所有结果已经保存到指定位置");
}

void MainWindow::playSelectedList(){
    // 获取当前选中图片索引
    QList selectedItems = listWidget->selectedItems();

    std::vector<int> selectedIndices;
    QList<QModelIndex> selectedIndexes = listWidget->selectionModel()->selectedIndexes();
    std::vector<QPixmap> tempList;
    if(!selectedIndexes.isEmpty()){
        for (const QModelIndex &index : selectedIndexes) {
            tempList.push_back(this->dragResList[index.row()]);
        }
    }
    else{
        txtDlg("未选中","请先选择生成结果再播放");
    }

    paintWidget->setResList(tempList);
    // emit changeCanvasStatus(3);
    paintWidget->setCanPlay(); // 设置播放模式
    paintWidget->playResList();// 启动定时器
}

// qlist操作按钮组
void MainWindow::createListBtn(){

    btnWidget=new QWidget(this);
    QVBoxLayout *layout = new QVBoxLayout(btnWidget);
    // -- 添加功能按钮
    QPushButton *btn1 = new QPushButton("上传当前结果",btnWidget);
    QPushButton *btn2 = new QPushButton("删除选中结果",btnWidget);
    QPushButton *btn3 = new QPushButton("保存选中结果",btnWidget);
    QPushButton *btn4 = new QPushButton("展示选中结果",btnWidget);

    layout->addWidget(btn1);
    layout->addWidget(btn2);
    layout->addWidget(btn3);
    layout->addWidget(btn4);

    connect(btn1,SIGNAL(clicked()),this,SLOT(add2list()));
    connect(btn2,SIGNAL(clicked()),this,SLOT(delSelectedList()));
    connect(btn3,SIGNAL(clicked()),this,SLOT(saveSelectedList()));
    connect(btn4,SIGNAL(clicked()),this,SLOT(playSelectedList()));

    btnWidget->setLayout(layout);
    btnWidget->setGeometry(this->width*0.81, this->height*0.55, 140, 120);
    btnWidget->show();
}

// 滑块设置区域大小
void MainWindow::createRadiusSlider(){
    sliderRWidget = new QWidget(this);
    QVBoxLayout *layout = new QVBoxLayout(sliderRWidget);
    QLabel *l = new QLabel("控制点大小：",sliderRWidget);
    QSlider *slider = new QSlider(sliderRWidget);
    slider->setMaximum(20);
    slider->setMinimum(1);
    slider->setValue(10);
    slider->setPageStep(1);
    slider->setTracking(false);
    slider->setOrientation(Qt::Horizontal);

    layout->addWidget(l);
    layout->addWidget(slider);
    connect(slider, SIGNAL(sliderMoved(int)), this, SLOT(changeRadius(int)));

    sliderRWidget->setLayout(layout);
    sliderRWidget->setGeometry(this->width*0.03, this->height*0.37, 110, 90);
    sliderRWidget->show();
}


// 滑块设置播放速度
void MainWindow::createPlayRateSlider(){
    sliderWidget1 = new QWidget(this);
    QHBoxLayout *layout = new QHBoxLayout(sliderWidget1);
    QLabel *l = new QLabel("播放速度：",sliderWidget1);
    QSlider *slider = new QSlider(sliderWidget1);
    slider->setMaximum(20);
    slider->setMinimum(2);
    slider->setValue(20);
    slider->setPageStep(2);
    slider->setTracking(false);
    slider->setOrientation(Qt::Horizontal);
    slider->setStyleSheet("QSlider::handle { width: 10px; height: 10px; }");

    layout->addWidget(l);
    layout->addWidget(slider);
    connect(slider, SIGNAL(sliderMoved(int)), this, SLOT(changeRate(int)));

    sliderWidget1->setLayout(layout);
    sliderWidget1->setGeometry(this->width*0.8, this->height*0.48, 150, 60);
    sliderWidget1->show();
}

void MainWindow::changeRate(int rate){
    qDebug()<<"当前滑块值为："<<rate;
    paintWidget->setPlayRate(rate);
}

void MainWindow::changeOpacity1(int opacity){
    paintWidget->setOpacity1(opacity);
}

void MainWindow::changeOpacity2(int opacity){
    paintWidget->setOpacity2(opacity);
}

void MainWindow::changeOpacity3(int opacity){
    paintWidget->setOpacity3(opacity);
}


void MainWindow::changeRadius(int r){
    paintWidget->setDiameter(r*2*3.2);
}

void MainWindow::setImgList(){
    //--- list
    // 切换显示模式setViewMode(QListView::ViewMode mode)
    listWidget = new QListWidget(this);
    listWidget->setSelectionMode(QAbstractItemView::MultiSelection); // 设置为多选模式
    // 遍历循环
    if(!dragResList.empty()){
        int x = 0;
        for(auto& pixmap:dragResList){
            QListWidgetItem *item = new QListWidgetItem(listWidget);
            item->setIcon(QIcon(pixmap));
            QString str = QString::asprintf("生成结果%d",x);
            item->setText(str);
            item->setSelected(false);        // 设为选中状态
            item->setFlags(Qt::ItemIsSelectable |      // 设置为不可编辑状态
                            Qt::ItemIsUserCheckable
                            |Qt::ItemIsEnabled);
            x++;
      }

    }

    listWidget->setGeometry(this->width*0.81,this->height*0.18,140,180);
    listWidget->show();

}

void MainWindow::onButtonClicked() {
    QRadioButton *button = qobject_cast<QRadioButton *>(sender());
    if (button) {
        int id = buttonGroup->id(button);
        qDebug() << "Button" << id << "clicked";
        // 根据id返回对应值
        switch (id) {
        case 1:
            paintWidget->setGridSize(8);
            break;
        case 2:
            paintWidget->setGridSize(16);
            break;
        case 3:
            paintWidget->setGridSize(32);
            break;
        case 4:
            paintWidget->setGridSize(64);
            break;
        default:
            break;
        }
    }
}

void MainWindow::delDragMode(){

    delete gridWidget;
    gridWidget = nullptr;

    delete btnWidget;
    btnWidget = nullptr;

    delete sliderWidget1;
    sliderWidget1 = nullptr;

    delete sliderRWidget;
    sliderRWidget = nullptr;

    delete listWidget;
    listWidget = nullptr;

    delete deformWidget;
    deformWidget = nullptr;

}

void MainWindow::generateInbetweening(){
    paintWidget->generateInbetweening();

}

// --自定义纯文本对话框
void MainWindow::txtDlg(QString tittle,QString content){
    QDialog *dlg = new QDialog(this);
    dlg->setWindowTitle(tittle);
    QVBoxLayout* layout = new QVBoxLayout();
    dlg->setLayout(layout);
    //创建标签和按钮
    QLabel * label = new QLabel(content,this);
    QPushButton* button = new QPushButton("好的",this);
    //将控件添加进布局中
    layout->addWidget(label);
    layout->addWidget(button);
    //进行按钮槽函数链接
    connect(button,SIGNAL(clicked()),dlg,SLOT(close()));
    dlg->exec();
}

// --自定义保存为动图或保存为图片列表对话框
void MainWindow::saveSelectDlg() {
    QDialog *dlg = new QDialog(this);
    dlg->setWindowTitle(tr("选择保存形式"));
    QVBoxLayout* layoutV = new QVBoxLayout(dlg);   //  纵向排列
    QHBoxLayout* layoutH = new QHBoxLayout();   //  横向排列

    // 创建标签和按钮
    QLabel * label = new QLabel(tr("请问保存图片形式还是动图形式关键帧？"), dlg);
    QPushButton* button1 = new QPushButton("动图", dlg);
    QPushButton* button2 = new QPushButton("图片", dlg);
    layoutH->addWidget(button1);
    layoutH->addWidget(button2);
    // 将控件添加进布局中
    layoutV->addWidget(label);
    layoutV->addLayout(layoutH);
    dlg->setLayout(layoutV);
    dlg->show();

    if(txt->text()=="当前为拖拽模式"){

    // 获取当前选中结果
    // 声明一个存储 QPixmap 的 vector
    if (!listWidget){
        qDebug()<<"未初始化listWidget";
    }else{
    std::vector<QPixmap> selectedPixmaps;
    auto model = listWidget->selectionModel();
    if (!model) {
        return;
    }

    // 获取选中的索引
    QList<QModelIndex> selectedIndexes = listWidget->selectionModel()->selectedIndexes();
    if (!selectedIndexes.isEmpty()) {
        // 遍历选中的索引并获取对应的 QPixmap
        for (const QModelIndex &index : selectedIndexes) {
            if (index.row() >= 0 && index.row() < dragResList.size()) {
                // 将 dragResList 中对应索引的 QPixmap 添加到 selectedPixmaps 中
                selectedPixmaps.push_back(dragResList[index.row()]);
            }
        }

        static QString filePath = ""; // 静态变量用于保存文件路径
        if (filePath.isEmpty()) {
            // 如果文件路径为空，即第一次保存，弹出对话框
            filePath = QFileDialog::getSaveFileName(this, "保存选中图片", "/Users/wuketian/project12_副本/res", "All Files (*)");
            if (filePath.isEmpty()) {
                txtDlg("保存失败", "请设置保存路径");
                return;
            }
        }
        // 连接按钮的点击信号到相应的槽函数
        connect(button1, &QPushButton::clicked, dlg, [=]() {
            qDebug() << "Clicked 动图按钮";
            savePixmapsAsGif(selectedPixmaps, filePath, 100);
            dlg->accept();
        });
        connect(button2, &QPushButton::clicked, dlg, [=]() {
            qDebug() << "Clicked 图片按钮";
            savePixmaps(selectedPixmaps, filePath);
            dlg->accept();
        });
        dlg->exec();
    } else {
        txtDlg("保存失败", "请选择至少一张图片");
        return;
    }
    }

    }

    if(txt->text()=="当前为自动模式"){

                static QString filePath = ""; // 静态变量用于保存文件路径
                if (filePath.isEmpty()) {
                    // 如果文件路径为空，即第一次保存，弹出对话框
                    filePath = QFileDialog::getSaveFileName(this, "保存选中图片", "/Users/wuketian/project12_副本/res", "All Files (*)");
                    if (filePath.isEmpty()) {
                        txtDlg("保存失败", "请设置保存路径");
                        return;
                    }
                }
                // 连接按钮的点击信号到相应的槽函数
                connect(button1, &QPushButton::clicked, dlg, [=]() {
                    qDebug() << "Clicked 动图按钮";
                    savePixmapsAsGif(paintWidget->getAutoResList(), filePath, 100);
                    dlg->accept();
                });
                connect(button2, &QPushButton::clicked, dlg, [=]() {
                    qDebug() << "Clicked 图片按钮";
                    savePixmaps(paintWidget->getAutoResList(), filePath);
                    dlg->accept();
                });
                dlg->exec();
    }
    else {
        txtDlg("保存失败", "请选择至少一张图片");
        return;
    }


}




// 将图片列表保存为gif
void MainWindow::savePixmapsAsGif(const std::vector<QPixmap>& pixmaps, const QString& fileName, int delay)
{
    qWarning() << "chuafa";
    if (pixmaps.empty()) {
        qWarning() << "No pixmaps to save!";
        return;
    }

    QBuffer buffer;
    buffer.open(QIODevice::WriteOnly);

    QImageWriter writer(&buffer, "gif");
    writer.setFormat("gif");
    writer.setCompression(0);
    writer.setOptimizedWrite(true);
    writer.setQuality(-1);

    bool firstImage = true;
    qDebug()<<"pixmaps大小为："<<pixmaps.size();

    for (const QPixmap& pixmap : pixmaps) {
        QImage image = pixmap.toImage();

        // 检查并转换图片格式为RGB32
        QImage indexedImage = image.convertToFormat(QImage::Format_Indexed8);

        qWarning() << "image.format():" << image.format();

        if (firstImage) {
            if (!writer.write(image)) {
                qWarning() << "Failed to write first image to gif:" << writer.errorString();

                return;
            }
            firstImage = false;
        } else {
            writer.write(image);
        }
    }

    QFile file(fileName);
    if (!file.open(QIODevice::WriteOnly)) {
        qWarning() << "Failed to open file for writing:" << fileName;
        return;
    }

    file.write(buffer.data());
    file.close();

}

void MainWindow::savePixmaps(const std::vector<QPixmap>& pixmaps, const QString& fileName){

}


MainWindow::~MainWindow()
{
}

