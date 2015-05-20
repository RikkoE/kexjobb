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
    chosenCharacteristic = m_services.at(deviceIndex);
    qDebug() << "Chosen characteristic: " << chosenCharacteristic;

    if(chosenCharacteristic == "ECG measurement") {
        emit showEcgCanvas();
    } else if(chosenCharacteristic == "Battery level indicator") {
        emit showBatteryCanvas();
    }


    startDataThread();

//    if(chosenCharacteristic == "Battery level indicator") {
//        qDebug() << "Battery chosen";
//        int batteryLevel = 0;
//        batteryLevel = java->getBatteryLevel();
//        qDebug() << "Battery level: " << batteryLevel;
//    } else if(chosenCharacteristic == "Manufacturer name") {
//        qDebug() << "Name chosen";
//        QString manufacurerName;
//        manufacurerName = java->getManufacturerName();
//        qDebug() << "Manufacturer name: " << manufacurerName;
//    } else if(chosenCharacteristic == "ECG measurement") {
//        qDebug() << "ECG chosen";
//        int *ecgData;
//        ecgData = java->getEcgData();
//        int ecgTimeStamp = ecgData[0];
//        qDebug() << "ECG time stamp: " << ecgTimeStamp;
//        for(int i = 1; i < 7; i++) {
//            qDebug() << "ECG sample: " << ecgData[i];
//        }
//    } else {
//        qDebug() << "CHARACTERISTIC NOT FOUND";
//    }
//    glob_characIndex = deviceIndex;
//    QVariant qv(" ");
//    m_bleData = qv;
//    emit bleDataChanged();
}

void HealthyWayFunctions::disconnectNotification()
{
    m_experiment = 0;
    m_experimentY = 0;
    dataThread->Stop = true;
    java->disconnectDataStream(chosenCharacteristic);
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
    if(chosenCharacteristic == "Battery level indicator") {
        qDebug() << "Battery chosen";
        int batteryLevel = 0;
        batteryLevel = java->getBatteryLevel();
        qDebug() << "Battery level: " << batteryLevel;

        m_batteryLevel = batteryLevel;
        emit batteryLevelChanged();

        m_bleData = QVariant::fromValue(batteryLevel);

        emit bleDataChanged();

        emit showBatteryCanvas();

    } else if(chosenCharacteristic == "Manufacturer name") {
        qDebug() << "Name chosen";
        QString manufacurerName;
        manufacurerName = java->getManufacturerName();
        qDebug() << "Manufacturer name: " << manufacurerName;



        m_bleData = QVariant::fromValue(manufacurerName);

        emit bleDataChanged();

    } else if(chosenCharacteristic == "ECG measurement") {
        qDebug() << "ECG chosen";
//        int ecgData[7];
        int *ecgData;
        ecgData = java->getEcgData();
        int ecgTimeStamp = ecgData[0];
        qDebug() << "ECG time stamp: " << ecgTimeStamp;
//        for(int i = 1; i < 7; i++) {
//            qDebug() << "ECG sample: " << ecgData[i];
//        }

        int timeDivider = 10/6;

        if(ecgTimeStamp != glob_timeStamp) {
            for(int i = 1; i < 7; i++) {
                qDebug() << "sample in int: " << ecgData[i];
                m_experimentY = ecgData[i]/10000;
                m_experiment = ecgTimeStamp*10 + (i-1)*timeDivider;
                emit experimentYChanged();
            }
            glob_timeStamp = ecgTimeStamp;
        }

        m_bleData = QVariant::fromValue(ecgTimeStamp);

        emit bleDataChanged();

        emit showEcgCanvas();

    } else {
        qDebug() << "CHARACTERISTIC NOT FOUND";
    }
//    QStringList dataList;
//    dataList = java->getDeviceData(glob_characIndex);

//    qDebug() << "TIME STAMP: " << dataList.at(0);

//    int temp = dataList.at(0).toInt();
//    int divider = 10/6;

//    if(temp != glob_timeStamp) {
//        for(int i = 1; i < dataList.length(); i++) {
//            qDebug() << "sample in int: " << dataList.at(i).toInt();
//            m_experimentY = (dataList.at(i).toInt())/10000;
//            m_experiment = temp*10 + (i-1)*divider;
//            emit experimentYChanged();
//        }
//        glob_timeStamp = temp;
//    }

//    m_bleData = QVariant::fromValue(dataList.at(0));

//    emit bleDataChanged();
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
