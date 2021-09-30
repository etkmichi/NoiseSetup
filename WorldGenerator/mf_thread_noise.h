#ifndef MFNOISETHREAD_H
#define MFNOISETHREAD_H

#define TASK_COUNT 10

#include "mf_gl_view.h"
#include <QThread>
#include <QMutex>
//#include "mapview.h"
#include "mf_map_generator.h"
#include "mf_signal_ctrl.h"


class MFThreadNoise : public QThread
{
public:
    MFThreadNoise(MFSignalCtrl *sigCtl, MFMapGenerator *generator = 0, QWidget *parent=0);
    ~MFThreadNoise();
    int appendTask(int task);
    void stop();
    void addTask(int task);
    void saveAs(QString fileName);
    void setColorVector(std::vector<glm::vec3> colVec);
    void setMapGenerator(MFMapGenerator *mapGenerator);
    void continueWhile();
    void clearTasks();
signals:
    void viewClosed();
protected:
    void run();


private:
    std::vector<glm::vec3> mGradientColors;
    //MapView *mGLView;
    GLfloat fla;
    GLfloat* mPMapBuffer;
    float* colors;
    QWidget *mPParent;
    QMutex mutex;
    QString mapName;
    int runCounter=0;
    volatile bool stopped;
    int tasks[TASK_COUNT];
    int taskCounter=0;
    MFMapGenerator *mGen;
    MFGLView* mView;
    MFSignalCtrl *mSignalCtl;
    std::vector<GLuint> *m_mapIndices;
};

#endif // MFNOISETHREAD_H
