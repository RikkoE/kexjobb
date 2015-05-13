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

    connect(dataThread, SIGNAL(valueChanged(int)), this, SLOT(updateData()));
    connect(scanThread, SIGNAL(valueChanged(int)), this, SLOT(scanLeDevices()));
}

HealthyWayFunctions &HealthyWayFunctions::instance(QObject *parent)
{
    static HealthyWayFunctions mainFunctions(parent);
    return mainFunctions;
}


void HealthyWayFunctions::deviceClicked(const int &deviceIndex)
{
    QString chosenDevice = m_devices.at(deviceIndex);
    QAndroidJniObject deviceString = QAndroidJniObject::fromString(chosenDevice);
    qDebug() << "Chosen device: " << deviceString.toString();
    QAndroidJniObject::callStaticMethod<void>("org/qtproject/example/notification/NotificationClient",
                                              "connectDevice",
                                              "(Ljava/lang/String;)V",
                                              deviceString.object<jstring>());
}

void HealthyWayFunctions::disconnectDevice()
{
    QAndroidJniObject::callStaticMethod<void>("org/qtproject/example/notification/NotificationClient",
                                              "disconnectDevice");
}

void HealthyWayFunctions::listServices()
{
    m_services.clear();

    QAndroidJniObject javaArray = QAndroidJniObject::callStaticObjectMethod("org/qtproject/example/notification/NotificationClient",
                                                                              "updateServices",
                                                                              "()[Ljava/lang/String;");

    jobjectArray servicesFromJava = javaArray.object<jobjectArray>();

    QAndroidJniEnvironment env;

    int size = env->GetArrayLength(servicesFromJava);

    jstring string;
    const char *formatted;

    for (int i=0; i < size; i++)
    {
        string = (jstring)env->GetObjectArrayElement(servicesFromJava, i);
        formatted = env->GetStringUTFChars(string, 0);
        m_services.append(formatted);
        env->ReleaseStringUTFChars(string, formatted);
        env->DeleteLocalRef(string);
    }

    emit serviceListChanged();
}

void HealthyWayFunctions::getCharacteristicData(const int &deviceIndex)
{
    QString chosenCharacteristic = m_services.at(deviceIndex);
    glob_characIndex = deviceIndex;
    qDebug() << "Chosen characteristic: " << chosenCharacteristic;

    QAndroidJniObject::callStaticMethod<void>("org/qtproject/example/notification/NotificationClient",
                                              "getCharacData",
                                              "(I)V",
                                              deviceIndex);
    QVariant qv(" ");
    m_bleData = qv;
    emit bleDataChanged();
}

void HealthyWayFunctions::disconnectNotification()
{
    dataThread->Stop = true;

    QAndroidJniObject::callStaticMethod<void>("org/qtproject/example/notification/NotificationClient",
                                              "disconnectNotification",
                                              "(I)V",
                                              glob_characIndex);
}

void HealthyWayFunctions::testThreads()
{
    dataThread->Stop = false;
    dataThread->start();
}

void HealthyWayFunctions::startScanThread()
{
    emit scanningStarted();
    QAndroidJniObject::callStaticMethod<void>("org/qtproject/example/notification/NotificationClient",
                                              "scanLeDevices");
    scanThread->Stop = false;
    scanThread->start();
}

void HealthyWayFunctions::updateData()
{
    QAndroidJniObject javaArray = QAndroidJniObject::callStaticObjectMethod("org/qtproject/example/notification/NotificationClient",
                                                    "getData",
                                                    "(I)[Ljava/lang/String;",
                                                    glob_characIndex);

    jobjectArray servicesFromJava = javaArray.object<jobjectArray>();

    QAndroidJniEnvironment env;

    int size = env->GetArrayLength(servicesFromJava);

    QStringList list;

    jstring string;
    const char *formatted;

    for (int i=0; i < size; i++)
    {
        string = (jstring)env->GetObjectArrayElement(servicesFromJava, i);
        formatted = env->GetStringUTFChars(string, 0);
        list.append(formatted);
        env->ReleaseStringUTFChars(string, formatted);
        env->DeleteLocalRef(string);
    }

    m_bleData = QVariant::fromValue(list.at(0));

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
    jboolean scanning = QAndroidJniObject::callStaticMethod<jboolean>("org/qtproject/example/notification/NotificationClient",
                                                                      "scanningStatus");
    if(scanning == 0) {
        scanThread->Stop = true;
        emit scanningStopped();
        return;
    }

    m_devices.clear();

    QAndroidJniObject stringArray = QAndroidJniObject::callStaticObjectMethod("org/qtproject/example/notification/NotificationClient",
                                                                              "getDeviceList",
                                                                              "()[Ljava/lang/String;");

    jobjectArray arr = stringArray.object<jobjectArray>();

    QAndroidJniEnvironment env;

    int size = env->GetArrayLength(arr);

    jstring string;
    const char *formatted;

    for (int i=0; i < size; i++)
    {
        string = (jstring)env->GetObjectArrayElement(arr, i);
        formatted = env->GetStringUTFChars(string, 0);
        m_devices.append(formatted);
        env->ReleaseStringUTFChars(string, formatted);
        env->DeleteLocalRef(string);
    }

    emit deviceListChanged();
}

void HealthyWayFunctions::onButtonClicked()
{
    qDebug() << "bluetooth on";
    QAndroidJniObject::callStaticMethod<void>("org/qtproject/example/notification/NotificationClient",
                                              "btON");
}

void HealthyWayFunctions::offButtonClicked()
{
    qDebug() << "bluetooth off";
    QAndroidJniObject::callStaticMethod<void>("org/qtproject/example/notification/NotificationClient",
                                              "btOFF");
}

static void sendScanResult(JNIEnv */*env*/, jobject /*obj*/, jobjectArray stringArray)
{
    HealthyWayFunctions* func = new HealthyWayFunctions;

    QMetaObject::invokeMethod(func, "updateModel", Q_ARG(jobjectArray, stringArray));

    qDebug() << "after model";
}

static void sendHeartRate(JNIEnv */*env*/, jobject /*obj*/, jint n)
{
    qDebug() << "Data from java: " << n;

    QString newText;
    newText = "Hello!";
}

// step 2
// create a vector with all our JNINativeMethod(s)
static JNINativeMethod methods[] = {
    { "sendScanResult", // const char* function name;
      "([Ljava/lang/String;)V", // const char* function signature
      (void *)sendScanResult // function pointer
    },
    { "sendHeartRate",
      "(I)V", // const char* function signature
      (void *)sendHeartRate // function pointer
    }
};


JNIEXPORT jint JNI_OnLoad(JavaVM* vm, void* /*reserved*/)
{
    JNIEnv* env;
    // get the JNIEnv pointer.
    if (vm->GetEnv(reinterpret_cast<void**>(&env), JNI_VERSION_1_6) != JNI_OK)
        return JNI_ERR;

    // step 3
    // search for Java class which declares the native methods
    jclass javaClass = env->FindClass("org/qtproject/example/notification/MyJavaNatives");
    if (!javaClass)
        return JNI_ERR;

    // step 4
    // register our native methods
    if (env->RegisterNatives(javaClass, methods,
                             sizeof(methods) / sizeof(methods[0])) < 0) {
        return JNI_ERR;
    }
    return JNI_VERSION_1_6;
}

