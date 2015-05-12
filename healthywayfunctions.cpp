#include "healthywayfunctions.h"
#include <QAbstractListModel>
#include <QtAndroidExtras/QAndroidJniObject>
#include <QAndroidJniEnvironment>
#include <QDebug>
//#include "serviceinformation.h"
#include <thread>

HealthyWayFunctions::HealthyWayFunctions(QObject *parent):QObject(parent)
{
//    connect(m_model, SIGNAL(listChanged(QStringList)), this, SLOT(setList(QStringList)));

    mythread = new MyThread(this);
    scanThread = new MyThread(this);

    connect(mythread, SIGNAL(valueChanged(int)), this, SLOT(updateData()));
    connect(scanThread, SIGNAL(valueChanged(int)), this, SLOT(scanLeDevices()));
}

//void HealthyWayFunctions::onValueChanged(int newValue)
//{
//    updateData();
//}

HealthyWayFunctions &HealthyWayFunctions::instance(QObject *parent)
{
    static HealthyWayFunctions mainFunctions(parent);
    return mainFunctions;
}


void HealthyWayFunctions::deviceClicked(const int &deviceIndex)
{
    //qDebug() << "Chosen device: " << m_list.at(deviceIndex);

    QString chosenDevice = m_list.at(deviceIndex);
    QAndroidJniObject deviceString = QAndroidJniObject::fromString(chosenDevice);
    qDebug() << "Chosen device: " << deviceString.toString();
    QAndroidJniObject::callStaticMethod<void>("org/qtproject/example/notification/NotificationClient",
                                              "connectDevice",
                                              "(Ljava/lang/String;)V",
                                              deviceString.object<jstring>());
//    QAndroidJniObject::callStaticMethod<void>("org/qtproject/example/notification/NotificationClient",
//                                              "connectDevice");


}

void HealthyWayFunctions::disconnectDevice()
{
    QAndroidJniObject::callStaticMethod<void>("org/qtproject/example/notification/NotificationClient",
                                              "disconnectDevice");
}

void HealthyWayFunctions::listServices()
{

    QAndroidJniObject javaArray = QAndroidJniObject::callStaticObjectMethod("org/qtproject/example/notification/NotificationClient",
                                                                              "updateServices",
                                                                              "()[Ljava/lang/String;");

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

    qDebug() << size;
    qDebug() << string;
    qDebug() << list;

    m_services = list;

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
    mythread->Stop = true;

    QAndroidJniObject::callStaticMethod<void>("org/qtproject/example/notification/NotificationClient",
                                              "disconnectNotification",
                                              "(I)V",
                                              glob_characIndex);
}

void HealthyWayFunctions::testThreads()
{
    mythread->Stop = false;
    mythread->start();
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

    m_list.clear();
//    qDebug() << "Scan start";

//    QAndroidJniObject::callStaticMethod<void>("org/qtproject/example/notification/NotificationClient",
//                                              "scanLeDevices");

    QAndroidJniObject stringArray = QAndroidJniObject::callStaticObjectMethod("org/qtproject/example/notification/NotificationClient",
                                                                              "getDeviceList",
                                                                              "()[Ljava/lang/String;");
//    qDebug() << "Scan stopped";

    jobjectArray arr = stringArray.object<jobjectArray>();

    QAndroidJniEnvironment env;

    int size = env->GetArrayLength(arr);
//    qDebug() << "size: " << size;

    jstring string;
    const char *formatted;

    for (int i=0; i < size; i++)
    {
        string = (jstring)env->GetObjectArrayElement(arr, i);
        formatted = env->GetStringUTFChars(string, 0);
        m_list.append(formatted);
        env->ReleaseStringUTFChars(string, formatted);
        env->DeleteLocalRef(string);
    }

//    qDebug() << size;
//    qDebug() << string;
//    qDebug() << m_list;
//    qDebug() << formatted;

    m_devices = m_list;

    emit deviceListChanged();
//    scanThread->Stop = true;
}

int HealthyWayFunctions::updateButtonClicked()
{
    qDebug() << "Updating battery level!";
    jint battery = QAndroidJniObject::callStaticMethod<jint>("org/qtproject/example/notification/NotificationClient",
                                              "returnBatteryLevel");
    qDebug() << "Battery level: " << battery;
    return battery;
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

void HealthyWayFunctions::scanButtonClicked()
{
    m_list.clear();
    qDebug() << "Scan start";
    //    QStringList list;
    //    list.append("Done");

    //    QAndroidJniObject::callStaticMethod<void>("org/qtproject/example/notification/NotificationClient",
    //                                       "scan");
    QAndroidJniObject stringArray = QAndroidJniObject::callStaticObjectMethod("org/qtproject/example/notification/NotificationClient",
                                                                              "scanReturn",
                                                                              "()[Ljava/lang/String;");
    qDebug() << "Scan stopped";

    jobjectArray arr = stringArray.object<jobjectArray>();

    QAndroidJniEnvironment env;

    QStringList list;

    int size = env->GetArrayLength(arr);
//    jstring string = (jstring)env->GetObjectArrayElement(arr, 0);

//    const char *formatted = env->GetStringUTFChars(string, 0);
//    list.append(formatted);

    jstring string;
    const char *formatted;

    for (int i=0; i < size; i++)
    {
        string = (jstring)env->GetObjectArrayElement(arr, i);
        formatted = env->GetStringUTFChars(string, 0);
        m_list.append(formatted);
        env->ReleaseStringUTFChars(string, formatted);
        env->DeleteLocalRef(string);
    }

    qDebug() << size;
    qDebug() << string;
    qDebug() << m_list;
    qDebug() << formatted;
//    env->ReleaseStringUTFChars(string, formatted);

//    m_model->setStringList(m_list);
}

static void sendFibonaciResult(JNIEnv */*env*/, jobject /*obj*/, jint n)
{
    qDebug() << "Computed fibonacci is:" << n;
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
//    newText = n.toString();

    QMetaObject::invokeMethod(&HealthyWayFunctions::instance(), "handleSubmitTextField", Q_ARG(QString, newText));
}

void HealthyWayFunctions::handleSubmitTextField(QString in)
{
    qDebug() << "c++: HandleTextField::handleSubmitTextField:" << in;
    QVariant qv(in);
    m_bleData = qv;
    emit bleDataChanged();
}

void HealthyWayFunctions::doWork()
{
    for(int i = 0; i < 100; i++) {
        m_bleData = QVariant::fromValue(i);
//        qDebug() << "loop nr: " << i;
        emit bleDataChanged();
        usleep(500);
    }
    QThread::currentThread()->quit();
}



// step 2
// create a vector with all our JNINativeMethod(s)
static JNINativeMethod methods[] = {
    { "sendFibonaciResult", // const char* function name;
      "(I)V", // const char* function signature
      (void *)sendFibonaciResult // function pointer
    },
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

