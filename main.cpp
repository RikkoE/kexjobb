#include <QApplication>
#include <QGuiApplication>
#include <QQmlApplicationEngine>
#include <QQmlContext>
#include <QQuickView>
#include <QQuickWindow>
#include <QStringListModel>
#include <QStandardItemModel>
#include "healthywayfunctions.h"
#include <jni.h>
#include <QObject>

int main(int argc, char *argv[])
{
    QGuiApplication app(argc, argv);

    HealthyWayFunctions *generator = new HealthyWayFunctions();



    QQuickView *view = new QQuickView;

    view->rootContext()->setContextProperty("generator", generator);
    view->setSource(QUrl("qrc:/main.qml"));
    view->setResizeMode(QQuickView::SizeRootObjectToView);
    view->show();

    return app.exec();
}

