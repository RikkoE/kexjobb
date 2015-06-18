#ifndef MYTHREAD_H
#define MYTHREAD_H

#include <QThread>
#include <QMutex>

///
/// \brief The MyThread class
///
/// This class only creates a new thread that sends a signal every loop.
/// This is used for polling the information from the native Java code.
///
/// @author Rickard Eriksson
/// @author Sajjadali Hemani
///

class MyThread : public QThread
{
    Q_OBJECT
public:
    explicit MyThread(QObject *parent = 0, bool b = false, int i = 40);
    void run();

    // if Stop = true, the thread will break
    // out of the loop, and will be disposed
    bool Stop;
    int sleepTimer;

signals:
    // To communicate with Gui Thread
    // we need to emit a signal
    void valueChanged();

public slots:

};

#endif // MYTHREAD_H
