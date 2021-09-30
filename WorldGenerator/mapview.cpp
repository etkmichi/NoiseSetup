#include "mapview.h"

MapView::MapView(QWidget *parent)
    : QGLWidget(parent)
{
    setFormat(QGLFormat(QGL::DoubleBuffer | QGL::DepthBuffer));
    rotationX =-21.0;
    rotationY =-57.0;
    rotationZ = 0.0;
    faceColors[0] = Qt::red;
    faceColors[1] = Qt::green;
    faceColors[2] = Qt::blue;
    faceColors[3] = Qt::yellow;
    bufferValid=false;
    mColorBufferValid=false;
}

void MapView::initializeGL(){
    qglClearColor(Qt::black);
    glShadeModel(GL_FLAT);
    glEnable(GL_DEPTH_TEST);
   // glEnable(GL_CULL_FACE);
    glDepthFunc(GL_LESS);
}
void MapView::resizeGL(int width, int height){
    glViewport(0,0,width,height);
    glMatrixMode(GL_PROJECTION);
    glLoadIdentity();
    GLfloat x = GLfloat(width) / height;
    glFrustum(-x,+x,-1.0,+1.0,4.0,15.0);
    glMatrixMode(GL_MODELVIEW);
}

void MapView::generateBuffers(){

    //    elementbuffer[0]=-1;
    //    elementbuffer[1]=-1;


    //    //vertex buffer
    //    glGenBuffers(2,&elementbuffer[0]);
    //    glBindBuffer(GL_ARRAY_BUFFER,elementbuffer[0]);
    //    glBufferData(GL_ARRAY_BUFFER,mMapSize,(void*)(mPMapData),GL_STATIC_DRAW);

    //    //color buffer
    //    glGenBuffers(1, &colorbuffer);
    //    glBindBuffer(GL_ARRAY_BUFFER, colorbuffer);
    //indice buffer
    int xSize = mMapConfig.sizeX*mMapConfig.resX;
    int ySize = mMapConfig.sizeY*mMapConfig.resY;
    int indexPos = 0;
    std::vector<unsigned int> indices2(mMapSize);
    indices=indices2;
    for(int px=1;px<xSize-2;px++)
    {//
        for(int py=1;py<ySize-2;py++)
        {
            indices[indexPos]=px*ySize+py;      indexPos++;
            indices[indexPos]=px*ySize+py+1;    indexPos++;
            indices[indexPos]=(px+1)*ySize+py;  indexPos++;

            indices[indexPos]=px*ySize+py+1;    indexPos++;
            indices[indexPos]=(px+1)*ySize+py+1;indexPos++;
            indices[indexPos]=(px+1)*ySize+py;  indexPos++;
        }
    }

    //pos
    //    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER,elementbuffer[1]);
    //    glBufferData(GL_ELEMENT_ARRAY_BUFFER,indices.size()*sizeof(unsigned int),&indices[0],GL_STATIC_DRAW);
    bufferValid=true;


}
void MapView::setColorData(GLfloat *colorData){
    mPColorData = colorData;
    mColorBufferValid = true;
}

void MapView::paintGL()
{
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
    draw();
}
void MapView::draw(){

    glMatrixMode(GL_MODELVIEW);
    glLoadIdentity();
    glTranslatef(0.0,0.0,-10.0);
    glRotatef(rotationX,1.0,0.0,0.0);
    glRotatef(rotationY,0.0,1.0,0.0);
    glRotatef(rotationZ,0.0,0.0,1.0);

    if(bufferValid){
        // Index buffer
        glEnableClientState(GL_VERTEX_ARRAY);
        glEnableClientState(GL_INDEX_ARRAY );

        if(mColorBufferValid){
            glEnableClientState(GL_COLOR_ARRAY);
            glColorPointer(3,GL_FLOAT,0,mPColorData);
        }
        glVertexPointer(3,GL_FLOAT,0,mPMapData);

        // Draw the triangles !
        glDrawElements(
                    GL_TRIANGLES,      // mode
                    indices.size(),    // count
                    GL_UNSIGNED_INT,   // type
                    &indices[0]     // element array buffer offset
                );
        glDisableClientState(GL_VERTEX_ARRAY);
        glDisableClientState(GL_INDEX_ARRAY );
        glDisableClientState(GL_COLOR_ARRAY );

    }
}
//void MapView::mousePressEvent(QMouseEvent *event)
//{
//    lastPos = event->pos();
//}

//void MapView::mouseMoveEvent(QMouseEvent *event){
//    GLfloat dx = GLfloat(event->x() - lastPos.x()) / width();
//    GLfloat dy = GLfloat(event->y() - lastPos.y()) / height();
//    if(event->buttons() & Qt::LeftButton){
//        rotationX += 180*dy;
//        rotationY += 180*dx;
//        updateGL();
//    }else if (event->buttons() & Qt::RightButton) {
//        rotationX += 180*dy;
//        rotationZ += 180*dx;
//        updateGL();
//    }
//    lastPos = event->pos();

//}

int MapView::faceAtPosition(const QPoint &pos){
    const int MaxSize = 512;
    GLuint buffer[MaxSize];
    GLint viewport[4];

    makeCurrent();

    glGetIntegerv(GL_VIEWPORT,viewport);
    glSelectBuffer(MaxSize, buffer);
    glRenderMode(GL_SELECT);
    glInitNames();
    glPushName(0);

    glMatrixMode(GL_PROJECTION);
    glPushMatrix();
    glLoadIdentity();
    gluPickMatrix(GLdouble(pos.x()),GLdouble(viewport[3]-pos.y()),5.0,5.0,viewport);
    GLfloat x = GLfloat(width()*1.0 / height());
    glFrustum(-x,+x,-1.0,+1.0,4.0,15.0);
    //draw();
//    glMatrixMode(GL_PROJECTION);
//    glPopMatrix();

    if(!glRenderMode((GL_RENDERER)))
        return -1;
    return buffer[3];
}

void MapView::setMapData(GLfloat* mapData, int mapSize, MF_struct_MapConfiguration mapConfig){
    bufferValid = false;
    mMapSize = mapSize;
    mPMapData = mapData;
    mMapConfig = mapConfig;
}
