#include "mythread.h"
#include <QDebug>

MyThread::MyThread(QObject *parent, bool b) :
    QThread(parent), Stop(b)
{
}

// run() will be called when a thread starts
void MyThread::run()
{
    qDebug() << "Stop value: " << this->Stop;

    while(!(this->Stop)){
        emit valueChanged();
        this->msleep(50);
    }
    qDebug() << "Stop value: " << this->Stop;

    qDebug() << "out of loop";

    // emit the signal for the count label
}
