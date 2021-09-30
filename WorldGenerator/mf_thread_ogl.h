#ifndef MF_OGL_THREAD_H
#define MF_OGL_THREAD_H

#include <QMutex>
#include <QThread>

#include "mapview.h"


class MFThreadOGL : public QThread
{

    Q_OBJECT

public:
    MFThreadOGL(QWidget *parent);
    ~MFThreadOGL();
    void stop();
    void showMapView();

protected:
    void run();

private:
    QThread *secondThread;
    bool stopped;
    QWidget *pParent;
    QMutex mutex;
    MapView *mGLView;
};

#endif // MF_OGL_THREAD_H
