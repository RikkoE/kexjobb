#include <QApplication>
#include <QQmlApplicationEngine>
#include <QQmlContext>
#include <QQuickView>
#include <QStringListModel>
#include "healthywayfunctions.h"

int main(int argc, char *argv[])
{
    QApplication app(argc, argv);

    QQmlApplicationEngine engine;
    engine.load(QUrl(QStringLiteral("qrc:/main.qml")));

    QStringList dataList;
    dataList.append("No devices");

    QStringListModel devicesModel;
    devicesModel.setStringList(dataList);

    HealthyWayFunctions generator(&devicesModel);

    engine.rootContext()->setContextProperty("devicesModel", &devicesModel);
    engine.rootContext()->setContextProperty("generator", &generator);

    return app.exec();
}
