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
#include "qgeomapgeometry_p.h"

#include "qgeocameradata_p.h"
#include "qgeoprojection_p.h"
#include "qgeotilespec.h"

#include "qdoublevector2d_p.h"
#include "qdoublevector3d_p.h"

#include <Qt3D/qglscenenode.h>
#include <Qt3D/qglbuilder.h>
#include <Qt3D/qglmaterial.h>
#include <Qt3D/qgltexture2d.h>
#include <Qt3D/qgeometrydata.h>
#include <Qt3D/qglcamera.h>
#include <Qt3D/qglpainter.h>

#include <QHash>

#include <QPointF>

#include <cmath>

QT_BEGIN_NAMESPACE

class QGeoCoordinateInterpolator2D : public QGeoCoordinateInterpolator
{
public:
    QGeoCoordinateInterpolator2D();
    virtual ~QGeoCoordinateInterpolator2D();

    virtual QGeoCoordinate interpolate(const QGeoCoordinate &start, const QGeoCoordinate &end, qreal progress);
};

QGeoCoordinateInterpolator2D::QGeoCoordinateInterpolator2D() {}

QGeoCoordinateInterpolator2D::~QGeoCoordinateInterpolator2D() {}

QGeoCoordinate QGeoCoordinateInterpolator2D::interpolate(const QGeoCoordinate &start, const QGeoCoordinate &end, qreal progress)
{
    if (start == end) {
        if (progress < 0.5) {
            return start;
        } else {
            return end;
        }
    }

    QGeoCoordinate s2 = start;
    QGeoCoordinate e2 = end;
    QDoubleVector2D s = QGeoProjection::coordToMercator(s2);
    QDoubleVector2D e = QGeoProjection::coordToMercator(e2);

    double x = s.x();

    if (0.5 < qAbs(e.x() - s.x())) {
        // handle dateline crossing
        double ex = e.x();
        double sx = s.x();
        if (ex < sx)
            sx -= 1.0;
        else if (sx < ex)
            ex -= 1.0;

        x = (1.0 - progress) * sx + progress * ex;

        if (!qFuzzyIsNull(x) && (x < 0.0))
            x += 1.0;

    } else {
        x = (1.0 - progress) * s.x() + progress * e.x();
    }

    double y = (1.0 - progress) * s.y() + progress * e.y();

    QGeoCoordinate result = QGeoProjection::mercatorToCoord(QDoubleVector2D(x, y));
    result.setAltitude((1.0 - progress) * start.altitude() + progress * end.altitude());
    return result;
}

class QGeoMapGeometryPrivate {
public:
    QGeoMapGeometryPrivate();
    ~QGeoMapGeometryPrivate();

    QSize screenSize_;
    int tileSize_;
    QGeoCameraData cameraData_;
    QSet<QGeoTileSpec> visibleTiles_;

    QGLCamera *camera_;
    QGLSceneNode *sceneNode_;

    double scaleFactor_;

    QHash<QGeoTileSpec, QGLSceneNode*> nodes_;

    int minTileX_;
    int minTileY_;
    int maxTileX_;
    int maxTileY_;
    int tileXWrapsBelow_;

    double mercatorCenterX_;
    double mercatorCenterY_;
    double mercatorWidth_;
    double mercatorHeight_;

    double screenOffsetX_;
    double screenOffsetY_;
    double screenWidth_;
    double screenHeight_;

    bool useVerticalLock_;
    bool verticalLock_;

    QSharedPointer<QGeoCoordinateInterpolator> coordinateInterpolator_;

    void addTile(const QGeoTileSpec &spec, QGLTexture2D *texture);

    QDoubleVector2D screenPositionToMercator(const QPointF &pos) const;
    QPointF mercatorToScreenPosition(const QDoubleVector2D &mercator) const;

    void setVisibleTiles(const QSet<QGeoTileSpec> &tiles);
    void removeOldTiles(const QSet<QGeoTileSpec> &oldTiles);
    void setTileBounds();
    void setupCamera();

    void paintGL(QGLPainter *painter);
};


