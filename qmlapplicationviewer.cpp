#include "qmlapplicationviewer.h"

#include "qmlapplicationviewer.h"

#include <QtCore/QDir>
#include <QtCore/QFileInfo>
#include <QtQml/QQmlComponent>
#include <QtQml/QQmlEngine>
#include <QtQml/QQmlContext>
#include <QtGui/QGuiApplication>

class QmlApplicationViewerPrivate
{
    QmlApplicationViewerPrivate(QQuickView *view_) : view(view_) {}

    QString mainQmlFile;
    QQuickView *view;
    friend class QmlApplicationViewer;
    QString adjustPath(const QString &path);
};

QString QmlApplicationViewerPrivate::adjustPath(const QString &path)
{
#ifdef Q_OS_UNIX
#ifdef Q_OS_MAC
    if (!QDir::isAbsolutePath(path))
        return QCoreApplication::applicationDirPath()
                + QLatin1String("/../Resources/") + path;
#else
    QString pathInInstallDir;
    const QString applicationDirPath = QCoreApplication::applicationDirPath();
    pathInInstallDir = QString::fromLatin1("%1/../%2").arg(applicationDirPath, path);

    if (QFileInfo(pathInInstallDir).exists())
        return pathInInstallDir;
#endif
#endif
    return path;
}

QmlApplicationViewer::QmlApplicationViewer(QWindow *parent)
    : QQuickView(parent)
    , d(new QmlApplicationViewerPrivate(this))
{
    connect(engine(), SIGNAL(quit()), QCoreApplication::instance(), SLOT(quit()));
    setResizeMode(QQuickView::SizeRootObjectToView);
}

QmlApplicationViewer::QmlApplicationViewer(QQuickView *view, QWindow *parent)
    : QQuickView(parent)
    , d(new QmlApplicationViewerPrivate(view))
{
    connect(view->engine(), SIGNAL(quit()), QCoreApplication::instance(), SLOT(quit()));
    view->setResizeMode(QQuickView::SizeRootObjectToView);
}

QmlApplicationViewer::~QmlApplicationViewer()
{
    delete d;
}

QmlApplicationViewer *QmlApplicationViewer::create()
{
    return new QmlApplicationViewer();
}

void QmlApplicationViewer::setMainQmlFile(const QString &file)
{
    d->mainQmlFile = d->adjustPath(file);
    d->view->setSource(QUrl::fromLocalFile(d->mainQmlFile));
}

void QmlApplicationViewer::addImportPath(const QString &path)
{
    d->view->engine()->addImportPath(d->adjustPath(path));
}

void QmlApplicationViewer::showExpanded()
{
#if defined(Q_WS_SIMULATOR)
    d->view->showFullScreen();
#else
    d->view->show();
#endif
}

QGuiApplication *createApplication(int &argc, char **argv)
{
    return new QGuiApplication(argc, argv);
}
