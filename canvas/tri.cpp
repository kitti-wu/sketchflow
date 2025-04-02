#include "tri.h"

Tri::Tri() {}

void Tri::paint(QPainter &painter)
{
    QPen pen;
    pen.setColor(color);
    pen.setWidth(width);
    painter.setPen(pen);
    painter.drawEllipse(start.x(), start.y(),end.x() - start.x(), end.y() - start.y());

}
