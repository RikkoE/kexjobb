#include "serviceinformation.h"

void ServiceInformation::setName(QString nameToAdd)
{
    serviceName = nameToAdd;
}

void ServiceInformation::setUUID(QString addressToAdd)
{
    serviceUUID = addressToAdd;
}

QString ServiceInformation::getName()
{
    return serviceName;
}

QString ServiceInformation::getUUID()
{
    return serviceUUID;
}



void CharacteristicInformation::setName(QString nameToAdd)
{
    characteristicName = nameToAdd;
}

void CharacteristicInformation::setUUID(QString addressToAdd)
{
    characteristicUUID = addressToAdd;
}

QString CharacteristicInformation::getName()
{
    return characteristicName;
}

QString CharacteristicInformation::getUUID()
{
    return characteristicUUID;
}
