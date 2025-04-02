#include "paintarea.h"

PaintArea::PaintArea(QWidget *parent)
    : QWidget(parent),currShapeCode(Shape::Line), shape(NULL), perm(false), undoStack(new QUndoStack(this)), imageLabel(new QLabel(this)
                )
{
    setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Expanding);
    setMouseTracking(true);
    diameter = 32;
}

// ------处理鼠标输入-----

void PaintArea::paintEvent(QPaintEvent *event)
{
    QPainter painter(this);
    switch(canvasStatus){
    case 0:
    {
        qDebug() << "当前为绘图模式";
        // 绘制背景
        painter.fillRect(0, 0, size().width(), size().height(),Qt::white);
        if (!image.isNull()) {
            // qreal devicePixelRatio = devicePixelRatioF();
            qreal devicePixelRatio = 2 ;
            // 缩放适应大小
            QImage scaledImage = image.scaled(size() * devicePixelRatio, Qt::KeepAspectRatio, Qt::SmoothTransformation);
            scaledImage.setDevicePixelRatio(devicePixelRatio);
            int x = (width() - scaledImage.width()) / 2;
            int y = (height() - scaledImage.height()) / 2;
            painter.drawImage(x, y, scaledImage);
            // showImage(image);
        }
        if(!dragRes.isNull()){
            int x = (width() - dragRes.width()) / 2;
            int y = (height() - dragRes.height()) / 2;
            painter.drawPixmap(x, y, dragRes);
        }

        // 绘制线条
        for(Shape * shape: shapeList) {
            shape->paint(painter);
        }
        break;
    }
    case 1:
    {
        qDebug() << "当前为拖拽模式";
        // 播放结果
        if(canPlay){
            painter.fillRect(0, 0, size().width(), size().height(),Qt::white);
            // 播放结果
            // 获取图片列表
            if(!this->dragResList.empty()){
                if (currentImageIndex < dragResList.size()) {
                    QPixmap pixmap = dragResList[currentImageIndex];
                    int x = (width() - pixmap.width()) / 2;
                    int y = (height() - pixmap.height()) / 2;
                    painter.drawPixmap(x, y, pixmap);
                    currentImageIndex++;
                    qDebug()<<"dragResList大小为："<<dragResList.size();
                }
                else{
                    canPlay = false;
                    // setImage((dragResList[currentImageIndex]).toImage());
                    currentImageIndex = 0;
                    timer->stop();
                    qDebug()<<"结束播放";
                }
            }
        }
        else{
        qDebug()<<"不能播放";
        if (!image.isNull()) {
            qDebug()<<"image不为空";
            painter.fillRect(0, 0, size().width(), size().height(),Qt::white);
            qreal devicePixelRatio = devicePixelRatioF();
            // 缩放适应大小
            QImage scaledImage = image.scaled(size() * devicePixelRatio, Qt::KeepAspectRatio, Qt::SmoothTransformation);
            scaledImage.setDevicePixelRatio(devicePixelRatio);
            int x = (width() - image.width()) / 2;
            int y = (height() - image.height()) / 2;
            painter.drawImage(x, y, scaledImage);

        }

        // 绘制网格上的点
        for (const auto& point : gridPoints) {
            QPointF qtPoint(point.x, point.y);
            int distance = qSqrt(qPow(mousePos.x() - qtPoint.x(), 2) + qPow(mousePos.y() - qtPoint.y(), 2));
            painter.setPen(Qt::blue);
            painter.setBrush(Qt::blue);
            if (distance < diameter/2) {
                painter.drawEllipse(qtPoint, 5, 5);
            } else {
                painter.drawEllipse(qtPoint, 2, 2);
            }
        }
        // 绘制网格线
        for (const auto& edge : edges) {
            QLine qtLine(gridPoints[edge.first].x,gridPoints[edge.first].y,
            gridPoints[edge.second].x,gridPoints[edge.second].y);
            painter.drawLine(qtLine);
        }


        // 绘制提示圆
        if(isDrawing&&isFirstPress){
            painter.setPen(Qt::black);
            painter.setBrush(Qt::white);
            if(diameter==0){diameter=64;} // 圆的半径
            painter.drawEllipse(lastPoint, diameter/2, diameter/2);
            isDrawing = false;
            isFirstPress = false;
        }
        }

        break;

    }
    case 2:
    {
        qDebug() << "当前为上传图片生成模式";
        painter.fillRect(0, 0, size().width(), size().height(),Qt::white);

        if(opacity1==0){opacity1=0.5;}
        if(opacity2==0){opacity2=0.5;}
        if(opacity3==0){opacity3=0.5;}

        if(!keyframe1.isNull()){
            int x = (width() - keyframe1.width()) / 2;
            int y = (height() - keyframe1.height()) / 2;
            painter.setOpacity(opacity1);
            painter.drawPixmap(x, y, keyframe1);
            }
        if(!keyframe2.isNull()){
            int x = (width() - keyframe2.width()) / 2;
            int y = (height() - keyframe2.height()) / 2;
            painter.setOpacity(opacity2);
            painter.drawPixmap(x, y, keyframe2);
        }
        if(!autoResList.empty()){
            for(auto &img:autoResList){
                int x = (width() - img.width()) / 2;
                int y = (height() - img.height()) / 2;
                painter.setOpacity(opacity3);
                painter.drawPixmap(x, y, img);
            }
        }
        else{
            if(!matchRes.isNull()){
                int x = (width() - matchRes.width()) / 2;
                int y = (height() - matchRes.height()) / 2;
                painter.setOpacity(opacity3);
                painter.drawPixmap(x, y, matchRes);
            }
        }
        painter.setOpacity(1);
        break;
    }

}
}

