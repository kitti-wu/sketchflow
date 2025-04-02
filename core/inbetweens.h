/*
 * SPDX-FileCopyrightText: 2021-2023 Melvin Even <melvin.even@inria.fr>
 *
 * SPDX-License-Identifier: CECILL-2.1
 */

#ifndef __INBETWEENS_H__
#define __INBETWEENS_H__

#include <vector>
#include <QHash>
#include "stroke.h"
#include "point.h"
#include "uvhash.h"

struct Inbetween {
    QHash<int, StrokePtr> strokes;
    QHash<int, StrokePtr> backwardStrokes;
    QHash<int, std::vector<Point::VectorType>> corners;


};

class Inbetweens : public std::vector<Inbetween> {
public:
    void makeDirty();


};

#endif // __INBETWEENS_H__
