#ifndef MAPVIEW_H
#define MAPVIEW_H
#define GL_GLEXT_PROTOTYPES
#include <QObject>
#include <QMouseEvent>
#include <QGLWidget>
#include <QColorDialog>

#include <GL/glu.h>
#include <GL/gl.h>
#include <stdlib.h>
#include <glm/glm.hpp>
#include <GLFW/glfw3.h>
#include <GLFW/glfw3native.h>

#include <QMutex>

#include "helper/mf_structs.h"

class MapView : public QGLWidget
{
    Q_OBJECT
public:
    MapView(QWidget *parent);
    void setMapData(GLfloat *mapData, int mapSize, MF_struct_MapConfiguration mapConfig);
    void setColorData(GLfloat *colorData);
    void generateBuffers();
protected:
    void initializeGL();
    void resizeGL(int width, int hight);
    void paintGL();

    void computeMatricesFromInputs();
private:
    void draw();
    int faceAtPosition(const QPoint &pos);

    bool mColorBufferValid;
    GLfloat rotationX;
    GLfloat rotationY;
    GLfloat rotationZ;
    QColor faceColors[4];
    QPoint lastPos;
    //MapData

    bool bufferValid;
    int mMapSize=0;
    GLfloat *mPMapData;
    GLfloat *mPColorData;
    MF_struct_MapConfiguration mMapConfig;
    std::vector<GLuint> indices;//Indexing
    GLuint elementbuffer[2]; //element buffer index (to buffer)
    GLuint colorbuffer=0;

};

#endif // MAPVIEW_H
