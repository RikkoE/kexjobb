#ifndef JNIHEALTHYWAY_H
#define JNIHEALTHYWAY_H

#include <QDebug>
#include <QObject>
#include <QtAndroidExtras/QAndroidJniObject>
#include <QAndroidJniEnvironment>

///
/// \brief The JNIHealthyWay class
///
/// This class contains the necessary functions to call the native Java
/// methods. The class also converts the values from native Java to regular
/// C++ data values.
///
/// @author Rickard Eriksson
/// @author Sajjadali Hemani
///
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
    bool newEcgDataAvailable();

    QStringList listServices();
    QList<QStringList> listDevices();

    int getBatteryLevel();
    QString getManufacturerName();
    int *getEcgData();

private:
    // An array that contains the ECG timestamp and reading samples.
    int ecgData[7];

};

#endif // JNIHEALTHYWAY_H
