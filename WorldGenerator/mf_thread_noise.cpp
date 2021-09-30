#include "mf_thread_noise.h"
#include "mf_signal_ctrl.h"
#include <iostream>



MFThreadNoise::MFThreadNoise(MFSignalCtrl *sigCtl,MFMapGenerator *generator,QWidget *parent)
{
    stopped=false;
    mGen=generator;
    mSignalCtl = sigCtl;
    mPParent=parent;

    clearTasks();

    mView = new MFGLView();
    if(sigCtl == 0)
        mSignalCtl= new MFSignalCtrl();

}

MFThreadNoise::~MFThreadNoise(){
    mSignalCtl->blockSignals(true);
    this->blockSignals(true);
    stopped = true;
    delete(mView);
    this->exit(0);
}

void MFThreadNoise::run(){
    while(!stopped && (taskCounter>0)){
        mutex.lock();
        switch (tasks[taskCounter-1]) {
        case TASK_GENERATE_MAP:
            if(mGen == 0)
                break;
            mGen->startCalculation();
            mGen->getMapData(&mPMapBuffer);
            colors = *(mGen->getColorData());
            mSignalCtl->emitMapCalculated();
            tasks[taskCounter-1]=0;
            taskCounter--;
            break;

        case TASK_SAVE_MAP:
            if((mPMapBuffer==0) && (0>=mGen->getMapByteSize()) ){
                tasks[taskCounter-1]=0;
                taskCounter--;
                mSignalCtl->emitMapSaved();
                break;
            }

            if(!mapName.isEmpty())
                mGen->saveFile(mapName);
            mSignalCtl->emitMapSaved();
            tasks[taskCounter-1]=0;
            taskCounter--;
            break;

        case TASK_VIEW_MAP:
            if((mPMapBuffer==0) && (0>=mGen->getMapByteSize()) ){
                mSignalCtl->emitViewClosed();
                tasks[taskCounter-1]=0;
                taskCounter--;
                break;
            }

            mGen->getOGLIndices(&m_mapIndices);

            mView->setMapData(m_mapIndices,mPMapBuffer,mGen->getMapByteSize(),mGen->getMapConfig());
            mView->setColorData(colors);
            mView->generateBuffers();
            mView->initializeGL();
            mView->paintGL();
            tasks[taskCounter-1]=0;
            taskCounter--;
            delete(mView);
            mView = new MFGLView();
            //mSignalCtl->emitViewClosed();
            break;
        case TASK_IDLE:
            sleep(0.3);
            stopped = true;
            break;
        default:
            break;
        }

        mutex.unlock();
        sleep(0.5);
    }
}

void MFThreadNoise::setColorVector(std::vector<glm::vec3> colVec){
    mutex.lock();
    mGradientColors = colVec;
    mutex.unlock();
}

void MFThreadNoise::setMapGenerator(MFMapGenerator *mapGenerator){
    mutex.lock();
    mGen = mapGenerator;
    mutex.unlock();
}

void MFThreadNoise::addTask(int task){
    mutex.lock();
    tasks[taskCounter%TASK_COUNT]=task;
    stopped = false;
    start();
    taskCounter++;
    mutex.unlock();
}

void MFThreadNoise::saveAs(QString fileName){
    mutex.lock();
    mapName=fileName;
    tasks[taskCounter%TASK_COUNT]=TASK_SAVE_MAP;
    taskCounter++;
    mutex.unlock();
}

void MFThreadNoise::stop(){
    mutex.lock();
    stopped=true;
    mutex.unlock();
}

void MFThreadNoise::continueWhile(){
    mutex.lock();
    stopped=false;
    this->start();
    mutex.unlock();
}

void MFThreadNoise::clearTasks(){
    mutex.lock();
    taskCounter=1;
    tasks[0]=TASK_IDLE;
    for(int i=1;i<TASK_COUNT;i++){
        tasks[i]=-1;
    }
    mutex.unlock();
}
