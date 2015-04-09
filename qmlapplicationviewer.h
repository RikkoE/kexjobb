
#ifndef QMLAPPLICATIONVIEWER_H
#define QMLAPPLICATIONVIEWER_H

#include <QtQuick/QQuickView>
#include <QtGui/QGuiApplication>

class QmlApplicationViewer : public QQuickView
{
    Q_OBJECT

public:
    explicit QmlApplicationViewer(QWindow *parent = 0);
    virtual ~QmlApplicationViewer();

    static QmlApplicationViewer *create();

    void setMainQmlFile(const QString &file);
    void addImportPath(const QString &path);

    void showExpanded();

private:
    explicit QmlApplicationViewer(QQuickView *view, QWindow *parent);
    class QmlApplicationViewerPrivate *d;
};

QGuiApplication *createApplication(int &argc, char **argv);

#endif // QMLAPPLICATIONVIEWER_H
