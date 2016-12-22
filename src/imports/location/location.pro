QT += quick-private network positioning-private positioningquick-private location-private qml-private core-private gui-private

SOURCES += \
           location.cpp

load(qml_plugin)

LIBS_PRIVATE += -L$$MODULE_BASE_OUTDIR/lib -lclip2tri$$qtPlatformTargetSuffix() -lclipper$$qtPlatformTargetSuffix() -lpoly2tri$$qtPlatformTargetSuffix()
QMAKE_LIBS_PRIVATE += -lclip2tri$$qtPlatformTargetSuffix() -lclipper$$qtPlatformTargetSuffix() -lpoly2tri$$qtPlatformTargetSuffix()

OTHER_FILES += \
    plugin.json \
    qmldir