QGeoMapGeometry::QGeoMapGeometry()
    : d_ptr(new QGeoMapGeometryPrivate()) {}

QGeoMapGeometry::~QGeoMapGeometry()
{
    delete d_ptr;
}

void QGeoMapGeometry::setUseVerticalLock(bool lock)
{
    Q_D(QGeoMapGeometry);
    d->useVerticalLock_ = lock;
}

void QGeoMapGeometry::setScreenSize(const QSize &size)
{
    Q_D(QGeoMapGeometry);
    d->screenSize_ = size;
}

void QGeoMapGeometry::setTileSize(int tileSize)
{
    Q_D(QGeoMapGeometry);
    d->tileSize_ = tileSize;
}

void QGeoMapGeometry::setCameraData(const QGeoCameraData &cameraData)
{
    Q_D(QGeoMapGeometry);
    d->cameraData_ = cameraData;
}

void QGeoMapGeometry::setVisibleTiles(const QSet<QGeoTileSpec> &tiles)
{
    Q_D(QGeoMapGeometry);
    d->setVisibleTiles(tiles);
}

void QGeoMapGeometry::addTile(const QGeoTileSpec &spec, QGLTexture2D *texture)
{
    Q_D(QGeoMapGeometry);
    d->addTile(spec, texture);
}

QDoubleVector2D QGeoMapGeometry::screenPositionToMercator(const QPointF &pos) const
{
    Q_D(const QGeoMapGeometry);
    return d->screenPositionToMercator(pos);
}

QPointF QGeoMapGeometry::mercatorToScreenPosition(const QDoubleVector2D &mercator) const
{
    Q_D(const QGeoMapGeometry);
    return d->mercatorToScreenPosition(mercator);
}

QGLCamera* QGeoMapGeometry::camera() const
{
    Q_D(const QGeoMapGeometry);
    return d->camera_;
}

QGLSceneNode* QGeoMapGeometry::sceneNode() const
{
    Q_D(const QGeoMapGeometry);
    return d->sceneNode_;
}

bool QGeoMapGeometry::verticalLock() const
{
    Q_D(const QGeoMapGeometry);
    return d->verticalLock_;
}

void QGeoMapGeometry::paintGL(QGLPainter *painter)
{
    Q_D(QGeoMapGeometry);
    d->paintGL(painter);
}

QSharedPointer<QGeoCoordinateInterpolator> QGeoMapGeometry::coordinateInterpolator() const
{
    Q_D(const QGeoMapGeometry);
    return d->coordinateInterpolator_;
}

QGeoMapGeometryPrivate::QGeoMapGeometryPrivate()
    : tileSize_(0),
      camera_(new QGLCamera()),
      sceneNode_(new QGLSceneNode()),
      scaleFactor_(10.0),
      minTileX_(-1),
      minTileY_(-1),
      maxTileX_(-1),
      maxTileY_(-1),
      tileXWrapsBelow_(0),
      screenOffsetX_(0.0),
      screenOffsetY_(0.0),
      useVerticalLock_(false),
      verticalLock_(false),
      coordinateInterpolator_(QSharedPointer<QGeoCoordinateInterpolator>(new QGeoCoordinateInterpolator2D())) {}

QGeoMapGeometryPrivate::~QGeoMapGeometryPrivate()
{
    delete sceneNode_;
    delete camera_;
}

QDoubleVector2D QGeoMapGeometryPrivate::screenPositionToMercator(const QPointF &pos) const
{
    int zpow2 = 1 << cameraData_.zoomLevel();

    double x = mercatorWidth_ * (((pos.x() - screenOffsetX_) / screenWidth_) - 0.5);
    x += mercatorCenterX_;

    if (x > 1.0 * zpow2)
        x -= 1.0 * zpow2;
    if (x < 0.0)
        x += 1.0 * zpow2;

    x /= 1.0 * zpow2;

    double y = mercatorHeight_ * (((pos.y() - screenOffsetY_) / screenHeight_) - 0.5);
    y += mercatorCenterY_;
    y /= 1.0 * zpow2;

    return QDoubleVector2D(x, y);
}

