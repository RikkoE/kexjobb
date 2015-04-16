#include "healthywayfunctions.h"
#include <QAbstractListModel>
#include <QtAndroidExtras/QAndroidJniObject>
#include <QAndroidJniEnvironment>
#include <QDebug>

HealthyWayFunctions::HealthyWayFunctions(QObject *parent):QObject(parent)
{
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
    //    env->GetObjectArrayElement(arr,size);

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
        list.append(formatted);
        env->ReleaseStringUTFChars(string, formatted);
        env->DeleteLocalRef(string);
    }


    //    QString str = stringArray.toString();
    qDebug() << size;
    qDebug() << string;
    qDebug() << strlen;
    qDebug() << list;
    qDebug() << formatted;
//    env->ReleaseStringUTFChars(string, formatted);

    m_model->setStringList(list);
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

