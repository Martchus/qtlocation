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
#ifndef QGEOMAPCONTROLLER_P_H
#define QGEOMAPCONTROLLER_P_H

//
//  W A R N I N G
//  -------------
//
// This file is not part of the Qt API.  It exists purely as an
// implementation detail.  This header file may change from version to
// version without notice, or even be removed.
//
// We mean it.
//

#include <QObject>

#include "qgeocoordinate.h"
#include "qgeocameradata_p.h"

QT_BEGIN_NAMESPACE

class QGeoMap;

class Q_LOCATION_EXPORT AnimatableCoordinate {
public:
    AnimatableCoordinate();
    AnimatableCoordinate(const QGeoCoordinate &coordinate,
                         QSharedPointer<QGeoProjection> projection);

    QGeoCoordinate coordinate() const;
    void setCoordinate(const QGeoCoordinate &coordinate);

    QSharedPointer<QGeoProjection> projection() const;
    void setProjection(QSharedPointer<QGeoProjection> projection);

private:
    QGeoCoordinate coordinate_;
    QSharedPointer<QGeoProjection> projection_;
};

class Q_LOCATION_EXPORT QGeoMapController : public QObject
{
    Q_OBJECT

    Q_PROPERTY(AnimatableCoordinate center READ center WRITE setCenter NOTIFY centerChanged)
    Q_PROPERTY(qreal bearing READ bearing WRITE setBearing NOTIFY bearingChanged)
    Q_PROPERTY(qreal tilt READ tilt WRITE setTilt NOTIFY tiltChanged)
    Q_PROPERTY(qreal roll READ roll WRITE setRoll NOTIFY rollChanged)
    Q_PROPERTY(qreal zoom READ zoom WRITE setZoom NOTIFY zoomChanged)

public:
    QGeoMapController(QGeoMap *map, QSharedPointer<QGeoProjection> projection);
    ~QGeoMapController();

    AnimatableCoordinate center() const;
    void setCenter(const AnimatableCoordinate &center);

    qreal bearing() const;
    void setBearing(qreal bearing);

    qreal tilt() const;
    void setTilt(qreal tilt);

    qreal roll() const;
    void setRoll(qreal roll);

    qreal zoom() const;
    void setZoom(qreal zoom);

    void pan(qreal dx, qreal dy);

private slots:
    void cameraDataChanged(const QGeoCameraData &cameraData);

signals:
    void centerChanged(const AnimatableCoordinate &center);
    void bearingChanged(qreal bearing);
    void tiltChanged(qreal tilt);
    void rollChanged(qreal roll);
    void zoomChanged(qreal zoom);

private:
    QGeoMap *map_;
    QSharedPointer<QGeoProjection> projection_;
    QGeoCameraData oldCameraData_;
};

QT_END_NAMESPACE

Q_DECLARE_METATYPE(AnimatableCoordinate)

#endif // QGEOMAPCONTROLLER_P_H