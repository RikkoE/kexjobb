#ifndef SERVICEINFORMATION_H
#define SERVICEINFORMATION_H

#include <QObject>
#include <QList>

class CharacteristicInformation
{
public:
    void setName(QString);
    void setUUID(QString);
    QString getName();
    QString getUUID();
private:
    QString characteristicName;
    QString characteristicUUID;
};

class ServiceInformation
{
public:
    void setName(QString);
    void setUUID(QString);
    QString getName();
    QString getUUID();
    QList<CharacteristicInformation> characteristics;
private:
    QString serviceName;
    QString serviceUUID;

};



#endif // SERVICEINFORMATION_H
