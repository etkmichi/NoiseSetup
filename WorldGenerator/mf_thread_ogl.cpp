#include "mf_thread_ogl.h"
#include <iostream>

MFThreadOGL::MFThreadOGL(QWidget *parent)
{
    mutex.lock();
    pParent=parent;
    secondThread = new QThread();
    stopped=false;
    mGLView = new MapView(0);
    mGLView->moveToThread(secondThread);
    mGLView->window()->moveToThread(secondThread);
    mGLView->setWindowTitle("myMap");
    if(parent!=0)
        mGLView->setGeometry(pParent->width()+pParent->pos().x(),pParent->pos().y()+30,pParent->width(),pParent->height());
    mutex.unlock();
}

MFThreadOGL::~MFThreadOGL(){
    mutex.unlock();
}

void MFThreadOGL::run(){
    mutex.lock();
    std::cout<<std::to_string(2)<<std::endl;
    while(!stopped){
        this->sleep(1);
    }
    mutex.unlock();
}

void MFThreadOGL::stop(){
    mutex.lock();
    stopped=true;
    mutex.unlock();
}

void MFThreadOGL::showMapView(){
    mutex.lock();
    mGLView->show();
    mutex.unlock();
}
