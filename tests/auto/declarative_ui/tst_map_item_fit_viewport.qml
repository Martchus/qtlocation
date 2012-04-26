/****************************************************************************
**
** Copyright (C) 2012 Nokia Corporation and/or its subsidiary(-ies).
** Contact: http://www.qt-project.org/
**
** This file is part of the test suite of the Qt Toolkit.
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

import QtQuick 2.0
import QtTest 1.0
import QtLocation 5.0
import QtLocation.test 5.0

    /*

     (0,0)   ---------------------------------------------------- (240,0)
             | no map                                           |
             |    (20,20)                                       |
     (0,20)  |    ------------------------------------------    | (240,20)
             |    |                                        |    |
             |    |  map                                   |    |
             |    |                                        |    |
             |    |                                        |    |
             |    |                                        |    |
             |    |                   (lat 20, lon 20)     |    |
             |    |                     x                  |    |
             |    |                                        |    |
             |    |                                        |    |
             |    |                                        |    |
             |    |                                        |    |
             |    |                                        |    |
             |    ------------------------------------------    |
             |                                                  |
     (0,240) ---------------------------------------------------- (240,240)

     */

Item {
    id: page
    x: 0; y: 0;
    width: 240
    height: 240
    Plugin { id: testPlugin; name : "qmlgeo.test.plugin"; allowExperimental: true }
    Coordinate{ id: mapDefaultCenter; latitude: 20; longitude: 20}
    Coordinate{ id: preMapRectangleDefaultTopLeft; latitude: 20; longitude: 20}
    Coordinate{ id: preMapRectangleDefaultBottomRight; latitude: 10; longitude: 30}
    Coordinate{ id: preMapCircleDefaultCenter; latitude: 10; longitude: 30}
    Coordinate{ id: preMapQuickItemDefaultCoordinate; latitude: 35; longitude: 3}


    property list<Coordinate> preMapPolygonDefaultPath:[
        Coordinate { latitude: 25; longitude: 5},
        Coordinate { latitude: 20; longitude: 10},
        Coordinate { latitude: 15; longitude: 6}
    ]

    property list<Coordinate> preMapPolylineDefaultPath:[
        Coordinate { latitude: 25; longitude: 15},
        Coordinate { latitude: 20; longitude: 19},
        Coordinate { latitude: 15; longitude: 16}
    ]

    property list<Coordinate> preMapRouteDefaultPath:[
        Coordinate { latitude: 25; longitude: 14},
        Coordinate { latitude: 20; longitude: 18},
        Coordinate { latitude: 15; longitude: 15}
    ]

    Coordinate{ id: mapCircleTopLeft; latitude: 0; longitude: 0}
    Coordinate{ id: mapCircleBottomRight; latitude: 0; longitude: 0}
    Coordinate{ id: mapQuickItemTopLeft; latitude: 0; longitude: 0}
    Coordinate{ id: mapQuickItemBottomRight; latitude: 0; longitude: 0}
    Coordinate{ id: mapPolygonTopLeft; latitude: 0; longitude: 0}
    Coordinate{ id: mapPolygonBottomRight; latitude: 0; longitude: 0}
    Coordinate{ id: mapPolylineTopLeft; latitude: 0; longitude: 0}
    Coordinate{ id: mapPolylineBottomRight; latitude: 0; longitude: 0}
    Coordinate{ id: mapRouteTopLeft; latitude: 0; longitude: 0}
    Coordinate{ id: mapRouteBottomRight; latitude: 0; longitude: 0}

    BoundingBox {
        id: boundingBox
        topLeft: Coordinate {
            latitude: 0
            longitude: 0
        }
        bottomRight: Coordinate {
            latitude: 0
            longitude: 0
        }
    }

    Map {
        id: map;
        x: 20; y: 20; width: 200; height: 200
        zoomLevel: 3
        center: mapDefaultCenter
        plugin: testPlugin;

        MapRectangle {
            id: preMapRect
            color: 'darkcyan'
            border.width: 0
            topLeft: preMapRectangleDefaultTopLeft
            bottomRight: preMapRectangleDefaultBottomRight
            MapMouseArea {
                id: preMapRectMa
                anchors.fill: parent
                drag.target: parent
                SignalSpy { id: preMapRectClicked; target: parent; signalName: "clicked" }
                SignalSpy { id: preMapRectActiveChanged; target: parent.drag; signalName: "activeChanged" }
            }
            SignalSpy {id: preMapRectTopLeftChanged; target: parent; signalName: "topLeftChanged" }
            SignalSpy {id: preMapRectBottomRightChanged; target: parent; signalName: "bottomRightChanged" }
            SignalSpy {id: preMapRectColorChanged; target: parent; signalName: "colorChanged"}
        }
        MapCircle {
            id: preMapCircle
            color: 'darkmagenta'
            border.width: 0
            center: preMapCircleDefaultCenter
            radius: 400000
            MapMouseArea {
                id: preMapCircleMa
                anchors.fill: parent
                drag.target: parent
                SignalSpy { id: preMapCircleClicked; target: parent; signalName: "clicked" }
                SignalSpy { id: preMapCircleActiveChanged; target: parent.drag; signalName: "activeChanged" }
            }
            SignalSpy {id: preMapCircleCenterChanged; target: parent; signalName: "centerChanged"}
            SignalSpy {id: preMapCircleColorChanged; target: parent; signalName: "colorChanged"}
            SignalSpy {id: preMapCircleRadiusChanged; target: parent; signalName: "radiusChanged"}
            SignalSpy {id: preMapCircleBorderColorChanged; target: parent.border; signalName: "colorChanged"}
            SignalSpy {id: preMapCircleBorderWidthChanged; target: parent.border; signalName: "widthChanged"}
        }
        MapQuickItem {
            id: preMapQuickItem
            MapMouseArea {
                anchors.fill: parent
                drag.target: parent
                SignalSpy { id: preMapQuickItemClicked; target: parent; signalName: "clicked" }
                SignalSpy { id: preMapQuickItemActiveChanged; target: parent.drag; signalName: "activeChanged" }
            }
            coordinate: preMapQuickItemDefaultCoordinate
            sourceItem: Rectangle {
                color: 'darkgreen'
                width: 20
                height: 20
            }
            SignalSpy { id: preMapQuickItemCoordinateChanged; target: parent; signalName: "coordinateChanged"}
            SignalSpy { id: preMapQuickItemAnchorPointChanged; target: parent; signalName: "anchorPointChanged"}
            SignalSpy { id: preMapQuickItemZoomLevelChanged; target: parent; signalName: "zoomLevelChanged"}
            SignalSpy { id: preMapQuickItemSourceItemChanged; target: parent; signalName: "sourceItemChanged"}
        }
        MapPolygon {
            id: preMapPolygon
            color: 'darkgrey'
            border.width: 0
            path: [
                Coordinate { latitude: 25; longitude: 5},
                Coordinate { latitude: 20; longitude: 10},
                Coordinate { latitude: 15; longitude: 6}
            ]
            MapMouseArea {
                anchors.fill: parent
                drag.target: parent
                SignalSpy { id: preMapPolygonClicked; target: parent; signalName: "clicked" }
            }
            SignalSpy {id: preMapPolygonPathChanged; target: parent; signalName: "pathChanged"}
            SignalSpy {id: preMapPolygonColorChanged; target: parent; signalName: "colorChanged"}
            SignalSpy {id: preMapPolygonBorderWidthChanged; target: parent.border; signalName: "widthChanged"}
            SignalSpy {id: preMapPolygonBorderColorChanged; target: parent.border; signalName: "colorChanged"}
        }
        MapPolyline {
            id: preMapPolyline
            line.color: 'darkred'
            path: [
                Coordinate { latitude: 25; longitude: 15},
                Coordinate { latitude: 20; longitude: 19},
                Coordinate { latitude: 15; longitude: 16}
            ]
            SignalSpy {id: preMapPolylineColorChanged; target: parent.line; signalName: "colorChanged"}
            SignalSpy {id: preMapPolylineWidthChanged; target: parent.line; signalName: "widthChanged"}
            SignalSpy {id: preMapPolylinePathChanged; target: parent; signalName: "pathChanged"}
        }
        MapRoute {
            id: preMapRoute
            line.color: 'yellow'
            // don't try this at home - route is not user instantiable
            route: Route {
                path: [
                    Coordinate { latitude: 25; longitude: 14},
                    Coordinate { latitude: 20; longitude: 18},
                    Coordinate { latitude: 15; longitude: 15}
                ]
            }
            SignalSpy {id: preMapRouteRouteChanged; target: parent; signalName: "routeChanged"}
            SignalSpy {id: preMapRouteLineWidthChanged; target: parent.line; signalName: "widthChanged"}
            SignalSpy {id: preMapRouteLineColorChanged; target: parent.line; signalName: "colorChanged"}
        }
    }

    TestCase {
        name: "Map Items Fit Viewport"
        when: windowShown

        function test_aa_visible_basic() { // aa et al. for execution order
            wait(10)
            // sanity check that the coordinate conversion works, as
            // rest of the case relies on it. for robustness cut
            // a little slack with fuzzy compare
            var mapcenter = map.toScreenPosition(map.center)
            verify (fuzzy_compare(mapcenter.x, 100, 2))
            verify (fuzzy_compare(mapcenter.y, 100, 2))

            reset()
            // normal case - fit viewport to items which are all already visible
            verify_visibility_all_items()
            map.fitViewportToMapItems()
            visualInspectionPoint()
            verify_visibility_all_items()
        }

        function test_ab_visible_zoom() {
            var i
            // zoom in (clipping also occurs)
            var z = map.zoomLevel
            for (i = (z + 1); i < map.maximumZoomLevel; ++i ) {
                reset()
                map.zoomLevel = i
                visualInspectionPoint()
                map.fitViewportToMapItems()
                visualInspectionPoint()
                verify_visibility_all_items()
            }
            // zoom out
            for (i = (z - 1); i >= 0; --i ) {
                reset()
                map.zoomLevel = i
                visualInspectionPoint()
                verify_visibility_all_items()
                map.fitViewportToMapItems()
                visualInspectionPoint()
                verify_visibility_all_items()
            }
        }

        function test_ac_visible_map_move() {
            // move map so all items are out of screen
            // then fit viewport
            var xDir = 1
            var yDir = 0
            var xDirChange = -1
            var yDirChange = 1
            var dir = 0
            for (dir = 0; dir < 4; dir++) {
                reset()
                verify_visibility_all_items()
                var i = 0
                var panX = map.width * xDir * 0.5
                var panY = map.height * yDir * 0.5
                map.pan(panX, panY)
                map.pan(panX, panY)
                visualInspectionPoint()
                // check all items are indeed not within screen bounds
                calculate_bounds()
                verify(!is_coord_on_screen(preMapRect.topLeft))
                verify(!is_coord_on_screen(preMapRect.bottomRight))
                verify(!is_coord_on_screen(mapCircleTopLeft))
                verify(!is_coord_on_screen(mapCircleBottomRight))
                verify(!is_coord_on_screen(mapPolygonTopLeft))
                verify(!is_coord_on_screen(mapPolygonBottomRight))
                verify(!is_coord_on_screen(mapQuickItemTopLeft))
                verify(!is_coord_on_screen(mapQuickItemBottomRight))
                verify(!is_coord_on_screen(mapPolylineTopLeft))
                verify(!is_coord_on_screen(mapPolylineBottomRight))
                verify(!is_coord_on_screen(mapRouteTopLeft))
                verify(!is_coord_on_screen(mapRouteBottomRight))
                // fit viewport and verify that all items are visible again
                map.fitViewportToMapItems()
                visualInspectionPoint()
                verify_visibility_all_items()
                if (dir == 2)
                    xDirChange *= -1
                if (dir == 1)
                    yDirChange *= -1
                xDir += xDirChange
                yDir += yDirChange
            }
        }

        function test_ad_visible_items_move() {
            // move different individual items out of screen
            // then fit viewport
            var xDir = 1
            var yDir = 0
            var xDirChange = -1
            var yDirChange = 1
            var dir = 0
            var move = 50
            for (dir = 0; dir < 4; dir++) {
                // move rect out of screen
                reset()
                verify_visibility_all_items()
                preMapRect.topLeft.longitude += move * xDir
                preMapRect.topLeft.latitude += move * yDir
                preMapRect.bottomRight.longitude += move * xDir
                preMapRect.bottomRight.latitude += move * yDir
                calculate_bounds()
                verify(!is_coord_on_screen(preMapRect.topLeft))
                verify(!is_coord_on_screen(preMapRect.bottomRight))
                map.fitViewportToMapItems()
                visualInspectionPoint()
                verify_visibility_all_items()

                // move circle out of screen
                reset()
                verify_visibility_all_items()
                preMapCircle.center.longitude += move * xDir
                preMapCircle.center.latitude += move * yDir
                calculate_bounds()
                verify(!is_coord_on_screen(mapCircleTopLeft))
                verify(!is_coord_on_screen(mapCircleBottomRight))
                map.fitViewportToMapItems()
                visualInspectionPoint()
                verify_visibility_all_items()

                // move quick item out of screen
                reset()
                verify_visibility_all_items()
                preMapQuickItem.coordinate.longitude += move * xDir
                preMapQuickItem.coordinate.latitude += move * yDir
                calculate_bounds()
                verify(!is_coord_on_screen(mapQuickItemTopLeft))
                verify(!is_coord_on_screen(mapQuickItemBottomRight))
                map.fitViewportToMapItems()
                visualInspectionPoint()
                verify_visibility_all_items()

                // move map polygon out of screen
                reset()
                verify_visibility_all_items()
                var i
                for (i = 0; i < preMapPolygonDefaultPath.length; ++i) {
                    preMapPolygon.path[i].longitude += move * xDir
                    preMapPolygon.path[i].latitude += move * yDir
                }
                calculate_bounds()
                verify(!is_coord_on_screen(mapPolygonTopLeft))
                verify(!is_coord_on_screen(mapPolygonBottomRight))
                map.fitViewportToMapItems()
                visualInspectionPoint()
                verify_visibility_all_items()
               if (dir == 2)
                    xDirChange *= -1
                if (dir == 1)
                    yDirChange *= -1
                xDir += xDirChange
                yDir += yDirChange
            }
        }

        function clear_data() {
            preMapRectClicked.clear()
            preMapCircleClicked.clear()
            preMapQuickItemClicked.clear()
            preMapPolygonClicked.clear()
            preMapCircleCenterChanged.clear()
            preMapCircleColorChanged.clear()
            preMapCircleRadiusChanged.clear()
            preMapCircleBorderColorChanged.clear()
            preMapCircleBorderWidthChanged.clear()
            preMapRectTopLeftChanged.clear()
            preMapRectBottomRightChanged.clear()
            preMapRectColorChanged.clear()
            preMapPolylineColorChanged.clear()
            preMapPolylineWidthChanged.clear()
            preMapPolylinePathChanged.clear()
            preMapPolygonPathChanged.clear()
            preMapPolygonColorChanged.clear()
            preMapPolygonBorderColorChanged.clear()
            preMapPolygonBorderWidthChanged.clear()
            preMapRouteRouteChanged.clear()
            preMapRouteLineColorChanged.clear()
            preMapRouteLineWidthChanged.clear()
            preMapQuickItemCoordinateChanged.clear()
            preMapQuickItemAnchorPointChanged.clear()
            preMapQuickItemZoomLevelChanged.clear()
            preMapQuickItemSourceItemChanged.clear()
        }

        function calculate_bounds(){
            var circleDiagonal = Math.sqrt(2 * preMapCircle.radius * preMapCircle.radius)
            var itemTopLeft = preMapCircle.center.atDistanceAndAzimuth(circleDiagonal,-45)
            var itemBottomRight = preMapCircle.center.atDistanceAndAzimuth(circleDiagonal,135)

            mapCircleTopLeft.latitude = itemTopLeft.latitude
            mapCircleTopLeft.longitude = itemTopLeft.longitude
            mapCircleBottomRight.latitude = itemBottomRight.latitude
            mapCircleBottomRight.longitude = itemBottomRight.longitude

            itemTopLeft = preMapQuickItem.coordinate
            var preMapQuickItemScreenPosition = map.toScreenPosition(preMapQuickItem.coordinate)
            preMapQuickItemScreenPosition.x += preMapQuickItem.sourceItem.width
            preMapQuickItemScreenPosition.y += preMapQuickItem.sourceItem.height
            itemBottomRight = map.toCoordinate(preMapQuickItemScreenPosition)

            mapQuickItemTopLeft.latitude = itemTopLeft.latitude
            mapQuickItemTopLeft.longitude = itemTopLeft.longitude
            mapQuickItemBottomRight.latitude = itemBottomRight.latitude
            mapQuickItemBottomRight.longitude = itemBottomRight.longitude

             var bounds = min_max_bounds_from_list(preMapPolygon.path)
             mapPolygonTopLeft.latitude = bounds.topLeft.latitude
             mapPolygonTopLeft.longitude = bounds.topLeft.longitude
             mapPolygonBottomRight.latitude = bounds.bottomRight.latitude
             mapPolygonBottomRight.longitude = bounds.bottomRight.longitude

             bounds = min_max_bounds_from_list(preMapPolyline.path)
             mapPolylineTopLeft.latitude = bounds.topLeft.latitude
             mapPolylineTopLeft.longitude = bounds.topLeft.longitude
             mapPolylineBottomRight.latitude = bounds.bottomRight.latitude
             mapPolylineBottomRight.longitude = bounds.bottomRight.longitude

             bounds = min_max_bounds_from_list(preMapRoute.route.path)
             mapRouteTopLeft.latitude = bounds.topLeft.latitude
             mapRouteTopLeft.longitude = bounds.topLeft.longitude
             mapRouteBottomRight.latitude = bounds.bottomRight.latitude
             mapRouteBottomRight.longitude = bounds.bottomRight.longitude
        }

        function min_max_bounds_from_list(coorindates){
            var i = 0
            var point = map.toScreenPosition(coorindates[0])
            var minX = point.x
            var minY = point.y
            var maxX = point.x
            var maxY = point.y

            for (i=1; i < coorindates.length; ++i) {
                point = map.toScreenPosition(coorindates[i])
                if (point.x < minX)
                    minX = point.x
                if (point.x > maxX)
                    maxX = point.x
                if (point.y < minY)
                    minY = point.y
                if (point.y > maxY)
                    maxY = point.y
            }
            point.x = minX
            point.y = minY
            var itemTopLeft = map.toCoordinate(point)
            point.x = maxX
            point.y = maxY
            var itemBottomRight = map.toCoordinate(point)

            boundingBox.topLeft.latitude = itemTopLeft.latitude
            boundingBox.topLeft.longitude = itemTopLeft.longitude
            boundingBox.bottomRight.latitude = itemBottomRight.latitude
            boundingBox.bottomRight.longitude = itemBottomRight.longitude
            return boundingBox
        }

        function verify_visibility_all_items(){
            calculate_bounds()
            verify(is_coord_on_screen(preMapRect.topLeft))
            verify(is_coord_on_screen(preMapRect.bottomRight))
            verify(is_coord_on_screen(mapCircleTopLeft))
            verify(is_coord_on_screen(mapCircleBottomRight))
            verify(is_coord_on_screen(mapPolygonTopLeft))
            verify(is_coord_on_screen(mapPolygonBottomRight))
            verify(is_coord_on_screen(mapQuickItemTopLeft))
            verify(is_coord_on_screen(mapQuickItemBottomRight))
            verify(is_coord_on_screen(mapPolylineTopLeft))
            verify(is_coord_on_screen(mapPolylineBottomRight))
            verify(is_coord_on_screen(mapRouteTopLeft))
            verify(is_coord_on_screen(mapRouteBottomRight))
        }

        function reset(){
            preMapRect.topLeft.latitude = 20
            preMapRect.topLeft.longitude = 20
            preMapRect.bottomRight.latitude = 10
            preMapRect.bottomRight.longitude = 30
            preMapCircle.center.latitude = 10
            preMapCircle.center.longitude = 30
            preMapQuickItem.coordinate.latitude = 35
            preMapQuickItem.coordinate.longitude = 3
            var i
            for (i=0; i< preMapPolygon.path.length; ++i) {
                preMapPolygon.path[i].latitude = preMapPolygonDefaultPath[i].latitude
                preMapPolygon.path[i].longitude = preMapPolygonDefaultPath[i].longitude
            }
            for (i=0; i< preMapPolyline.path.length; ++i) {
                preMapPolyline.path[i].latitude = preMapPolylineDefaultPath[i].latitude
                preMapPolyline.path[i].longitude = preMapPolylineDefaultPath[i].longitude
            }
            for (i=0; i< preMapRoute.route.path.length; ++i) {
                preMapRoute.route.path[i].latitude = preMapRouteDefaultPath[i].latitude
                preMapRoute.route.path[i].longitude = preMapRouteDefaultPath[i].longitude
            }
            // remove items
            map.clearMapItems()
            clear_data()
            compare (map.mapItems.length, 0)
            // reset map
            map.center.latitude = 20
            map.center.longitude = 20
            map.zoomLevel = 3
            // re-add items and verify they are back (without needing to pan map etc.)
            map.addMapItem(preMapRect)
            map.addMapItem(preMapCircle)
            map.addMapItem(preMapQuickItem)
            map.addMapItem(preMapPolygon)
            map.addMapItem(preMapPolyline)
            map.addMapItem(preMapRoute)
            compare (map.mapItems.length, 6)
            calculate_bounds()
        }

        function is_coord_on_screen(coord) {
            return is_point_on_screen(map.toScreenPosition(coord))
        }

        function is_point_on_screen(point) {
            if (point.x >= 0 && point.x <= (map.x + map.width)
                    && point.y >=0 && point.y <= (map.y + map.height) )
                return true;
            else
                return false;
        }

        function fuzzy_compare(val, ref, tol) {
            var tolerance = 2
            if (tol !== undefined)
                tolerance = tol
            if ((val >= ref - tolerance) && (val <= ref + tolerance))
                return true;
            console.log('map fuzzy cmp returns false for value, ref, tolerance: ' + val + ', ' + ref + ', ' + tolerance)
            return false;
        }

        // call to visualInspectionPoint testcase (for dev time visual inspection)
        function visualInspectionPoint(time) {
            var waitTime = 0 // 300
            if (time !== undefined)
                waitTime = time
            if (waitTime > 0) {
                console.log('halting for ' + waitTime + ' milliseconds')
                wait (waitTime)
            }
        }
    }
}
