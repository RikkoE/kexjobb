#ifndef HEALTHYWAYFUNCTIONS_H
#define HEALTHYWAYFUNCTIONS_H

#include <QDebug>
#include <QObject>
#include <QStringListModel>
#include <QStandardItemModel>
#include <QtAndroidExtras/QAndroidJniObject>
#include <QAndroidJniEnvironment>
#include <QWidget>
#include <QVariant>
#include <unistd.h>
#include <QThread>
#include <QtConcurrent/QtConcurrent>
#include "mythread.h"
#include "jnihealthyway.h"


class HealthyWayFunctions : public QObject
{
    Q_OBJECT

    Q_PROPERTY(QVariant serviceList READ getServiceList NOTIFY serviceListChanged)
    Q_PROPERTY(QVariant deviceList READ getDeviceList NOTIFY deviceListChanged)
    Q_PROPERTY(QVariant bleData READ getBleData NOTIFY bleDataChanged)
    Q_PROPERTY(int experiment READ experiment NOTIFY experimentChanged)
    Q_PROPERTY(int experimentY READ experimentY NOTIFY experimentYChanged)


public:
    // QObjects are expected to support a parent/child hierarchy.  I've modified
    // the constructor to match the standard.
    HealthyWayFunctions(QObject *parent = 0);
    static HealthyWayFunctions &instance(QObject *parent = 0);

    MyThread *dataThread;
    MyThread *scanThread;

    Q_INVOKABLE void offButtonClicked();
    Q_INVOKABLE void onButtonClicked();
    Q_INVOKABLE void deviceClicked(const int &deviceIndex);

    Q_INVOKABLE void disconnectNotification();
    Q_INVOKABLE void disconnectDevice();

    Q_INVOKABLE void listServices();
    Q_INVOKABLE void getCharacteristicData(const int &deviceIndex);

    Q_INVOKABLE void startDataThread();
    Q_INVOKABLE void startScanThread();

    QVariant getServiceList();
    QVariant getDeviceList();
    QVariant getBleData();

    int experiment() const
    {
        return m_experiment;
    }

    int experimentY() const
    {
        return m_experimentY;
    }

public slots:
    void scanLeDevices();
    void updateData();

signals:
    void serviceListChanged();
    void deviceListChanged();
    void bleDataChanged();

    void scanningStarted();
    void scanningStopped();

    void experimentChanged();
    void experimentYChanged();

private:
    int glob_characIndex = 0;

    QStringList m_services;
    QStringList m_devices;

    QVariant m_serviceList;
    QVariant m_deviceList;
    QVariant m_bleData;

    JNIHealthyWay *java = new JNIHealthyWay();

    int m_experiment = 0;
    int m_experimentY = 0;
};


#endif // HEALTHYWAYFUNCTIONS_H
