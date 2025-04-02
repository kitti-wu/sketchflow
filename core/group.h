/*
 * SPDX-FileCopyrightText: 2017-2023 Pierre Benard <pierre.g.benard@inria.fr>
 * SPDX-FileCopyrightText: 2021-2023 Melvin Even <melvin.even@inria.fr>
 *
 * SPDX-License-Identifier: CECILL-2.1
 */

#ifndef GROUP_H
#define GROUP_H

#include <QBrush>
#include <QColor>
#include <QDomDocument>
#include <QDomElement>
#include <QDomNode>
#include <QPainter>
#include <QPen>
#include <QRect>
#include <QTransform>
#include <QHash>
#include <functional>

#include "point.h"
#include "strokeinterval.h"
#include "uvhash.h""

enum GroupType : unsigned int { PRE, POST, MAIN };

class VectorKeyFrame;
class KeyframedTransform;
class KeyframedFloat;
class Editor;

class Group {
public:
    Group(VectorKeyFrame *keyframe, GroupType type);
    Group(VectorKeyFrame *keyframe, const QColor &color, GroupType type);
    Group(const Group& other);
    ~Group();

    // lattice
    Lattice *lattice() const { return m_grid.get(); }

    static void reset();
    void loadStrokes(QDomElement &strokesElt, uint size);
    void load(QDomNode &groupNode);
    void save(QDomDocument &doc, QDomElement &groupsElt) const;
    void update();
    void makeBreakdown(VectorKeyFrame *newKeyframe, VectorKeyFrame *nextKeyframe, Group *breakdown, int inbetween, float linearAlpha, const Point::Affine &rigidTransform, const QHash<int, int> &backwardStrokesMap, Editor *editor);

    // lattice
    std::shared_ptr<Lattice> m_grid;
    UVHash m_forwardUVs, m_backwardUVs;
    bool m_showGrid;        // true if the lattice can be displayed
    bool m_breakdown;
    bool m_disappear;
    int m_prevPreGroupId;   // only valid if m_type == POST, cache for VectorKeyFrame m_intraCorrespondences

    // interpolation results
    std::vector<Stroke *> m_origin_strokes;
    std::vector<std::vector<Stroke *>> m_inter_strokes;
    std::vector<std::vector<Stroke *>> m_candidate_strokes;
    bool m_showInterStroke{false};
    bool m_interpolated{false};
};

#endif

