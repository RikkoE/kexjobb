#include "healthywayfunctions.h"
#include <QAbstractListModel>
#include <QtAndroidExtras/QAndroidJniObject>
#include <QAndroidJniEnvironment>
#include <QDebug>

HealthyWayFunctions::HealthyWayFunctions(QObject *parent):QObject(parent)
{

}


void HealthyWayFunctions::deviceClicked(const int &deviceIndex)
{
    qDebug() << "Chosen device: " << m_list.at(deviceIndex);
    QString chosenDevice = m_list.at(deviceIndex);
 /*   QAndroidJniObject deviceString = QAndroidJniObject::fromString(m_list.at(deviceIndex));
    QAndroidJniObject::callStaticMethod<void>("org/qtproject/example/notification/NotificationClient",
                                              "connectDevice",
                                              "(Ljava/lang/String;)",
                                              deviceString.object<jstring>());*/
    QAndroidJniObject::callStaticMethod<void>("org/qtproject/example/notification/NotificationClient",
                                              "connectDevice");

}

void HealthyWayFunctions::scanLeDevices()
{
    m_list.clear();
    qDebug() << "Scan start";

    QAndroidJniObject::callStaticMethod<void>("org/qtproject/example/notification/NotificationClient",
                                              "scanLeDevices");

//    QAndroidJniObject stringArray = QAndroidJniObject::callStaticObjectMethod("org/qtproject/example/notification/NotificationClient",
//                                                                              "scanLeDevices",
//                                                                              "()[Ljava/lang/String;");
//    qDebug() << "Scan stopped";

//    jobjectArray arr = stringArray.object<jobjectArray>();

//    QAndroidJniEnvironment env;

//    int size = env->GetArrayLength(arr);

//    jstring string;
//    const char *formatted;

//    for (int i=0; i < size; i++)
//    {
//        string = (jstring)env->GetObjectArrayElement(arr, i);
//        formatted = env->GetStringUTFChars(string, 0);
//        m_list.append(formatted);
//        env->ReleaseStringUTFChars(string, formatted);
//        env->DeleteLocalRef(string);
//    }

//    qDebug() << size;
//    qDebug() << string;
//    qDebug() << m_list;
//    qDebug() << formatted;

//    m_model->setStringList(m_list);
}

int HealthyWayFunctions::updateButtonClicked()
{
    qDebug() << "Updating heart rate!";
    jint heart = QAndroidJniObject::callStaticMethod<jint>("org/qtproject/example/notification/NotificationClient",
                                              "updateHeartRate");
    qDebug() << "Heart rate: " << heart;
    return heart;
}

void HealthyWayFunctions::updateModel()
{
    qDebug() << "Writing to model";
    m_model->setStringList(m_list);
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

    int size = env->GetArrayLength(arr);

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

    m_model->setStringList(m_list);
}

static void sendFibonaciResult(JNIEnv */*env*/, jobject /*obj*/, jint n)
{
    qDebug() << "Computed fibonacci is:" << n;
}

HealthyWayFunctions luder;

static void sendScanResult(JNIEnv */*env*/, jobject /*obj*/, jobjectArray stringArray)
{
    HealthyWayFunctions* pung = new HealthyWayFunctions;

    luder.m_list.clear();

    QAndroidJniEnvironment env;

    int size = env->GetArrayLength(stringArray);

    jstring string;
    const char *formatted;

    for (int i=0; i < size; i++)
    {
        string = (jstring)env->GetObjectArrayElement(stringArray, i);
        formatted = env->GetStringUTFChars(string, 0);
        luder.m_list.append(formatted);
        env->ReleaseStringUTFChars(string, formatted);
        env->DeleteLocalRef(string);
    }

    qDebug() << size;
    qDebug() << string;
    qDebug() << luder.m_list;
    qDebug() << "before model";

    QMetaObject::invokeMethod(pung, "updateModel");

    qDebug() << "after model";
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