void PaintArea::mousePressEvent(QMouseEvent *event)
{
    switch(canvasStatus){
    case 0:{
        switch(currShapeCode)
        {
        case Shape::Line:
        {
            shape = new Line;
            break;
        }
        case Shape::Rect:
        {
            shape = new Rect;
            break;
        }
        case Shape::Tri:
        {
            shape = new Tri;
            break;
        }
        case Shape::Rubber:
        {
            shape = new Rubber;
            break;
        }
        }

        if(shape != NULL) {
            perm = false;
            shape->setColor(currentPenColor);
            shape->setWidth(currentPenWidth);
            shape->setStart(event->pos());
            shape->setEnd(event->pos());
        }
    }
    }
    if(canvasStatus==1){
        // 可视化按下操作
        if (event->button() == Qt::LeftButton) {
            lastPoint = event->pos();
            isDrawing = true;
            update(); // 触发 paintEvent
        }

        // 查找最近的点并移动
        for (int i = 0; i < gridPoints.size(); ++i) {
            QPointF point = QPointF(gridPoints[i].x, gridPoints[i].y);
            if (QLineF(point, event->pos()).length() <= 5) {
                isDragging = true;
                draggedIndex = i;
                lastMousePos = event->pos();
                break;
            }
        }
        qDebug()<<"按下的位置是："<<lastMousePos;
    }
}

void PaintArea::mouseMoveEvent(QMouseEvent *event)
{
    switch(canvasStatus){
    case 0:
    {
        if(shape && !perm) {
            qDebug() << "当前为绘图模式";
            shape->setEnd(event->pos());
        }
        break;
    }
    case 1:
    {
        qDebug() << "当前为拖拽模式";

        // 获取鼠标当前位置
        mousePos = event->pos();
        // 重新绘制窗口以更新点的颜色
        update();

        // 若可拖拽则更新点的位置
        if (isDragging && draggedIndex >= 0 && draggedIndex < gridPoints.size()) {
            QPointF offset = event->pos() - lastMousePos;
            gridPoints[draggedIndex] += cv::Point2f(offset.x(), offset.y());
            lastMousePos = event->pos(); // 更新鼠标移动的位置
            update();
        }


        break;
    }
    case 2:
    {
        qDebug() << "当前为上传图片生成模式";
        break;
    }
    }
    update();
}

