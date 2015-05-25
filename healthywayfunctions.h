#ifndef HEALTHYWAYFUNCTIONS_H
#define HEALTHYWAYFUNCTIONS_H

#include <QDebug>
#include <QObject>
#include <QStringListModel>
#include <QVariant>
#include <unistd.h>
#include <QThread>
#include "mythread.h"
#include "jnihealthyway.h"

///
/// \brief The HealthyWayFunctions class
///
/// This is a class that contains all the logistics of the application. It is here all
/// calculations and signals originate from. This class uses other classes to contact
/// the native code and to display the GUI of the application.
///

class HealthyWayFunctions : public QObject
{
    Q_OBJECT

    ///
    /// \brief
    /// Contains the list of services on a connected device and sets up a
    /// connection so the qml can get to the information via the getServiceList
    /// and get notified of changes via serviceListChanged.
    Q_PROPERTY(QVariant serviceList READ getServiceList NOTIFY serviceListChanged)
    ///
    /// \brief
    /// Contains the list of devices available and sets up a
    /// connection so the qml can get to the information via the getDeviceList
    /// and get notified of changes via deviceListChanged.
    Q_PROPERTY(QVariant deviceList READ getDeviceList NOTIFY deviceListChanged)
    Q_PROPERTY(QVariant bleData READ getBleData NOTIFY bleDataChanged)
    Q_PROPERTY(int ecgTimeStamp READ ecgTimeStamp NOTIFY ecgTimeStampChanged)
    Q_PROPERTY(int ecgReading READ ecgReading NOTIFY ecgReadingChanged)
    Q_PROPERTY(int batteryLevel READ batteryLevel NOTIFY batteryLevelChanged)


public:
    HealthyWayFunctions(QObject *parent = 0);

    ///
    /// \brief Creates a pointer to MyThread to be used to fetch data from
    /// the bluetooth low energy devices' services without locking the GUI.
    ///
    MyThread *dataThread;
    ///
    /// \brief Creates a pointer to MyThread to be used to scan for nearby
    /// bluetooth low energy devices without locking the GUI.
    ///
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

    int batteryLevel() const
    {
        return m_batteryLevel;
    }

    int ecgTimeStamp() const
    {
        return m_ecgTimeStamp;
    }

    int ecgReading() const
    {
        return m_ecgReading;
    }

public slots:
    void scanLeDevices();
    void updateData();

signals:
    ///
    /// \brief serviceListChanged
    /// is a signal that tells the qml to update the service list
    /// because it has changed.
    void serviceListChanged();
    ///
    /// \brief deviceListChanged
    /// is a signal that tells the qml to update the device list
    /// because it has changed.
    void deviceListChanged();
    ///
    /// \brief bleDataChanged
    /// is a signal that tells the qml that the value of the bleData
    /// has changed which updates the "Data: " label.
    void bleDataChanged();

    ///
    /// \brief scanningStarted
    /// is a signal that tells the qml that the device is scanning.
    void scanningStarted();
    ///
    /// \brief scanningStopped
    /// is a signal that tells the qml that the device no longer
    /// is scanning.
    void scanningStopped();

    ///
    /// \brief showEcgCanvas
    /// gives a signal to the qml to show the heart rate animation
    void showEcgCanvas();
    ///
    /// \brief showBatteryCanvas
    /// gives a signal to the qml to show the battery level animation
    void showBatteryCanvas();
    ///
    /// \brief batteryLevelChanged
    /// is a signal that tells the qml that the battery level has
    /// changed and is available for displaying
    void batteryLevelChanged();
    ///
    /// \brief ecgTimeStampChanged
    /// is a signal that tells the qml a new ECG timestamp is available
    /// for the graoh
    void ecgTimeStampChanged();
    ///
    /// \brief ecgReadingChanged
    /// is a signal that tells the qml a new ECG reading is available
    /// for the graph
    void ecgReadingChanged();

private:
    //! A variable to hold the ECG timestamp
    int glob_timeStamp = -1;

    ///
    /// \brief m_services
    /// holds the list of the services available on the connected device
    QStringList m_services;
    ///
    /// \brief m_devices
    /// holds the list of the bluetooth low energy devices available
    QStringList m_devices;

    ///
    /// \brief chosenCharacteristic
    /// holds the name of the characteristic the user wants to look at
    QString chosenCharacteristic;

    ///
    /// \brief java
    /// creates an object to the c++ class that holds the communication
    /// to the native code
    JNIHealthyWay *java = new JNIHealthyWay();

    int m_batteryLevel;
    int m_ecgTimeStamp = 0;
    int m_ecgReading = 0;
    QVariant m_bleData = " ";
};


#endif // HEALTHYWAYFUNCTIONS_H
