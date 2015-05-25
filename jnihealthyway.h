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
    bool scanningStatus();

    QStringList listServices();
    QStringList listDevices();

    int getBatteryLevel();
    QString getManufacturerName();
    int *getEcgData();

private:
    // An array that contains the ECG timestamp and reading samples.
    int ecgData[7];

};

#endif // JNIHEALTHYWAY_H
