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
/// @author Rickard Eriksson
/// @author Sajjadali Hemani
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
    ///
    /// \brief
    /// Contains the data from the remote device and sets up a
    /// connection so the qml can get to the information via the getBleData
    /// and get notified of changes via bleDataChanged.
    Q_PROPERTY(QVariant bleData READ getBleData NOTIFY bleDataChanged)

    ///
    /// \brief
    /// Contains the ECG timestamp and sets up a connection so the qml can
    /// get to the information via the ecgTimeStamp
    /// and get notified of changes via ecgTimeStampChanged.
    Q_PROPERTY(int ecgTimeStamp READ ecgTimeStamp NOTIFY ecgTimeStampChanged)
    ///
    /// \brief
    /// Contains the ECG sample and sets up a connection so the qml can
    /// get to the information via the ecgReading
    /// and get notified of changes via ecgReadingChanged.
    Q_PROPERTY(double ecgReading READ ecgReading NOTIFY ecgReadingChanged)
    ///
    /// \brief
    /// Contains the battery level and sets up a connection so the qml can
    /// get to the information via the batteryLevel
    /// and get notified of changes via batteryLevelChanged.
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

    ///
    /// \brief
    /// A function which only returns the value of the private variable m_batteryLevel
    /// \return
    /// the private variable containing the battery level
    ///
    int batteryLevel() const
    {
        return m_batteryLevel;
    }

    ///
    /// \brief
    /// A function which only returns the value of the private variable m_ecgTimeStamp
    /// \return
    /// the private variable containing the ECG timestamp
    ///
    int ecgTimeStamp() const
    {
        return m_ecgTimeStamp;
    }

    ///
    /// \brief
    /// A function which only returns the value of the private variable m_ecgReading
    /// \return
    /// the private variable containing the ECG sample
    ///
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
    int glob_timeStamp = -2;

    ///
    /// \brief m_services
    /// holds the list of the services available on the connected device
    QStringList m_services;
    ///
    /// \brief m_devices
    /// holds the list of the bluetooth low energy devices available
    QStringList m_devices;

    QStringList m_deviceAddresses;

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
    double m_ecgReading = 0;
    int m_lowestEcgReading = 1000000000;
    QVariant m_bleData = " ";
};


#endif // HEALTHYWAYFUNCTIONS_H
