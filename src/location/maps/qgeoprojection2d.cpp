/****************************************************************************
**
** Copyright (C) 2012 Nokia Corporation and/or its subsidiary(-ies).
** Contact: http://www.qt-project.org/
**
** This file is part of the QtLocation module of the Qt Toolkit.
**
** $QT_BEGIN_LICENSE:LGPL$
** GNU Lesser General Public License Usage
** This file may be used under the terms of the GNU Lesser General Public
** License version 2.1 as published by the Free Software Foundation and
** appearing in the file LICENSE.LGPL included in the packaging of this
** file. Please review the following information to ensure the GNU Lesser
** General Public License version 2.1 requirements will be met:
** http://www.gnu.org/licenses/old-licenses/lgpl-2.1.html.
**
** In addition, as a special exception, Nokia gives you certain additional
** rights. These rights are described in the Nokia Qt LGPL Exception
** version 1.1, included in the file LGPL_EXCEPTION.txt in this package.
**
** GNU General Public License Usage
** Alternatively, this file may be used under the terms of the GNU General
** Public License version 3.0 as published by the Free Software Foundation
** and appearing in the file LICENSE.GPL included in the packaging of this
** file. Please review the following information to ensure the GNU General
** Public License version 3.0 requirements will be met:
** http://www.gnu.org/copyleft/gpl.html.
**
** Other Usage
** Alternatively, this file may be used in accordance with the terms and
** conditions contained in a signed written agreement between you and Nokia.
**
**
**
**
**
**
** $QT_END_LICENSE$
**
****************************************************************************/
#include "qgeoprojection2d_p.h"

#include "qgeocoordinate.h"

#include "qdoublevector2d_p.h"
#include "qdoublevector3d_p.h"

#include <qnumeric.h>

QT_BEGIN_NAMESPACE

QGeoProjection2D::QGeoProjection2D(double baseHeight, double sideLength)
    : baseHeight_(baseHeight), sideLength_(sideLength) {}

QGeoProjection2D::~QGeoProjection2D() {}

QDoubleVector3D QGeoProjection2D::coordToPoint(const QGeoCoordinate &coord) const
{
    QDoubleVector2D m = coordToMercator(coord);
    double z = baseHeight_;
    if (!qIsNaN(coord.altitude()))
        z += coord.altitude();
    return QDoubleVector3D(m.x() * sideLength_, (1.0 - m.y()) * sideLength_, z);
}

QGeoCoordinate QGeoProjection2D::pointToCoord(const QDoubleVector3D &point) const
{
    QDoubleVector2D m = QDoubleVector2D(point.x() / sideLength_, 1.0 - point.y() / sideLength_);
    QGeoCoordinate coord = mercatorToCoord(m);
    coord.setAltitude(point.z() - baseHeight_);
    return coord;
}

QDoubleVector3D QGeoProjection2D::mercatorToPoint(const QDoubleVector2D &mercator) const
{
    return QDoubleVector3D(mercator.x() * sideLength_, (1.0 - mercator.y()) * sideLength_, baseHeight_);
}

QDoubleVector2D QGeoProjection2D::pointToMercator(const QDoubleVector3D &point) const
{
    return QDoubleVector2D(point.x() / sideLength_, 1.0 - (point.y() / sideLength_));
}

QGeoCoordinate QGeoProjection2D::interpolate(const QGeoCoordinate &start, const QGeoCoordinate &end, qreal progress)
{
    if (start == end) {
        if (progress < 0.5) {
            return start;
        } else {
            return end;
        }
    }

    QGeoCoordinate s2 = start;
    s2.setAltitude(0.0);
    QGeoCoordinate e2 = end;
    e2.setAltitude(0.0);
    QDoubleVector3D s = coordToPoint(s2);
    QDoubleVector3D e = coordToPoint(e2);

    double x = s.x();

    if (sideLength_ / 2.0 < qAbs(e.x() - s.x())) {
        // handle dateline crossing
    } else {
        x = (1.0 - progress) * s.x() + progress * e.x();
    }

    double y = (1.0 - progress) * s.y() + progress * e.y();

    QGeoCoordinate result = pointToCoord(QDoubleVector3D(x, y, 0.0));
    result.setAltitude((1.0 - progress) * start.altitude() + progress * end.altitude());
    return result;
}

QT_END_NAMESPACE