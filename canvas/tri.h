#ifndef TRI_H
#define TRI_H
#include "shape.h"

class Tri : public Shape
{
public:
    Tri();

    void paint(QPainter &painter);
};

#endif // TRI_H