void PaintArea::mouseReleaseEvent(QMouseEvent *event)
{
    switch(canvasStatus){
    case 0:
    {
        if (shape) { // 确保绘制了有效的形状
            // shapeList << shape; // 将形状添加到列表中
            DrawCommand *command = new DrawCommand(shape, shapeList, undoStack);
            undoStack->push(command);
            qDebug() << "Command pushed to stack.";
            perm = true;
            shape = nullptr;
        }
        break;
    }
    case 1:
    {
        qDebug() << "当前为拖拽模式";
        // 绘制提示圆
        if (event->button() == Qt::LeftButton) {
            isDrawing = false;
            isFirstPress = false;
            update(); // 触发 paintEvent
        }

        if (isDragging) {
            isDragging = false;

            // 可回退
            DrawCommand *command = new DrawCommand(shape, shapeList, undoStack);
            undoStack->push(command);

            emit gridPointsUpdated(this->gridPoints);
            // 抬起鼠标时计算形变
            updateImg();
        }
        break;
    }
    case 2:
    {
        qDebug() << "当前为上传图片生成模式";

        break;
    }
    }
    update();
}

// ------网格函数-----
void PaintArea::initalizeGrid(){


    QPixmap pixmap;
    if(dragRes.isNull()){
        pixmap = this->grab(this->rect());
        qDebug()<<"拖拽结果为空";
    }
    else{
        // pixmap = dragRes;
        pixmap = this->grab(this->rect());
    }

    // 获取设备像素比
    qreal devicePixelRatio = pixmap.devicePixelRatio();
    QSize pixmapSize = pixmap.size();
    // 创建相同大小的 QImage
    QSize scaledSize = pixmap.size() / devicePixelRatio;
    QImage image = pixmap.scaled(scaledSize, Qt::IgnoreAspectRatio, Qt::SmoothTransformation).toImage();
    cv::Mat matBGR=QImageToCvMat(image);
    laplaciandeform deform;
    if(gridSize==0){gridSize=32;}
    std::vector<cv::Point2f> srcPoints = deform.initializeGrids(matBGR, gridSize);
    this->edges = deform.initializeEdges(srcPoints, matBGR.rows, matBGR.cols, gridSize);

    // 初始化点
    this->points = srcPoints;
    this->gridPoints = srcPoints;

    setImage(image);
}

void PaintArea::setGridSize(int size){
    gridSize = size;
    initalizeGrid();
    update();
}

void PaintArea::setDeformAlg(int type){
    std::cout<<"更换形变类型："<<deformAlg<<std::endl;
    deformAlg = type;
    update();
}

void PaintArea::setImage(const QImage &image) {
    this->image = image;
    update(); // 请求小部件重绘
}

QImage PaintArea::getImage(){
    return this->image;
}

void PaintArea::setKeyframe1(const QPixmap &pixmap){
    this->keyframe1=pixmap;
    update();
}

void PaintArea::setKeyframe2(const QPixmap &pixmap){
    this->keyframe2=pixmap;
    update();
}

void PaintArea::setOpacity1(int opacity){
    this->opacity1 = opacity/10;
    update();
}

void PaintArea::setOpacity2(int opacity){
    this->opacity2 = opacity/10;
    update();
}

void PaintArea::setOpacity3(int opacity){
    this->opacity3 = opacity/10;
    update();
}

void PaintArea::setKeyframeNum(int num){
    inbetweeningNum = num;
}


