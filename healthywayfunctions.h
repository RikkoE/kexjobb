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


class HealthyWayFunctions : public QObject
{
    Q_OBJECT

    Q_PROPERTY(QVariant serviceList READ getServiceList NOTIFY serviceListChanged)
    Q_PROPERTY(QVariant deviceList READ getDeviceList NOTIFY deviceListChanged)
    Q_PROPERTY(QVariant bleData READ getBleData NOTIFY bleDataChanged)

public:
    // QObjects are expected to support a parent/child hierarchy.  I've modified
    // the constructor to match the standard.
    HealthyWayFunctions(QObject *parent = 0);

    static HealthyWayFunctions &instance(QObject *parent = 0);

    Q_INVOKABLE void scanButtonClicked();
    Q_INVOKABLE void offButtonClicked();
    Q_INVOKABLE void onButtonClicked();
    Q_INVOKABLE void deviceClicked(const int &deviceIndex);
    Q_INVOKABLE void scanLeDevices();
    Q_INVOKABLE int updateButtonClicked();
    Q_INVOKABLE void disconnectDevice();
    Q_INVOKABLE void listServices();
    Q_INVOKABLE void getCharacteristicData(const int &deviceIndex);
    Q_INVOKABLE void disconnectNotification();
    Q_INVOKABLE void testThreads();
    Q_INVOKABLE void updateData();

    void invokeSignal(QStringList);
    QVariant getServiceList();
    QVariant getDeviceList();
    QVariant getBleData();

    QStringListModel * m_model = new QStringListModel;
    QStringList m_list;

    QVariant serviceList() const
    {
        return m_serviceList;
    }

    QVariant bleData() const
    {
        return m_bleData;
    }

public slots:
    void handleSubmitTextField(QString newText);
    void doWork();

signals:
    void setTextField(QVariant text);

    void listUpdated(QStringList newList);

    void listChanged(QStringList newList);

    void serviceListChanged();

    void deviceListChanged();

    void bleDataChanged();

private:
    // This method needs to take either a QString or a const reference to one.
    // (QML doesn't support returning values via the parameter list.)
    QThread thread;

    QVariant m_serviceList;

    int glob_characIndex=0;

    QStringList m_services;
    QStringList m_devices;

    QVariant m_deviceList;
    QVariant m_bleData;
};


#endif // HEALTHYWAYFUNCTIONS_H
