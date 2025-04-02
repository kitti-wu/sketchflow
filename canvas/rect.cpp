#include "rect.h"

Rect::Rect()
{
}

void Rect::paint(QPainter &painter)
{
    QPen pen;
    pen.setColor(color);
    pen.setWidth(width);
    painter.setPen(pen);
    painter.drawRect(start.x(), start.y(),end.x() - start.x(), end.y() - start.y());
}