// 拖拽生成
void PaintArea::updateImg(){
    std::cout<<"形变类型："<<deformAlg<<std::endl;
    laplaciandeform deform;
    cv::Mat warpImg;
    cv::Mat img1 =QImageToCvMat(image);//image QImage类型

    std::vector<cv::Point2f> points = deform.initializeGrids(img1, gridSize);
    std::vector<std::pair<int, int>> edges = deform.initializeEdges(points, image.height(), image.width(), gridSize);
    // 拉普拉斯变换
    if(deformAlg==0){
    // 计算偏移量设置移动点
    std::map<std::pair<int, int>, std::pair<int, int>>  offset = deform.offset(points,gridPoints);
    const std::unordered_map<int, cv::Point2f>& movePoints = deform.setMovePoint(gridPoints);
    cv::Mat cvImage = QImageToCvMat(this->image);
    // std::unordered_map<int, cv::Point2f> fixedPoints = deform.setFixedPoint(cvImage,points,gridSize);
    std::unordered_map<int, cv::Point2f> fixedPoints;
    int cols=cvImage.cols;
    int rows=cvImage.rows;
    std::vector<cv::Point2f> dstPoints = deform.deformGrid(points,edges,fixedPoints,movePoints);

    int i = 0;
    for (const auto& pt : points) {
        if (pt.x == 0 || pt.x == cols  || pt.y == 0 || pt.y == rows ) {
            dstPoints[i] = cv::Point2f(pt.x, pt.y);
        }else if(calculateDistance(gridPoints[i],cv::Point2f(lastMousePos.x(), lastMousePos.y()))>diameter/2){
            dstPoints[i] = cv::Point2f(pt.x, pt.y);
        }
        i++;
    }
    gridPoints=dstPoints;
    deform.applyThinPlateSplineTransform(warpImg,img1,gridPoints,points);
    // 更新生成结果
    dragRes = QPixmap::fromImage(cvMat2QImage(warpImg));
    setImage(cvMat2QImage(warpImg));
    update();
    }
    else if(deformAlg==1){// ARAP

        cv::Mat cvImage = QImageToCvMat(this->image);
        int cols=cvImage.cols;
        int rows=cvImage.rows;
        ARAP arap;
        // input:
        int cols_num = cols / gridSize + 1;
        std::vector<cv::Point2f> dstPoints = arap.applyTransformation(points,gridPoints,cols_num);
        int i = 0;
        for (const auto& pt : points) {
            if (pt.x == 0 || pt.x == cols  || pt.y == 0 || pt.y == rows ) {
                dstPoints[i] = cv::Point2f(pt.x, pt.y);
            }
            std::cout<<"非边界点：("<<pt.x<<","<<pt.y<<")"<<std::endl;
            i++;
        }
        gridPoints=dstPoints;
        deform.applyThinPlateSplineTransform(warpImg,img1,gridPoints,points);
        // 更新生成结果
        dragRes = QPixmap::fromImage(cvMat2QImage(warpImg));
        setImage(cvMat2QImage(warpImg));
        update();

    }
}

void PaintArea::clearDragRes(){
    // dragRes = QPixmap();
    // 清除绘图列表防止切换状态时重绘
    shapeList.clear();
    undoStack->clear();

}


QPixmap PaintArea::getDragRes(){
    return this->dragRes;
}

void PaintArea::setResList(std::vector<QPixmap> dragResList){
    this->dragResList = dragResList;
}

void PaintArea::clearResList(){
    this->dragResList.clear();
}

void PaintArea::setCanPlay(){
    this->canPlay = true;
}

void PaintArea::setPlayRate(int rate){
    this->playRate = rate;
}

void PaintArea::setDiameter(int d){
    isFirstPress = true;
    diameter = d;
}

void PaintArea::playResList(){
    timer = new QTimer(this);
    connect(timer, SIGNAL(timeout()), this, SLOT(update()));
    if(playRate==0){playRate=18;}
    timer->start(int(3000/playRate));
}

