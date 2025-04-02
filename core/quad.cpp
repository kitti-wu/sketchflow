/*
 * SPDX-FileCopyrightText: 2017-2023 Pierre Benard <pierre.g.benard@inria.fr>
 * SPDX-FileCopyrightText: 2021-2023 Melvin Even <melvin.even@inria.fr>
 *
 * SPDX-License-Identifier: CECILL-2.1
 */

#include "quad.h"
#include "corner.h"

void Quad::clear() {
    //m_elements.clear();
    for (int i = 0; i < 4; ++i) {
        m_centroid[i] = Point::VectorType::Zero();
        corners[i] = nullptr;
    }
}

void Quad::computeCentroid(PosTypeIndex type) {
    m_centroid[type] = Point::VectorType::Zero();

    for (int i = 0; i < 4; ++i) {
        m_centroid[type] += corners[i]->coord(type);
    }

    m_centroid[type] *= 0.25;
}

void Quad::computeCentroids() {
    for (int i = 0; i < 4; ++i) {
        m_centroid[i] = Point::VectorType::Zero();
    }

    for (int i = 0; i < 4; i++) {
        for (int j = 0; j < 4; j++) {
            m_centroid[i] += corners[j]->coord((PosTypeIndex)i);
        }
    }

    for (int i = 0; i < 4; ++i) {
        m_centroid[i] *= 0.25;
    }
}

Point::VectorType Quad::biasedCentroid(PosTypeIndex type) const {
    if (!m_pinned) {
        return centroid(type);
    }

    Point::VectorType bCentroid = Point::VectorType::Zero();
    double weight = 10000.0;
    for (int i = 0; i < 4; ++i) {
        bCentroid += corners[i]->coord(type);
    }
    if (type != TARGET_POS) {
        bCentroid += getPoint(m_pinUV, type) * weight;
    } else {
        bCentroid += m_pinPosition * weight;
    }
    bCentroid /= (4 + weight);

    return bCentroid;
}


Point::VectorType Quad::getPoint(const Point::VectorType &uv, PosTypeIndex type) const {
    return (corners[TOP_LEFT]->coord(type) * (1.0 - uv.x()) + corners[TOP_RIGHT]->coord(type) * uv.x()) * (1.0 - uv.y()) // top edge
           + (corners[BOTTOM_LEFT]->coord(type) * (1.0 - uv.x()) + corners[BOTTOM_RIGHT]->coord(type) * uv.x()) * uv.y();  // bottom edge
}



