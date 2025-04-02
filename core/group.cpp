/*
 * SPDX-FileCopyrightText: 2017-2023 Pierre Benard <pierre.g.benard@inria.fr>
 * SPDX-FileCopyrightText: 2021-2023 Melvin Even <melvin.even@inria.fr>
 *
 * SPDX-License-Identifier: CECILL-2.1
 */

#include "group.h"

#include <math.h>

#include <QDomText>
#include <QString>
#include <QTextStream>
#include <QBrush>

#include "geom.h"
#include "dialsandknobs.h"

static dkBool k_displayGrids("Warp->Display grids", true);


Group::Group(const Group &other){

}

Group::~Group() {
}

void Group::reset() {}

void Group::loadStrokes(QDomElement &strokesElt, uint size) {
}

void Group::load(QDomNode &groupNode) {


}

void Group::save(QDomDocument &doc, QDomElement &groupsElt) const {
}
