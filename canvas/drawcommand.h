#ifndef DRAWCOMMAND_H
#define DRAWCOMMAND_H

#include <QUndoCommand>
#include "shape.h"
#include <QList>

class DrawCommand : public QUndoCommand {
public:
    DrawCommand(Shape* shape, QList<Shape*>& shapeList, QUndoStack* undoStack);

    void redo() override;
    void undo() override;
    void clear();

private:
    Shape* m_shape;
    QList<Shape*>& m_shapeList;
    QUndoStack* m_undoStack;
};

#endif // DRAWCOMMAND_H
