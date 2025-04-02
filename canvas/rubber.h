#ifndef RUBBER_H
#define RUBBER_H
#include "shape.h"

class Rubber : public Shape
{
public:
    Rubber();

    void paint(QPainter &painter);
};

#endif // RUBBER_H
