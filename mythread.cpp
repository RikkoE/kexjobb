#include "mythread.h"
#include <QDebug>

///
/// \brief MyThread::MyThread
/// \param parent is the pointer to the parent
/// \param b represents the value of the of the Stop command
/// \param i represents the sleep value in sleepTimer
///
MyThread::MyThread(QObject *parent, bool b, int i) :
    QThread(parent), Stop(b), sleepTimer(i)
{
}

///
/// \brief MyThread::run
/// Simply runs the loop until stopped
///
void MyThread::run()
{
    // Loops until it is signaled to stop
    while(!(this->Stop)){
        // Emit a signal to invoke functions
        emit valueChanged();
        // Sleep for specified amount of time
        this->msleep(sleepTimer);
    }
}
