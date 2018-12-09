TEMPLATE = subdirs

QT_FOR_CONFIG += location-private  # pulls in the features defined in configure.json

qtConfig(geoservices_here): SUBDIRS += nokia
qtConfig(geoservices_mapbox): SUBDIRS += mapbox
qtConfig(geoservices_esri): SUBDIRS += esri
qtConfig(geoservices_itemsoverlay): SUBDIRS += itemsoverlay
qtConfig(geoservices_osm): SUBDIRS += osm
