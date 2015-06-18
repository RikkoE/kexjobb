#include "healthywayfunctions.h"
#include <QDebug>

///
/// \brief This function is called when the object is created and connects
/// the threads (to scan and fetch data) with the functions that are to be
/// used in those situations.
///
HealthyWayFunctions::HealthyWayFunctions(QObject *parent):QObject(parent)
{
    dataThread = new MyThread(this);
    scanThread = new MyThread(this);

    /* Connects the dataThreads signal "valueChanged" to call the function/slot
     * "updateData" that fetches data from the connected device*/
    connect(dataThread, SIGNAL(valueChanged()), this, SLOT(updateData()));
    /* Connects the scanThreads signal "valueChanged" to call the function/slot
     * "scanLeDevices" that scans for nearby devices*/
    connect(scanThread, SIGNAL(valueChanged()), this, SLOT(scanLeDevices()));
}

///
/// \brief A function that takes the chosen device as a QString and sends
/// to the native function "connectDevice".
/// \param deviceIndex is the index of the item that was chosen in the
/// m_devices list from the qml.
///
void HealthyWayFunctions::deviceClicked(const int &deviceIndex)
{
    QString chosenDevice = m_deviceAddresses.at(deviceIndex);
    qDebug() << "Chosen device: " << chosenDevice;
    java->connectDevice(chosenDevice);
}

///
/// \brief A function that tells the native code to disconnect from
/// the bluetooth low energy device currently connected.
///
void HealthyWayFunctions::disconnectDevice()
{
    java->disconnectDevice();
}

///
/// \brief A function that first clears the list of current services, fetches
/// a new list of services and alerts the qml that the list has changed.
void HealthyWayFunctions::listServices()
{
    m_services.clear();

    QStringList listOfServices;
    listOfServices = java->listServices();

    m_services = listOfServices;

    emit serviceListChanged();
}

///
/// \brief A function that puts the chosen characteristic in the provate
/// variable chosenCharacteristic and starts the dataThread through
/// startDataThread.
/// \param deviceIndex is the index of the item that was chosen in the
/// m_devices list from the qml.
///
void HealthyWayFunctions::getCharacteristicData(const int &deviceIndex)
{
    chosenCharacteristic = m_services.at(deviceIndex);
    qDebug() << "Chosen characteristic: " << chosenCharacteristic;

    // Starts the thread that fetches data
    startDataThread();
}

///
/// \brief A function that resets the values stored for the ECG graph,
/// stops the dataThread and sends the name of the device to disconnect
/// to the native code.
void HealthyWayFunctions::disconnectNotification()
{
    m_ecgTimeStamp = 0;
    m_ecgReading = 0;
    dataThread->Stop = true;
    java->disconnectDataStream(chosenCharacteristic);
}

///
/// \brief A function that starts the dataThread
void HealthyWayFunctions::startDataThread()
{
    dataThread->Stop = false;
    if(chosenCharacteristic == "Battery level indicator") {
        dataThread->sleepTimer = 1000;
    } else if(chosenCharacteristic == "Manufacturer name") {
        dataThread->sleepTimer = 1000;
    } else {
        dataThread->sleepTimer = 40;
        java->getEcgData();
    }
    dataThread->start();
}

///
/// \brief A function that emits a signal to the qml that the device is
/// scanning, starts the scanning and starts the scanThread that waits
/// for the scanning to stop.
void HealthyWayFunctions::startScanThread()
{
    emit scanningStarted();
    java->scanForDevices();

    scanThread->Stop = false;
    scanThread->start();
}

///
/// \brief A big function that selects what data to get from the connected
/// device by checking the chosenCharacteristic variable. When the result
/// from the native code comes back it is treated in a way that is necessary
/// for that perticular service.
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
        int *ecgData;

        if(java->newEcgDataAvailable()) {
            ecgData = java->getEcgData();
            int ecgTimeStamp = ecgData[0];
            qDebug() << "ECG time stamp: " << ecgTimeStamp;

            // Sets the new reference timestamp.
            glob_timeStamp = ecgTimeStamp;

            /* A variable that converts the timestamp into chunks to be
             * used with the samples*/
            int timeDivider = 10/6;

            // Loops through all samples and sends them to the qml graph
            for(int i = 1; i < 7; i++) {
                qDebug() << "sample in int: " << ecgData[i];
                if(ecgData[i] < m_lowestEcgReading) {
                    m_lowestEcgReading = ecgData[i];
                }
                m_ecgReading = (ecgData[i]-m_lowestEcgReading)/100;
                m_ecgTimeStamp = ecgTimeStamp*10 + (i-1)*timeDivider;
                emit ecgReadingChanged();
            }

            // Updates the data label in the qml with the timestamp
            m_bleData = QVariant::fromValue(ecgTimeStamp);

            // Sends signal to the qml to let it know new data is available
            emit bleDataChanged();
            // Sends signal to the qml so it showa the ECG graph
            emit showEcgCanvas();
        }

    } else {
        qDebug() << "CHARACTERISTIC NOT FOUND";
    }
}

///
/// \brief Converts the value m_services
/// \return the service list
///
QVariant HealthyWayFunctions::getServiceList()
{
    return QVariant::fromValue(m_services);
}

///
/// \brief Converts the value m_devices
/// \return the list of devices
///
QVariant HealthyWayFunctions::getDeviceList()
{
    return QVariant::fromValue(m_devices);
}

///
/// \brief Converts the value m_bleData
/// \return the data for the "Data: " label
///
QVariant HealthyWayFunctions::getBleData()
{
    return QVariant::fromValue(m_bleData);
}

///
/// \brief A function that continously checks with the native code
/// if the device is scanning or not. If the scanning stops it
/// fetches the list of devices found and sends a signal to the
/// qml that displays the list.
void HealthyWayFunctions::scanLeDevices()
{
    // Fetches the status of the scanning
    bool scanning = java->scanningStatus();

    /* If the device isn't scanning, fetch the device
     * list and pass it to the qml*/
    if(!scanning) {
        // Clear old list of devices
        m_devices.clear();
        m_deviceAddresses.clear();

        QStringList listOfDeviceNames;
        QStringList listOfDeviceAddresses;
        QList<QStringList> deviceInformation;
        // Fetch new list of devices
        deviceInformation = java->listDevices();
        listOfDeviceAddresses = deviceInformation.at(0);
        listOfDeviceNames = deviceInformation.at(1);

        m_devices = listOfDeviceNames;
        m_deviceAddresses = listOfDeviceAddresses;
        // Send signal to the qml so it knows a new device list is available
        emit deviceListChanged();
        // Stop the scanThread
        scanThread->Stop = true;
        // Send signal to the qml so it knows the scanning has stopped
        emit scanningStopped();
    }
}

///
/// \brief A function that sends a request to the native code to
/// turn bluetooth on.
void HealthyWayFunctions::onButtonClicked()
{
    qDebug() << "bluetooth on";
    java->turnBluetoothOn();
}

///
/// \brief A function that sends a request to the native code to
/// turn bluetooth off.
void HealthyWayFunctions::offButtonClicked()
{
    qDebug() << "bluetooth off";
    java->turnBluetoothOff();
}
