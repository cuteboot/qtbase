TARGET = qeglfs-surfaceflinger-integration

PLUGIN_TYPE = egldeviceintegrations
PLUGIN_CLASS_NAME = QEglFSSurfaceFlingerIntegrationPlugin

load(qt_plugin)

QT += core-private gui-private platformsupport-private eglfs_device_lib-private
INCLUDEPATH += $$PWD/../..
CONFIG += egl

LIBS += -lui -lgui -lutils -lcutils
QMAKE_LFLAGS += $$QMAKE_LFLAGS_NOUNDEF

SOURCES += \
    $$PWD/qeglfssurfaceflingermain.cpp \
    $$PWD/qeglfssurfaceflingerintegration.cpp

HEADERS += \
    $$PWD/qeglfssurfaceflingerintegration.h

OTHER_FILES += $$PWD/eglfs_surfaceflinger.json

