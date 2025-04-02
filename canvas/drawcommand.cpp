#include "drawcommand.h"

DrawCommand::DrawCommand(Shape* shape, QList<Shape*>& shapeList, QUndoStack* undoStack)
    : QUndoCommand(), m_shape(shape), m_shapeList(shapeList), m_undoStack(undoStack) {
    setText("Draw shape"); // 为撤销/重做操作设置文本标签
}

void DrawCommand::redo() {
    qDebug() << "Redoing: adding shape";
    m_shapeList.append(m_shape); // 实现添加形状的逻辑

}

void DrawCommand::undo() {
    m_shapeList.removeOne(m_shape); // 实现撤销添加形状的逻辑
}

void DrawCommand::clear(){
    m_shapeList.clear();
}