// 根据关键帧生成
void PaintArea::generateInbetweening(){
    autoResList.clear();
    if(!keyframe1.isNull()&&!keyframe2.isNull()){
        cv::Mat img1 = QImageToCvMat(keyframe1.toImage());
        cv::Mat img2 = QImageToCvMat(keyframe2.toImage());

        // --正向生成
        siftmap SIFT;
        std::map<std::pair<int,int>,std::pair<int,int>> pointMap;
        pointMap = SIFT.usingSIFT(img1,img2);
        std::map<std::pair<int, int>, std::pair<int, int>>  offset = SIFT.offset(pointMap);
        // 形变
        laplaciandeform deform;
        gridSize=32;
        std::vector<cv::Point2f> srcPoints = deform.initializeGrids(img1, gridSize);
        std::vector<std::pair<int, int>> edges = deform.initializeEdges(srcPoints, img1.rows, img1.cols, gridSize);
        std::vector<cv::Point2f> dstPoints = deform.deform(img1,gridSize,offset);
        int cols=img1.cols;
        int rows=img1.rows;
        int i = 0;
        for (const auto& pt : srcPoints) {
            if (pt.x == 0 || pt.x == cols  || pt.y == 0 || pt.y == rows ) {
                dstPoints[i] = cv::Point2f(pt.x, pt.y);
            }
            i++;
        }
        if(inbetweeningNum==0||inbetweeningNum==1){
            cv::Mat warpImg;
            deform.applyThinPlateSplineTransform(warpImg,img1,dstPoints,srcPoints);
            matchRes=QPixmap::fromImage(cvMat2QImage(warpImg));
            update();
        }else{
        std::vector<cv::Point2f> temp(srcPoints.size());
        if(inbetweeningNum){
            for (int i=1;i<=int(inbetweeningNum);i++){
                for(int j=0;j<srcPoints.size();j++){
                    temp[j]=cv::Point2f(int(i*srcPoints[j].x+(inbetweeningNum-i)*dstPoints[j].x)/inbetweeningNum,
                                        int(i*srcPoints[j].y+(inbetweeningNum-i)*dstPoints[j].y)/inbetweeningNum);
                }
                cv::Mat tempImg ;
                deform.applyThinPlateSplineTransform(tempImg,img1,temp,srcPoints);
                autoResList.push_back(QPixmap::fromImage(cvMat2QImage(tempImg)));
            }
        }

        // 逆向生成
        pointMap = SIFT.usingSIFT(img2,img1);
        offset = SIFT.offset(pointMap);
        // 形变
        srcPoints = deform.initializeGrids(img1, gridSize);
        edges = deform.initializeEdges(srcPoints, img1.rows, img1.cols, gridSize);
        dstPoints = deform.deform(img1,gridSize,offset);
        cols=img2.cols;
        rows=img2.rows;
        i = 0;
        for (const auto& pt : srcPoints) {
            if (pt.x == 0 || pt.x == cols  || pt.y == 0 || pt.y == rows ) {
                dstPoints[i] = cv::Point2f(pt.x, pt.y);
            }
            i++;
        }

        if(inbetweeningNum){
            for (int i=1;i<=int(inbetweeningNum);i++){
                for(int j=0;j<srcPoints.size();j++){
                    temp[j]=cv::Point2f(int(i*srcPoints[j].x+(inbetweeningNum-i)*dstPoints[j].x)/inbetweeningNum,
                                          int(i*srcPoints[j].y+(inbetweeningNum-i)*dstPoints[j].y)/inbetweeningNum);
                }
                cv::Mat tempImg1 ;
                deform.applyThinPlateSplineTransform(tempImg1,img2,temp,srcPoints);
                autoResList.push_back(QPixmap::fromImage(cvMat2QImage(tempImg1)));
            }
        }
        }
    }
    update();

    }

void PaintArea::showKeyframe(){
    if(!keyframe1.isNull()){
        qreal devicePixelRatio=2;
        // 缩放适应大小
        QImage scaledImage = image.scaled(size() * devicePixelRatio, Qt::KeepAspectRatio, Qt::SmoothTransformation);
        scaledImage.setDevicePixelRatio(devicePixelRatio);
        QPainter painter(this);
        painter.drawImage(0, 0, scaledImage);
    }
}

std::vector<QPixmap> PaintArea::getAutoResList(){
    return autoResList;
}

