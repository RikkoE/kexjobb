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
    void disconnectDataStream(QString characteristic);
    void turnBluetoothOff();
    void turnBluetoothOn();
    void connectService(int serviceIndex);

    bool scanningStatus();

    QStringList listServices();
    QStringList listDevices();
    QStringList getDeviceData(int serviceIndex);

    int deviceDataExperiment(int serviceIndex);

    int getBatteryLevel();
    QString getManufacturerName();
    int *getEcgData();

private:
    int ecgData[7];

};

#endif // JNIHEALTHYWAY_H
