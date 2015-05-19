#include "healthywayfunctions.h"
#include <QAbstractListModel>
#include <QtAndroidExtras/QAndroidJniObject>
#include <QAndroidJniEnvironment>
#include <QDebug>
#include <thread>

HealthyWayFunctions::HealthyWayFunctions(QObject *parent):QObject(parent)
{
    dataThread = new MyThread(this);
    scanThread = new MyThread(this);

    connect(dataThread, SIGNAL(valueChanged()), this, SLOT(updateData()));
    connect(scanThread, SIGNAL(valueChanged()), this, SLOT(scanLeDevices()));
}

HealthyWayFunctions &HealthyWayFunctions::instance(QObject *parent)
{
    static HealthyWayFunctions mainFunctions(parent);
    return mainFunctions;
}


void HealthyWayFunctions::deviceClicked(const int &deviceIndex)
{
    QString chosenDevice = m_devices.at(deviceIndex);
    qDebug() << "Chosen device: " << chosenDevice;
    java->connectDevice(chosenDevice);

}

void HealthyWayFunctions::disconnectDevice()
{
    java->disconnectDevice();
}

void HealthyWayFunctions::listServices()
{
    m_services.clear();

    QStringList listOfServices;
    listOfServices = java->listServices();

    m_services = listOfServices;

    emit serviceListChanged();
}

void HealthyWayFunctions::getCharacteristicData(const int &deviceIndex)
{
    QString chosenCharacteristic = m_services.at(deviceIndex);
    glob_characIndex = deviceIndex;
    qDebug() << "Chosen characteristic: " << chosenCharacteristic;

    java->connectService(deviceIndex);
    QVariant qv(" ");
    m_bleData = qv;
    emit bleDataChanged();
}

void HealthyWayFunctions::disconnectNotification()
{
    m_experiment = 0;
    m_experimentY = 0;
    dataThread->Stop = true;
    java->disconnectDataStream(glob_characIndex);
}

void HealthyWayFunctions::startDataThread()
{
    dataThread->Stop = false;
    dataThread->start();
}

void HealthyWayFunctions::startScanThread()
{
    emit scanningStarted();
    java->scanForDevices();

    scanThread->Stop = false;
    scanThread->start();
}

void HealthyWayFunctions::updateData()
{
    QStringList dataList;
    dataList = java->getDeviceData(glob_characIndex);

    qDebug() << "TIME STAMP: " << dataList.at(0);

    int temp = dataList.at(0).toInt();
    int divider = 10/6;

//    m_experiment = temp;

//    emit experimentChanged();

    for(int i = 1; i < dataList.length(); i++) {
        qDebug() << "sample in int: " << dataList.at(i).toInt();
//        if(dataList.at(i).toInt() > 1400000) {
//            m_experimentY = 300;
//            m_experiment = temp*10 + (i-1)*divider;
//            emit experimentYChanged();
//        } else {
            m_experimentY = (dataList.at(i).toInt())/10000;
            m_experiment = temp*10 + (i-1)*divider;
            emit experimentYChanged();
//        }
    }

    m_bleData = QVariant::fromValue(dataList.at(0));

    emit bleDataChanged();
}

QVariant HealthyWayFunctions::getServiceList()
{
    return QVariant::fromValue(m_services);
}

QVariant HealthyWayFunctions::getDeviceList()
{
    return QVariant::fromValue(m_devices);
}

QVariant HealthyWayFunctions::getBleData()
{
    return QVariant::fromValue(m_bleData);
}

void HealthyWayFunctions::scanLeDevices()
{
    bool scanning = java->scanningStatus();

    if(!scanning) {
        scanThread->Stop = true;
        emit scanningStopped();
        return;
    }

    m_devices.clear();

    QStringList listOfDevices;
    listOfDevices = java->listDevices();

    m_devices = listOfDevices;

    emit deviceListChanged();
}

void HealthyWayFunctions::onButtonClicked()
{
    qDebug() << "bluetooth on";
    java->turnBluetoothOn();
}

void HealthyWayFunctions::offButtonClicked()
{
    qDebug() << "bluetooth off";
    java->turnBluetoothOff();
}