QPointF QGeoMapGeometryPrivate::mercatorToScreenPosition(const QDoubleVector2D &mercator) const
{
    int zpow2 = 1 << cameraData_.zoomLevel();

    double mx = zpow2 * mercator.x();

    double lb = mercatorCenterX_ - mercatorWidth_ / 2.0;
    if (lb < 0.0)
        lb += zpow2;
    double ub = mercatorCenterX_ + mercatorWidth_ / 2.0;
    if (zpow2 < ub)
        ub -= zpow2;

    double m = (mx - mercatorCenterX_) / mercatorWidth_;

    // correct for crossing dateline
    if (qFuzzyCompare(ub - lb + 1.0, 1.0) || (ub < lb) ) {
        if (mercatorCenterX_ < ub) {
            if (lb < mx) {
                m = (mx - mercatorCenterX_ - zpow2) / mercatorWidth_;
            }
        } else if (lb < mercatorCenterX_) {
            if (mx < ub) {
                m = (mx - mercatorCenterX_ + zpow2) / mercatorWidth_;
            }
        }
    }

    double x = screenWidth_ * (0.5 + m);
    double y = screenHeight_ * (0.5 + (zpow2 * mercator.y() - mercatorCenterY_) / mercatorHeight_);

    return QPointF(x + screenOffsetX_, y + screenOffsetY_);
}

void QGeoMapGeometryPrivate::addTile(const QGeoTileSpec &spec, QGLTexture2D *texture)
{
    int zpow2 = 1 << cameraData_.zoomLevel();
    int x = spec.x();

    if (x < tileXWrapsBelow_)
        x += zpow2;

    if ((x < minTileX_)
            || (maxTileX_ < x)
            || (spec.y() < minTileY_)
            || (maxTileY_ < spec.y())) {
        return;
    }

    QGLSceneNode *node = nodes_.value(spec, 0);
    if (!node) {

        double edge = scaleFactor_ * tileSize_;

        QGLBuilder builder;

        double x1 = (x - minTileX_);
        double x2 = x1 + 1.0;

        double y1 = (minTileY_ - spec.y());
        double y2 = y1 - 1.0;

        x1 *= edge;
        x2 *= edge;
        y1 *= edge;
        y2 *= edge;

        QGeometryData g;

        QDoubleVector3D n = QDoubleVector3D(0, 0, 1);

        g.appendVertex(QVector3D(x1, y1, 0.0));
        g.appendNormal(n);
        g.appendTexCoord(QVector2D(0.0, 1.0));

        g.appendVertex(QVector3D(x1, y2, 0.0));
        g.appendNormal(n);
        g.appendTexCoord(QVector2D(0.0, 0.0));

        g.appendVertex(QVector3D(x2, y2, 0.0));
        g.appendNormal(n);
        g.appendTexCoord(QVector2D(1.0, 0.0));

        g.appendVertex(QVector3D(x2, y1, 0.0));
        g.appendNormal(n);
        g.appendTexCoord(QVector2D(1.0, 1.0));

        builder.addQuads(g);

        node = builder.finalizedSceneNode();

        QGLMaterial *mat = new QGLMaterial(node);
        mat->setTexture(texture);
        node->setEffect(QGL::LitDecalTexture2D);
        node->setMaterial(mat);

        sceneNode_->addNode(node);
        nodes_.insert(spec, node);
    } else {
        // TODO handle texture updates when we make node removal more efficient
        node->material()->setTexture(texture);
    }
}

void QGeoMapGeometryPrivate::setVisibleTiles(const QSet<QGeoTileSpec> &tiles)
{
    // TODO make this more efficient
    removeOldTiles(visibleTiles_);

    visibleTiles_ = tiles;

    // work out the tile bounds for the new geometry
    setTileBounds();

    // set up the gl camera for the new geometry
    setupCamera();
}

