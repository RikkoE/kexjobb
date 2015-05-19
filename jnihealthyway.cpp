#include "jnihealthyway.h"

JNIHealthyWay::JNIHealthyWay()
{

}

JNIHealthyWay::~JNIHealthyWay()
{

}

void JNIHealthyWay::connectDevice(QString deviceName)
{
    QAndroidJniObject deviceString = QAndroidJniObject::fromString(deviceName);
    QAndroidJniObject::callStaticMethod<void>("org/qtproject/example/notification/NotificationClient",
                                              "connectDevice",
                                              "(Ljava/lang/String;)V",
                                              deviceString.object<jstring>());
}

void JNIHealthyWay::disconnectDevice()
{
    QAndroidJniObject::callStaticMethod<void>("org/qtproject/example/notification/NotificationClient",
                                              "disconnectDevice");
}

void JNIHealthyWay::scanForDevices()
{
    QAndroidJniObject::callStaticMethod<void>("org/qtproject/example/notification/NotificationClient",
                                              "scanLeDevices");
}

void JNIHealthyWay::disconnectDataStream(int deviceIndex)
{
    QAndroidJniObject::callStaticMethod<void>("org/qtproject/example/notification/NotificationClient",
                                              "disconnectNotification",
                                              "(I)V",
                                              deviceIndex);
}

void JNIHealthyWay::turnBluetoothOff()
{
    QAndroidJniObject::callStaticMethod<void>("org/qtproject/example/notification/NotificationClient",
                                              "btOFF");
}

void JNIHealthyWay::turnBluetoothOn()
{
    QAndroidJniObject::callStaticMethod<void>("org/qtproject/example/notification/NotificationClient",
                                              "btON");
}

void JNIHealthyWay::connectService(int serviceIndex)
{
    QAndroidJniObject::callStaticMethod<void>("org/qtproject/example/notification/NotificationClient",
                                              "getCharacData",
                                              "(I)V",
                                              serviceIndex);
}

bool JNIHealthyWay::scanningStatus()
{
    jboolean scanStatus = QAndroidJniObject::callStaticMethod<jboolean>("org/qtproject/example/notification/NotificationClient",
                                                                      "scanningStatus");
    if(scanStatus == 0) {
       return false;
    } else {
        return true;
    }
}

QStringList JNIHealthyWay::listServices()
{
    QStringList listOfServices;
    QAndroidJniEnvironment env;
    jstring string;
    const char *formatted;

    QAndroidJniObject javaArray = QAndroidJniObject::callStaticObjectMethod("org/qtproject/example/notification/NotificationClient",
                                                                              "updateServices",
                                                                              "()[Ljava/lang/String;");

    jobjectArray servicesFromJava = javaArray.object<jobjectArray>();
    int size = env->GetArrayLength(servicesFromJava);

    for (int i=0; i < size; i++)
    {
        string = (jstring)env->GetObjectArrayElement(servicesFromJava, i);
        formatted = env->GetStringUTFChars(string, 0);
        listOfServices.append(formatted);
        env->ReleaseStringUTFChars(string, formatted);
        env->DeleteLocalRef(string);
    }

    return listOfServices;
}

QStringList JNIHealthyWay::listDevices()
{
    QStringList listOfDevices;
    QAndroidJniEnvironment env;
    jstring string;
    const char *formatted;

    QAndroidJniObject stringArray = QAndroidJniObject::callStaticObjectMethod("org/qtproject/example/notification/NotificationClient",
                                                                              "getDeviceList",
                                                                              "()[Ljava/lang/String;");

    jobjectArray arr = stringArray.object<jobjectArray>();
    int size = env->GetArrayLength(arr);

    for (int i=0; i < size; i++)
    {
        string = (jstring)env->GetObjectArrayElement(arr, i);
        formatted = env->GetStringUTFChars(string, 0);
        listOfDevices.append(formatted);
        env->ReleaseStringUTFChars(string, formatted);
        env->DeleteLocalRef(string);
    }

    return listOfDevices;
}

QStringList JNIHealthyWay::getDeviceData(int serviceIndex)
{
    QStringList dataList;
    QAndroidJniEnvironment env;
    jstring string;
    const char *formatted;

    QAndroidJniObject javaArray = QAndroidJniObject::callStaticObjectMethod("org/qtproject/example/notification/NotificationClient",
                                                    "getData",
                                                    "(I)[Ljava/lang/String;",
                                                    serviceIndex);

    jobjectArray servicesFromJava = javaArray.object<jobjectArray>();
    int size = env->GetArrayLength(servicesFromJava);

    for (int i=0; i < size; i++)
    {
        string = (jstring)env->GetObjectArrayElement(servicesFromJava, i);
        formatted = env->GetStringUTFChars(string, 0);
        dataList.append(formatted);
        env->ReleaseStringUTFChars(string, formatted);
        env->DeleteLocalRef(string);
    }

    return dataList;
}

int JNIHealthyWay::deviceDataExperiment(int serviceIndex)
{
    jint x = QAndroidJniObject::callStaticMethod<jint>("org/qtproject/example/notification/NotificationClient",
                                                       "getData",
                                                       "(I)",
                                                       serviceIndex);

    return x;
}


