/*
 * SPDX-FileCopyrightText: 2017-2023 Pierre Benard <pierre.g.benard@inria.fr>
 * SPDX-FileCopyrightText: 2021-2023 Melvin Even <melvin.even@inria.fr>
 *
 * SPDX-License-Identifier: CECILL-2.1
 */

#ifndef QUAD_H
#define QUAD_H

#include <memory>
#include <QHash>
#include <QTransform>
#include "point.h"
#include "strokeinterval.h"

class Corner;

typedef enum { TARGET_POS = 0, REF_POS, INTERP_POS, DEFORM_POS, NUM_COORDS } PosTypeIndex;

/**
 * A cell of the lattice
 * Keeps track of the stroke intervals embedded in it (StrokeIntervals)
 */
class Quad {
public:
    Quad() : m_key(-1), m_flag(false), m_volatile(false), m_pinned(false) { clear(); }
    Quad(int key) : m_key(key), m_flag(false), m_volatile(false), m_pinned(false) { clear(); }

    void clear();
    void removeStroke(int strokeId);

    bool flag() const { return m_flag; }
    bool isVolatile() const { return m_volatile; }
    int key() const { return m_key; }
    void setKey(int k) { m_key = k; }
    void setFlag(bool flag) { m_flag = flag; }
    void setVolatile(bool _volatile) { m_volatile = _volatile; }
    bool isPinned() const { return m_pinned; }
    Point::VectorType pinPos() const { return m_pinPosition; }
    Point::VectorType pinUV() const { return m_pinUV; }
    void pin(const Point::VectorType &uv);
    void pin(const Point::VectorType &uv, const Point::VectorType &pos);
    void unpin();

    Point::VectorType centroid(PosTypeIndex type) const { return m_centroid[type]; }
    Point::VectorType biasedCentroid(PosTypeIndex type) const;
    void computeCentroid(PosTypeIndex type);
    void computeCentroids();
    Point::VectorType getPoint(const Point::VectorType &uv, PosTypeIndex type) const;
    Point::Affine optimalRigidTransform(PosTypeIndex source, PosTypeIndex target);
    Point::Affine optimalAffineTransform(PosTypeIndex source, PosTypeIndex target);

    // StrokeIntervals


    Corner *corners[4];  // pointers to the 4 corners of the quad (public for legacy reasons)
private:
    int m_key;                                          // quad ID (see lattice posToKey)
    bool m_flag;                                        // tmp flag for search and other misc operations
    bool m_volatile;                                    // true if the quad should be erased after modification of the grid (e.g. the quad has been added only to embed backward strokes)
    //StrokeIntervals m_elements;                         // stroke intervals embedded in this quad
    Point::VectorType m_centroid[4];                    // quad centroid in its different configuration (REF, TARGET, ...)
    Point::VectorType m_pinPosition;                    // position of the pin in the canvas
    Point::VectorType m_pinUV;                          // barycentric coordinate of the pin in the quad
    bool m_pinned;                                      // true if the quad is pinned
};

typedef std::shared_ptr<Quad> QuadPtr;

#endif  // QUAD_H
