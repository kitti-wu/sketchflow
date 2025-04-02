/*
 * SPDX-FileCopyrightText: 2017-2023 Pierre Benard <pierre.g.benard@inria.fr>
 * SPDX-FileCopyrightText: 2021-2023 Melvin Even <melvin.even@inria.fr>
 *
 * SPDX-License-Identifier: CECILL-2.1
 */

#ifndef STROKE_H
#define STROKE_H

#include <QTransform>

#include <eigen3/Eigen/Core>
#include <eigen3/Eigen/Geometry>

#include "point.h"

#include <QBrush>
#include <QColor>
#include <QPainter>
#include <QPen>
#include <QTextStream>
#include <QtXml/QDomDocument>
#include <QtXml/QDomElement>


class Point;
class Group;

class Stroke {
public:
    Stroke(unsigned int id, const QColor &c, double thickness = 1.5, bool _isScribble = false);
    Stroke(const Stroke &s);
    Stroke(const Stroke &s, unsigned int id, int from, int to);
    ~Stroke();




private:

    // stroke properties
    QColor m_color;
    double m_strokeWidth;
    bool m_isScribble;
    QPolygonF m_outline;


    unsigned int m_id;
    int m_canHashId = -1;
};

typedef std::shared_ptr<Stroke> StrokePtr;

#endif
