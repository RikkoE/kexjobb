#ifndef HEALTHYWAYFUNCTIONS_H
#define HEALTHYWAYFUNCTIONS_H

#include <QDebug>
#include <QObject>
#include <QStringListModel>

class HealthyWayFunctions : public QObject
{
    Q_OBJECT
    QStringListModel * m_model;

public:
    // QObjects are expected to support a parent/child hierarchy.  I've modified
    // the constructor to match the standard.
    HealthyWayFunctions(QObject *parent = 0);
    HealthyWayFunctions(QStringListModel * model) : m_model(model) {}
    Q_INVOKABLE void scanButtonClicked();
    Q_INVOKABLE void offButtonClicked();
    Q_INVOKABLE void onButtonClicked();

public slots:
    // This method needs to take either a QString or a const reference to one.
    // (QML doesn't support returning values via the parameter list.)
    //void buttonClicked(const QString& in);
};


#endif // HEALTHYWAYFUNCTIONS_H
