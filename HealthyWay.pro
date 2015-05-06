TEMPLATE = app

QT += qml quick widgets androidextras

ANDROID_PACKAGE_SOURCE_DIR = $$PWD/android-sources

SOURCES += main.cpp \
    healthywayfunctions.cpp \
    serviceinformation.cpp

RESOURCES += qml.qrc

# Additional import path used to resolve QML modules in Qt Creator's code model
QML_IMPORT_PATH =

# Default rules for deployment.
include(deployment.pri)

HEADERS += \
    healthywayfunctions.h \
    serviceinformation.h

OTHER_FILES += \
    android-sources/src/org/qtproject/example/notification/NotificationClient.java \
    android-sources/AndroidManifest.xml

DISTFILES += \
    android-sources/src/org/qtproject/example/notification/MyJavaNatives.java
