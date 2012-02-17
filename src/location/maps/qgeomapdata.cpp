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
#include "qgeomapdata_p.h"
#include "qgeomapdata_p_p.h"
#include "qgeomap_p.h"

#include "qgeotilecache_p.h"
#include "qgeotilespec.h"
#include "qgeoprojection_p.h"
#include "qgeocameracapabilities_p.h"
#include "qgeomapcontroller_p.h"
#include "qdoublevector2d_p.h"
#include "qdoublevector3d_p.h"

#include "qgeocameratiles_p.h"
#include "qgeomapimages_p.h"
#include "qgeomapgeometry_p.h"

#include "qgeomappingmanager.h"

#include <QMutex>
#include <QMap>

#include <qnumeric.h>

#include <qglscenenode.h>
#include <qgeometrydata.h>
#include <qglbuilder.h>
#include <Qt3D/qglpainter.h>
#include <Qt3D/qgeometrydata.h>
#include <Qt3D/qglbuilder.h>
#include <Qt3D/qglcamera.h>
#include <Qt3D/qglsubsurface.h>

#include <cmath>

QT_BEGIN_NAMESPACE

QGeoMapData::QGeoMapData(QGeoMappingManagerEngine *engine, QObject *parent)
    : QObject(parent),
      d_ptr(new QGeoMapDataPrivate(engine, this)) {}

QGeoMapData::~QGeoMapData()
{
    delete d_ptr;
}

QGeoMapController* QGeoMapData::mapController()
{
    Q_D(QGeoMapData);
    return d->mapController();
}

QGLCamera* QGeoMapData::glCamera() const
{
    Q_D(const QGeoMapData);
    return d->glCamera();
}

void QGeoMapData::resize(int width, int height)
{
    Q_D(QGeoMapData);
    d->resize(width, height);

    // always emit this signal to trigger items to redraw
    emit cameraDataChanged(d->cameraData());
}

int QGeoMapData::width() const
{
    Q_D(const QGeoMapData);
    return d->width();
}

int QGeoMapData::height() const
{
    Q_D(const QGeoMapData);
    return d->height();
}

void QGeoMapData::setCameraData(const QGeoCameraData &cameraData)
{
    Q_D(QGeoMapData);

    if (cameraData == d->cameraData())
        return;

    d->setCameraData(cameraData);
    update();

    emit cameraDataChanged(d->cameraData());
}

QGeoCameraData QGeoMapData::cameraData() const
{
    Q_D(const QGeoMapData);
    return d->cameraData();
}

void QGeoMapData::update()
{
    emit updateRequired();
}

void QGeoMapData::setActiveMapType(const QGeoMapType type)
{
    Q_D(QGeoMapData);
    d->setActiveMapType(type);
}

const QGeoMapType QGeoMapData::activeMapType() const
{
    Q_D(const QGeoMapData);
    return d->activeMapType();
}

QString QGeoMapData::pluginString()
{
    Q_D(QGeoMapData);
    return d->pluginString();
}

QSharedPointer<QGeoCoordinateInterpolator> QGeoMapData::coordinateInterpolator()
{
    Q_D(QGeoMapData);
    return d->coordinateInterpolator();
}

void QGeoMapData::setCoordinateInterpolator(QSharedPointer<QGeoCoordinateInterpolator> interpolator)
{
    Q_D(QGeoMapData);
    return d->setCoordinateInterpolator(interpolator);
}

QGeoCameraCapabilities QGeoMapData::cameraCapabilities()
{
    Q_D(QGeoMapData);
    if (d->engine())
        return d->engine()->cameraCapabilities();
    else
        return QGeoCameraCapabilities();
}

QGeoMappingManagerEngine *QGeoMapData::engine()
{
    Q_D(QGeoMapData);
    return d->engine();
}

QGeoMapDataPrivate::QGeoMapDataPrivate(QGeoMappingManagerEngine *engine, QGeoMapData *parent)
    : width_(0),
      height_(0),
      aspectRatio_(0.0),
      map_(parent),
      engine_(engine),
      controller_(0),
      camera_(new QGLCamera()),
      activeMapType_(QGeoMapType())
{
    pluginString_ = engine_->managerName() + QLatin1String("_") + QString::number(engine_->managerVersion());
}

QGeoMapDataPrivate::~QGeoMapDataPrivate()
{
    // controller_ is a child of map_, don't need to delete it here

    delete camera_;
    // TODO map items are not deallocated!
    // However: how to ensure this is done in rendering thread?
}

QGeoMappingManagerEngine *QGeoMapDataPrivate::engine() const
{
    return engine_;
}

QString QGeoMapDataPrivate::pluginString()
{
    return pluginString_;
}

QSharedPointer<QGeoCoordinateInterpolator> QGeoMapDataPrivate::coordinateInterpolator() const
{
    return coordinateInterpolator_;
}

void QGeoMapDataPrivate::setCoordinateInterpolator(QSharedPointer<QGeoCoordinateInterpolator> interpolator)
{
    coordinateInterpolator_ = interpolator;
}

QGeoMapController* QGeoMapDataPrivate::mapController()
{
    if (!controller_)
        controller_ = new QGeoMapController(map_, coordinateInterpolator_);
    return controller_;
}

QGLCamera* QGeoMapDataPrivate::glCamera() const
{
    return camera_;
}

void QGeoMapDataPrivate::setCameraData(const QGeoCameraData &cameraData)
{
    QGeoCameraData oldCameraData = cameraData_;
    cameraData_ = cameraData;

    if (engine_) {
        QGeoCameraCapabilities capabilities = engine_->cameraCapabilities();
        if (cameraData_.zoomLevel() < capabilities.minimumZoomLevel())
            cameraData_.setZoomLevel(capabilities.minimumZoomLevel());

        if (cameraData_.zoomLevel() > capabilities.maximumZoomLevel())
            cameraData_.setZoomLevel(capabilities.maximumZoomLevel());

        if (!capabilities.supportsBearing())
            cameraData_.setBearing(0.0);

        if (capabilities.supportsTilting()) {
            if (cameraData_.tilt() < capabilities.minimumTilt())
                cameraData_.setTilt(capabilities.minimumTilt());

            if (cameraData_.tilt() > capabilities.maximumTilt())
                cameraData_.setTilt(capabilities.maximumTilt());
        } else {
            cameraData_.setTilt(0.0);
        }

        if (!capabilities.supportsRolling())
            cameraData_.setRoll(0.0);
    }

    cameraData_.setCoordinateInterpolator(coordinateInterpolator_.toWeakRef());
    map_->changeCameraData(oldCameraData);
}

QGeoCameraData QGeoMapDataPrivate::cameraData() const
{
    return cameraData_;
}

void QGeoMapDataPrivate::resize(int width, int height)
{
    width_ = width;
    height_ = height;
    aspectRatio_ = 1.0 * width_ / height_;
    map_->mapResized(width, height);
    setCameraData(cameraData_);
}

int QGeoMapDataPrivate::width() const
{
    return width_;
}

int QGeoMapDataPrivate::height() const
{
    return height_;
}

double QGeoMapDataPrivate::aspectRatio() const
{
    return aspectRatio_;
}

void QGeoMapDataPrivate::setActiveMapType(const QGeoMapType &type)
{
    activeMapType_ = type;

    map_->changeActiveMapType(type);
    setCameraData(cameraData_);

    map_->update();
}

const QGeoMapType QGeoMapDataPrivate::activeMapType() const
{
  return activeMapType_;
}

QT_END_NAMESPACE