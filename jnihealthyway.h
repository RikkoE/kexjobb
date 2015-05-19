#ifndef JNIHEALTHYWAY_H
#define JNIHEALTHYWAY_H

#include <QDebug>
#include <QObject>
#include <QtAndroidExtras/QAndroidJniObject>
#include <QAndroidJniEnvironment>

class JNIHealthyWay
{
public:
    JNIHealthyWay();
    ~JNIHealthyWay();

    void connectDevice(QString deviceName);
    void disconnectDevice();
    void scanForDevices();
    void disconnectDataStream(int deviceIndex);
    void turnBluetoothOff();
    void turnBluetoothOn();
    void connectService(int serviceIndex);

    bool scanningStatus();

    QStringList listServices();
    QStringList listDevices();
    QStringList getDeviceData(int serviceIndex);

    int deviceDataExperiment(int serviceIndex);

};

#endif // JNIHEALTHYWAY_H