cv::Mat PaintArea::QImageToCvMat(const QImage &image) {
    QImage img = image.convertToFormat(QImage::Format_RGBA8888);

    // 获取设备像素比
    qreal devicePixelRatio = img.devicePixelRatio();
    QImage scaledImage = img.scaled(img.width() , img.height(), Qt::KeepAspectRatio, Qt::SmoothTransformation);
    scaledImage.setDevicePixelRatio(devicePixelRatio);
    // 创建 cv::Mat 并将数据指向 QImage 数据
    cv::Mat mat(scaledImage.height(), scaledImage.width(), CV_8UC4, const_cast<uchar*>(scaledImage.bits()), scaledImage.bytesPerLine());

    // 转换为 BGR 格式
    cv::Mat matBGR;
    cv::cvtColor(mat, matBGR, cv::COLOR_BGRA2BGR);
    return matBGR.clone(); // 返回深拷贝的 cv::Mat 对象
}

QImage PaintArea::cvMat2QImage(const cv::Mat &mat)
{
    QImage image;
    switch(mat.type())
    {
    case CV_8UC1:
        // QImage构造：数据，宽度，高度，每行多少字节，存储结构
        image = QImage((const unsigned char*)mat.data, mat.cols, mat.rows, mat.step, QImage::Format_Grayscale8);
        break;
    case CV_8UC3:
        image = QImage((const unsigned char*)mat.data, mat.cols, mat.rows, mat.step, QImage::Format_RGB888);
        //image = image.rgbSwapped(); // BRG转为RGB
        // Qt5.14增加了Format_BGR888
        // image = QImage((const unsigned char*)mat.data, mat.cols, mat.rows, mat.cols * 3, QImage::Format_BGR888);
        break;
    case CV_8UC4:
        image = QImage((const unsigned char*)mat.data, mat.cols, mat.rows, mat.step, QImage::Format_ARGB32);
        break;
    case CV_16UC4:
        image = QImage((const unsigned char*)mat.data, mat.cols, mat.rows, mat.step, QImage::Format_RGBA64);
        // image = image.rgbSwapped(); // BRG转为RGB
        break;
    }
    return image;
}

// -----绘图函数-------

void PaintArea::undo() {


    if (undoStack->canUndo()) {
        undoStack->undo();
    } else {
        qDebug() << "No more actions to undo.";
    }
    this->update();
}

void PaintArea::redo(){
    undoStack->redo();
    this->update();
}

void PaintArea::clear(){
    QDialog dialog(this);
    dialog.setWindowTitle(tr("清除画布"));

    QVBoxLayout* mainLayout = new QVBoxLayout();

    // 创建图标
    QLabel* iconLabel = new QLabel();
    QPixmap iconPixmap(":/new/prefix2/icons/hint.png");
    iconLabel->setPixmap(iconPixmap.scaled(32, 32, Qt::KeepAspectRatio, Qt::SmoothTransformation));
    mainLayout->addWidget(iconLabel, 0, Qt::AlignCenter);

    // 创建提示信息
    QLabel* messageLabel = new QLabel(tr("你想清除画布上的所有内容吗？此操作不可撤销。"));
    mainLayout->addWidget(messageLabel, 0, Qt::AlignCenter);

    // 创建按钮
    QHBoxLayout* buttonLayout = new QHBoxLayout();
    QPushButton* yesButton = new QPushButton(tr("是"));
    QPushButton* noButton = new QPushButton(tr("否"));
    buttonLayout->addWidget(yesButton);
    buttonLayout->addWidget(noButton);
    mainLayout->addLayout(buttonLayout);

    dialog.setLayout(mainLayout);

    // 连接按钮信号
    QObject::connect(yesButton, &QPushButton::clicked, [&]() {
        shapeList.clear();  // 清除形状列表
        undoStack->clear(); // 清除撤销栈
        this->update();     // 重绘界面
        dialog.accept();    // 关闭并接受对话框
    });

    QObject::connect(noButton, &QPushButton::clicked, &dialog, &QDialog::reject);

    dialog.exec();
}

// 计算距离
double PaintArea::calculateDistance(const cv::Point2f& pt1, const cv::Point2f& pt2) {
    return std::sqrt(std::pow(pt1.x - pt2.x, 2) + std::pow(pt1.y - pt2.y, 2));
}

void PaintArea::clearDrawItem(){
    shapeList.clear();
    undoStack->clear();
}



