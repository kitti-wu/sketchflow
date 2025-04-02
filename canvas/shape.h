#ifndef SHAPE_H
#define SHAPE_H

#include <QtGui>
class Shape
{
public:

    enum Code {
        Line,
        Rect,
        Tri,
        Rubber
    };

    Shape();

    void setStart(QPoint s)
    {
        start = s;
    }

    void setEnd(QPoint e)
    {
        end = e;
    }

    void setColor(QColor c)
    {
        color = c;
    }

    void setWidth(int w)
    {
        width = w;
    }

    QPoint startPoint()
    {
        return start;
    }

    QPoint endPoint()
    {
        return end;
    }

    void virtual paint(QPainter & painter) = 0;

protected:
    QPoint start;
    QPoint end;
    QColor color;
    int width;
};
#endif // SHAPE_H
