#include "rubber.h"

Rubber::Rubber()
{
}

void Rubber::paint(QPainter &painter)
{
    QPen pen;
    pen.setColor(Qt::white);
    pen.setWidth(width);
    painter.setPen(pen);
    painter.drawLine(start, end);
}
