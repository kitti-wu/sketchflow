#include "paintwidget.h"

PaintWidget::PaintWidget(QWidget *parent)
    : QWidget(parent),currShapeCode(Shape::Line), shape(NULL), perm(false), undoStack(new QUndoStack(this))
{
    //setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Expanding);
    //setMouseTracking(true);
}



void PaintWidget::showImage(const QImage& image) {
    //ImageWindow *imageWindow = new ImageWindow(image);
    //imageWindow->show();
}

void PaintWidget::paintEvent(QPaintEvent *event)
{
    QPainter painter(this);
    switch(canvasStatus){
    case 0:
    {
        painter.setBrush(Qt::white);
        painter.drawRect(0, 0, size().width(), size().height());
        qDebug() << "Painting shapes, current list size:" << shapeList.size();
        for(Shape * shape: shapeList) {
            shape->paint(painter);
            qDebug() << shape->startPoint()<<shape->endPoint();
        }

        break;
    }
    case 1:
    {
        qDebug() << "当前为拖拽模式";
        if (!image.isNull()) {
            qDebug() << "image不为空";
            painter.drawImage(0, 0, image);
            //showImage(image);
        }
        else{qDebug() << "image为空";}

        // 绘制点
        for (const auto& point : gridPoints) {
            QPointF qtPoint(point.x, point.y);
            int distance = qSqrt(qPow(mousePos.x() - qtPoint.x(), 2) + qPow(mousePos.y() - qtPoint.y(), 2));
            painter.setPen(Qt::blue);
            painter.setBrush(Qt::blue);
            if (distance < 8) {
                painter.drawEllipse(qtPoint, 5, 5);
            } else {
                painter.drawEllipse(qtPoint, 2, 2);
            }
        }



        break;
    }
    case 2:
    {
        qDebug() << "当前为上传图片生成模式";
        break;
    }
    }
}

void PaintWidget::mousePressEvent(QMouseEvent *event)
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

        qDebug()<<"抬起的位置是："<<lastMousePos;
    }
}

void PaintWidget::mouseMoveEvent(QMouseEvent *event)
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
        qDebug() << "鼠标位置为："<<mousePos.x()<<mousePos.y();
        // 重新绘制窗口以更新点的颜色
        update();

        // 若可拖拽则更新点的位置
        if (isDragging && draggedIndex >= 0 && draggedIndex < gridPoints.size()) {
            QPointF offset = event->pos() - lastMousePos;
            gridPoints[draggedIndex] += cv::Point2f(offset.x(), offset.y());
            lastMousePos = event->pos();
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

void PaintWidget::mouseReleaseEvent(QMouseEvent *event)
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
        if (isDragging) {
            isDragging = false;
            // emit gridPointsUpdated(this->gridPoints); // Emit signal with updated grid points
            // updateImg();
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

void PaintWidget::undo() {
    qDebug() << "Current undo stack count:" << undoStack->count();
    qDebug() << "Undo stack index before attempt:" << undoStack->index();

    if (undoStack->canUndo()) {
        undoStack->undo();
        qDebug() << "Attempting to undo...";
    } else {
        qDebug() << "No more actions to undo.";
    }
    this->update();
}

void PaintWidget::redo(){
    undoStack->redo();
    this->update();
}

void PaintWidget::clear(){
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