void QGeoMapGeometryPrivate::removeOldTiles(const QSet<QGeoTileSpec> &oldTiles)
{
    typedef QSet<QGeoTileSpec>::const_iterator iter;
    iter i = oldTiles.constBegin();
    iter end = oldTiles.constEnd();

    for (; i != end; ++i) {
        QGeoTileSpec tile = *i;
        QGLSceneNode *node = nodes_.value(tile, 0);
        if (node) {
            // TODO protect with mutex?
            sceneNode_->removeNode(node);
            nodes_.remove(tile);
            delete node;
            // TODO handle deleting of node elsewhere?
        }
    }
}

void QGeoMapGeometryPrivate::setTileBounds()
{
    if (visibleTiles_.isEmpty()) {
        minTileX_ = -1;
        minTileY_ = -1;
        maxTileX_ = -1;
        maxTileY_ = -1;
        return;
    }

    typedef QSet<QGeoTileSpec>::const_iterator iter;
    iter i = visibleTiles_.constBegin();
    iter end = visibleTiles_.constEnd();

    int zpow2 = 1 << cameraData_.zoomLevel();
    bool hasFarLeft = false;
    bool hasFarRight = false;
    bool hasMidLeft = false;
    bool hasMidRight = false;

    for (; i != end; ++i) {
        int x = (*i).x();
        if (x == 0)
            hasFarLeft = true;
        else if (x == (zpow2 - 1))
            hasFarRight = true;
        else if (x == ((zpow2 / 2) - 1)) {
            hasMidLeft = true;
        } else if (x == (zpow2 / 2)) {
            hasMidRight = true;
        }
    }

    tileXWrapsBelow_ = 0;

    if (hasFarLeft && hasFarRight) {
        if (!hasMidRight) {
            tileXWrapsBelow_ = zpow2 / 2;
        } else if (!hasMidLeft) {
            tileXWrapsBelow_ = (zpow2 / 2) - 1;
        }
    }

    i = visibleTiles_.constBegin();

    QGeoTileSpec tile = *i;

    int x = tile.x();
    if (tile.x() < tileXWrapsBelow_)
        x += zpow2;

    minTileX_ = x;
    maxTileX_ = x;
    minTileY_ = tile.y();
    maxTileY_ = tile.y();

    ++i;

    for (; i != end; ++i) {
        tile = *i;
        int x = tile.x();
        if (tile.x() < tileXWrapsBelow_)
            x += zpow2;

        minTileX_ = qMin(minTileX_, x);
        maxTileX_ = qMax(maxTileX_, x);
        minTileY_ = qMin(minTileY_, tile.y());
        maxTileY_ = qMax(maxTileY_, tile.y());
    }
}

