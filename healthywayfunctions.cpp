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
    QAndroidJniObject::callStaticMethod<void>("org/qtproject/example/notification/NotificationClient",
                                              "btON");
}

void HealthyWayFunctions::offButtonClicked()
{
    QAndroidJniObject::callStaticMethod<void>("org/qtproject/example/notification/NotificationClient",
                                              "btOFF");
}

void HealthyWayFunctions::scanButtonClicked()
{
    //    QStringList list;
    //    list.append("Done");

    //    QAndroidJniObject::callStaticMethod<void>("org/qtproject/example/notification/NotificationClient",
    //                                       "scan");
    QAndroidJniObject stringArray = QAndroidJniObject::callStaticObjectMethod("org/qtproject/example/notification/NotificationClient",
                                                                              "scanReturn",
                                                                              "()[Ljava/lang/String;");
    jobjectArray arr = stringArray.object<jobjectArray>();

    QAndroidJniEnvironment env;
    //    env->GetObjectArrayElement(arr,size);

    QStringList list;

    int size = env->GetArrayLength(arr);
    jstring string = (jstring)env->GetObjectArrayElement(arr, 0);

    int strlen = env->GetStringLength(string);
    //    if(strlen > 0) {
    const char *formatted = env->GetStringUTFChars(string, 0);
    list.append(formatted);

    //    }

    //    for (int i=0; i < size; ++i)
    //    {
    //        jstring string = env->GetObjectArrayElement(arr, i);
    //        result[i] = env->GetStringUTFChars(string, i);
    //        env->ReleaseStringUTFChars(string, arr);
    //        env->DeleteLocalRef(string);
    //    }


    //    QString str = stringArray.toString();
    qDebug() << size;
    qDebug() << string;
    qDebug() << strlen;
    qDebug() << list;
    qDebug() << formatted;
    env->ReleaseStringUTFChars(string, formatted);

    m_model->setStringList(list);
}

