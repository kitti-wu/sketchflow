/*
 * SPDX-FileCopyrightText: 2021-2023 Melvin Even <melvin.even@inria.fr>
 *
 * SPDX-License-Identifier: CECILL-2.1
 */

#ifndef __STROKEINTERVAL_H__
#define __STROKEINTERVAL_H__

#include <QtGlobal>
#include <QTransform>
#include <QList>
#include <QHash>

#include "point.h"

#define BETWEEN_INC(x, low, up) (x >= low && x <= up)

class Stroke;
class Lattice;
class VectorKeyFrame;

class StrokeInterval {
public:

    StrokeInterval(int fromId, int toId)
        : m_fromId(fromId),
        m_toId(toId),
        m_canOvershoot(true) { }

    StrokeInterval(const StrokeInterval &other)
        : m_fromId(other.m_fromId),
        m_toId(other.m_toId),
        m_canOvershoot(other.m_canOvershoot) { }

    ~StrokeInterval() { }

    inline unsigned int from() const { return m_fromId; }
    inline unsigned int to() const { return m_toId; }
    inline bool canOvershoot() const { return m_canOvershoot; }
    inline int nbPoints() const { return m_toId - m_fromId + 1; }
    void setOvershoot(bool overshoot) { m_canOvershoot = overshoot; }

    inline bool intersects(const StrokeInterval &other) const {
        return BETWEEN_INC(m_fromId, other.m_fromId, other.m_toId) || BETWEEN_INC(m_toId, other.m_fromId, other.m_toId)
               || BETWEEN_INC(other.m_fromId, m_fromId, m_toId)       || BETWEEN_INC(other.m_toId, m_fromId, m_toId);
    }

    inline void merge(const StrokeInterval &other) {
        m_fromId = std::min(m_fromId, other.m_fromId);
        m_toId = std::max(m_toId, other.m_toId);
    }

private:
    int m_fromId, m_toId;
    bool m_canOvershoot;    // false if the next point after toIdx doesn't exist or cannot be embedded in the lattice
};

typedef StrokeInterval Interval;
// typedef QList<Interval> Intervals;

class Intervals : private QList<Interval> {
public:

    void append(const Interval &interval);
    void append(const Intervals &intervals);

    void clear() { QList<Interval>::clear(); m_nbPoints = 0; }

    const Interval &at(unsigned int idx) const { return QList<Interval>::at(idx); }
    const Interval &front() const { return QList<Interval>::front(); }
    const Interval &back() const { return QList<Interval>::back(); }
    Interval &front() { return QList<Interval>::front(); }
    Interval &back() { return QList<Interval>::back(); }

    QList<Interval>::iterator begin() { return QList<Interval>::begin(); }
    QList<Interval>::const_iterator begin() const { return QList<Interval>::cbegin(); }
    QList<Interval>::const_iterator cbegin() const { return QList<Interval>::cbegin(); }

    QList<Interval>::iterator end() { return QList<Interval>::end(); }
    QList<Interval>::const_iterator end() const { return QList<Interval>::cend(); }
    QList<Interval>::const_iterator cend() const { return QList<Interval>::cend(); }

    size_t size() const { return QList<Interval>::size(); }
    bool empty() const { return QList<Interval>::empty(); }
    int nbPoints() const { return m_nbPoints; }

private:
    int m_nbPoints{0};
};

class StrokeIntervals : public QHash<unsigned int, Intervals> {
public:
    void forEachPoint(const VectorKeyFrame *key, std::function<void(Point *)> func, unsigned int id) const;
    void forEachPoint(const VectorKeyFrame *key, std::function<void(Point *)> func) const;
    void forEachPoint(const VectorKeyFrame *key, std::function<void(Point *, unsigned int sId, unsigned int pId)> func, unsigned int id) const;
    void forEachPoint(const VectorKeyFrame *key, std::function<void(Point *, unsigned int sId, unsigned int pId)> func) const;

    void forEachInterval(std::function<void(const Interval &)> func) const;
    void forEachInterval(std::function<void(const Interval &, unsigned int sId)> func) const;

    inline int nbPoints() const {
        int _nbPoints = 0;
        for (auto it = constBegin(); it != constEnd(); ++it) _nbPoints += it.value().nbPoints();
        return _nbPoints;
    }
};

#endif // __STROKEINTERVAL_H__m_uvs