void QGeoMapGeometryPrivate::setupCamera()
{
    int zpow2 = 1 << cameraData_.zoomLevel();

    double f = 1.0 * qMin(screenSize_.width(), screenSize_.height());

    double z = pow(2.0, cameraData_.zoomFactor() - cameraData_.zoomLevel());

    double altitude = scaleFactor_ * f / (4.0 * z);

    double aspectRatio = 1.0 * screenSize_.width() / screenSize_.height();

    double a = f / (z * tileSize_);

    if (aspectRatio > 1.0) {
        mercatorHeight_ = a;
        mercatorWidth_ = a * aspectRatio;
    } else {
        mercatorWidth_ = a;
        mercatorHeight_ = a / aspectRatio;
    }

    // calculate center

    double edge = scaleFactor_ * tileSize_;

    QDoubleVector3D center = (zpow2 * QGeoProjection::coordToMercator(cameraData_.center()));

    if (center.x() < tileXWrapsBelow_)
        center.setX(center.x() + 1.0 * zpow2);

    mercatorCenterX_ = center.x();
    mercatorCenterY_ = center.y();

    center.setX(center.x() - 1.0 * minTileX_);
    center.setY(1.0 * minTileY_ - center.y());

    // letter box vertically
    if (useVerticalLock_ && (mercatorHeight_ > 1.0 * zpow2)) {
        center.setY(-1.0 * zpow2 / 2.0);
        mercatorCenterY_ = zpow2 / 2.0;
        screenOffsetY_ = screenSize_.height() * (0.5 - zpow2 / (2 * mercatorHeight_));
        screenHeight_ = screenSize_.height() - 2 * screenOffsetY_;
        mercatorHeight_ = 1.0 * zpow2;
        verticalLock_ = true;
    } else {
        screenOffsetY_ = 0.0;
        screenHeight_ = screenSize_.height();
        verticalLock_ = false;
    }

    if (mercatorWidth_ > 1.0 * zpow2) {
        screenOffsetX_ = screenSize_.width() * (0.5 - (zpow2 / (2 * mercatorWidth_)));
        screenWidth_ = screenSize_.width() - 2 * screenOffsetX_;
        mercatorWidth_ = 1.0 * zpow2;
    } else {
        screenOffsetX_ = 0.0;
        screenWidth_ = screenSize_.width();
    }

    center *= edge;

    // calculate eye

    QDoubleVector3D eye = center;
    eye.setZ(altitude);

    // calculate up

    QDoubleVector3D view = eye - center;
    QDoubleVector3D side = QDoubleVector3D::normal(view, QDoubleVector3D(0.0, 1.0, 0.0));
    QDoubleVector3D up = QDoubleVector3D::normal(side, view);

    // old bearing, tilt and roll code
    //    QMatrix4x4 mBearing;
    //    mBearing.rotate(-1.0 * camera.bearing(), view);
    //    up = mBearing * up;

    //    QDoubleVector3D side2 = QDoubleVector3D::normal(up, view);
    //    QMatrix4x4 mTilt;
    //    mTilt.rotate(camera.tilt(), side2);
    //    eye = (mTilt * view) + center;

    //    view = eye - center;
    //    side = QDoubleVector3D::normal(view, QDoubleVector3D(0.0, 1.0, 0.0));
    //    up = QDoubleVector3D::normal(view, side2);

    //    QMatrix4x4 mRoll;
    //    mRoll.rotate(camera.roll(), view);
    //    up = mRoll * up;

    // near plane and far plane

    double nearPlane = 0.5;
    double farPlane = 2.0 * edge;

    // TODO protect with mutex?
    // set glcamera parameters
    camera_->setCenter(center);
    camera_->setEye(eye);
    camera_->setUpVector(up);
    camera_->setNearPlane(nearPlane);
    camera_->setFarPlane(farPlane);
}

void QGeoMapGeometryPrivate::paintGL(QGLPainter *painter)
{
    // TODO protect with mutex?

    // TODO add a shortcut here for when we don't need to repeat and clip the map

    glEnable(GL_SCISSOR_TEST);

    painter->setScissor(QRect(screenOffsetX_, screenOffsetY_, screenWidth_, screenHeight_));

    painter->setCamera(camera_);
    painter->projectionMatrix().scale(1, -1, 1);
    sceneNode_->draw(painter);

    QGLCamera *camera = camera_;

    bool old = camera->blockSignals(true);

    glDisable(GL_DEPTH_TEST);

    double sideLength_ = scaleFactor_ * tileSize_ * (1 << cameraData_.zoomLevel());

    QDoubleVector3D c = QDoubleVector3D(camera->center());
    c.setX(c.x() + sideLength_);
    camera->setCenter(c);

    QDoubleVector3D e = QDoubleVector3D(camera->eye());
    e.setX(e.x() + sideLength_);
    camera->setEye(e);

    painter->setCamera(camera);
    painter->projectionMatrix().scale(1, -1, 1);
    sceneNode_->draw(painter);

    c.setX(c.x() - 2 * sideLength_);
    camera->setCenter(c);
    e.setX(e.x() - 2 * sideLength_);
    camera->setEye(e);

    painter->setCamera(camera);
    painter->projectionMatrix().scale(1, -1, 1);
    sceneNode_->draw(painter);

    c.setX(c.x() + sideLength_);
    camera->setCenter(c);
    e.setX(e.x() + sideLength_);
    camera->setEye(e);

    painter->setCamera(camera);
    painter->projectionMatrix().scale(1, -1, 1);
    sceneNode_->draw(painter);

    glEnable(GL_DEPTH_TEST);

    camera->blockSignals(old);
}

QT_END_NAMESPACE