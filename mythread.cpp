#include "mythread.h"
#include <QDebug>

MyThread::MyThread(QObject *parent, bool b) :
    QThread(parent), Stop(b)
{
}

// run() will be called when a thread starts
void MyThread::run()
{
    for(int i = 0; i <= 100; i++)
    {
        QMutex mutex;
        // prevent other threads from changing the "Stop" value
        mutex.lock();
        if(this->Stop) break;
        mutex.unlock();

        // emit the signal for the count label

        emit valueChanged(i);

        // slowdown the count change, msec
        this->msleep(500);
    }
}
