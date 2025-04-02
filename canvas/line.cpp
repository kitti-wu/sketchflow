#include "line.h"

Line::Line()
{
}

void Line::paint(QPainter &painter)
{
    QPen pen;
    pen.setColor(color);
    pen.setWidth(width);
    painter.setPen(pen);
    painter.drawLine(start, end);
}
