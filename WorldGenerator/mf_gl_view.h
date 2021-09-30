#ifndef MFGLVIEW_H
#define MFGLVIEW_H
// Include standard headers
#include <stdio.h>
#include <stdlib.h>
#include <vector>
// Include GLEW
#include <GL/glew.h>

// Include GLFW
#include <GLFW/glfw3.h>

// Include GLM
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>

#include <thread>
#include <chrono>

#include "helper/mf_structs.h"

using namespace glm;
class MFGLView
{
public:
    MFGLView();
    int addGeometry(std::vector<GLuint> *indices, std::vector<GLfloat> *vertices, GLfloat *trafoData);
    int addPlane(GLfloat* lowerLeft, GLfloat*lowerRight, GLfloat* upperRight, GLfloat* upperLeft);
    void setMapData(std::vector<GLuint> *mapIndices,GLfloat *mapData, int mapSize, MF_struct_MapConfiguration mapConfig);
    void setColorData(float *&colorData);
    void generateBuffers();
    void initializeGL();
    void paintGL();
    bool paintSingleFrame();
    void setSingleStepRender(bool singleStep);
protected:


private:
    //Window
    GLFWwindow* window;
    void draw();
    void drawPlanes();
    void drawCoordinateSystem();
    void drawFloor();
    void drawCameraVectors();
/*##########################################*/
    //Control functions and variables
    void computeMatricesFromInputs();

    // Initial position : on +Z
    glm::vec3 position = glm::vec3( 0, 0, -10 );
    vec3 DOF = vec3(0,-1,0);
    vec3 RIGHT = vec3(1,0,0);
    glm::vec3 rotAxeXCam = glm::vec3(1.0,0.0,0.0);
    glm::vec3 rotAxeYCam = glm::vec3(0.0,1.0,0.0);
    glm::vec3 rotAxeZCam = glm::vec3(0.0,0.0,1.0);
    // Initial horizontal angle : toward -Z
    float horizontalAngle = 3.14f;
    // Initial vertical angle : none
    float verticalAngle = 0.0f;
    // Initial Field of View
    float initialFoV = 45.0f;

    float speed = 3.0f; // 3 units / second
    float mouseSpeed = 0.01f;
    double lastTime;
    double currentTime;
    float deltaTime;
    double xpos=0, ypos=0;
/*##########################################*/

    bool mColorBufferValid;
    bool m_singleStep = false;
    GLfloat rotationX=0.0;
    GLfloat rotationY=0;
    GLfloat rotationZ=0;
    int m_focused = 0;
    //MapData
    int width=1024, height=800;
    bool bufferValid;
    int mMapSize=0;
    GLfloat *mPMapData;
    float *mPColorData;
    MF_struct_MapConfiguration mMapConfig;
    std::vector<GLuint> *m_mapIndices;//Indexing
    GLuint elementbuffer[2]; //element buffer index (to buffer)
    GLuint colorbuffer=0;
    typedef std::vector<GLfloat>* pVectorGLfloat;
    typedef std::vector<GLuint>* pVectorGLuint;
    //Object data
    std::vector<pVectorGLfloat> m_objectPVector;
    std::vector<pVectorGLuint> m_objectIndicesPVector;
    std::vector<GLfloat*> m_objectTransformation;

};

#endif // MFGLVIEW_H
