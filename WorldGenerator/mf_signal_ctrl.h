#ifndef MFSIGNALCTRL_H
#define MFSIGNALCTRL_H

#include <QObject>

class MFSignalCtrl : public QObject
{
    Q_OBJECT
public:
    explicit MFSignalCtrl(QObject *parent = 0);
    void emitViewClosed(){emit viewClosed();}
    void emitMapCalculated(){emit mapCalculated();}
    void emitMapSaved(){emit mapSaved();}
    void emitStopThread(){emit stopThread();}
signals:
    void viewClosed();
    void mapCalculated();
    void mapSaved();
    void stopThread();

public slots:
};

#endif // MFSIGNALCTRL_H
