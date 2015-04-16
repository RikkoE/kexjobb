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

}

int HealthyWayFunctions::updateButtonClicked()
{
    qDebug() << "Updating heart rate!";
    jint heart = QAndroidJniObject::callStaticMethod<jint>("org/qtproject/example/notification/NotificationClient",
                                              "updateHeartRate");
    qDebug() << "Heart rate: " << heart;
    return heart;
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

    m_model->setStringList(m_list);
}

//static void fromJavaCode(JNIEnv *env, jobject thiz, jint x) {
//    Q_UNUSED(env)
//    Q_UNUSED(thiz)
//    qDebug() << "From java code: " << x;
//}

//void registerNativeMethods() {
//    qDebug() << "Sup?";
//    JNINativeMethod methods[] = {{"callNativeFunction", "(I)V", reinterpret_cast<void *>(fromJavaCode)}};

//    QAndroidJniObject javaClass("org/qtproject/example/notification/NotificationClient");

//    QAndroidJniEnvironment env;

//    jclass objectClass = env->GetObjectClass(javaClass.object<jobject>());

//    env->RegisterNatives(objectClass, methods, sizeof(methods)/sizeof(methods[0]));

//    env->DeleteLocalRef(objectClass);
//}

