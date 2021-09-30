#include "mf_gl_view.h"
#include "math.h"
#include "iostream"
#include <unistd.h>

MFGLView::MFGLView()
{
    rotationX = 0.0;
    rotationY = 0.0;
    rotationZ = 0.0;
    bufferValid=false;    mColorBufferValid=false;
    position[0]=0;
    position[1]=0;
    position[2]=-10;
    return;
}

void MFGLView::initializeGL(){
    // Initialise GLFW
    if( !glfwInit() )
    {
        fprintf( stderr, "Failed to initialize GLFW\n" );
        getchar();
        return;
    }
    glfwWindowHint(GLFW_SAMPLES, 4);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 0);

    // Open a window and create its OpenGL context
    window = glfwCreateWindow( width, height, "Tutorial 0 - Keyboard and Mouse", NULL, NULL);

    if( window == NULL ){
        fprintf( stderr, "Failed to open GLFW window. If you have an Intel GPU, they are not 3.3 compatible. Try the 2.1 version of the tutorials.\n" );
        getchar();
        glfwTerminate();
        return;
    }
    glfwSetWindowPos(window,10,20);
    glfwMakeContextCurrent(window);
    if (glewInit() != GLEW_OK) {// Initialize GLEW
        fprintf(stderr, "Failed to initialize GLEW\n");
        getchar();
        glfwTerminate();
        return ;
    }

    glfwSetInputMode(window, GLFW_STICKY_KEYS, GL_TRUE);//capture the escape key

    //glfwSetInputMode(window, GLFW_CURSOR, GLFW_CURSOR_DISABLED);// Hide mouse and enable unlimited mouvement

    glClearColor(0.4f, 0.0f, 0.4f, 0.0f);//background
    glEnable(GL_DEPTH_TEST);// Enable depth test
    glDepthFunc(GL_LESS);// Accept fragment if it closer to the camera than the former one
    glEnable(GL_CULL_FACE);

    glfwGetFramebufferSize(window, &width, &height);

    glViewport(0, 0, width, height);
    glMatrixMode(GL_PROJECTION);
    glLoadIdentity();
    GLfloat windowRation = GLfloat(width) / GLfloat(height);
    gluPerspective(60.0,windowRation,0.1,60.0);
    //glFrustum(-windowRation,+windowRation,-1.0,+1.0,4.0,15.0);
    glMatrixMode(GL_MODELVIEW);

    // glfwGetTime is called only once, before paint function is called
    lastTime = glfwGetTime();//glfwGetTime();
    glClearColor(0.2f, 0.0f, 0.4f, 0.0f);
}

int MFGLView::addGeometry(std::vector<GLuint> *indices, std::vector<GLfloat> *vertices, GLfloat *trafoData){
    if(!(indices && vertices))
        return -1;

    //add the vertices to the object array
    m_objectPVector.push_back(vertices);
    //add the indices to the object indices array
    m_objectIndicesPVector.push_back(indices);
    m_objectTransformation.push_back(trafoData);
    return m_objectPVector.size()-1;
}

void MFGLView::generateBuffers(){
    //indice buffer
//    int xSize = mMapConfig.sizeX*mMapConfig.resX;
//    int ySize = mMapConfig.sizeY*mMapConfig.resY;
//    int indexPos = 0;
//    std::vector<unsigned int> indices2(mMapSize);
//    m_mapIndices=indices2;
//    for(int px=0;px<xSize-2;px++)
//    {//
//        for(int py=1;py<ySize-2;py++)
//        {
//            m_mapIndices[indexPos]=(px+1)*ySize+py;    indexPos++;
//            m_mapIndices[indexPos]=px*ySize+py+1;      indexPos++;
//            m_mapIndices[indexPos]=px*ySize+py;      indexPos++;

//            m_mapIndices[indexPos]=(px+1)*ySize+py;  indexPos++;
//            m_mapIndices[indexPos]=(px+1)*ySize+py+1;indexPos++;
//            m_mapIndices[indexPos]=px*ySize+py+1;    indexPos++;
//        }
//    }
//    bufferValid=true;
}

void MFGLView::setColorData(float* (&colorData)){
    mPColorData = colorData;
    mColorBufferValid = true;
}

void MFGLView::paintGL()
{
    int swapValue=0;
    do{
        m_focused = glfwGetWindowAttrib(window, GLFW_FOCUSED);
        if(!m_focused)
            usleep(60000);
        // Clear the screen
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
        // Compute the MVP matrix from keyboard and mouse input
        computeMatricesFromInputs();
        draw();
        // Swap buffers
        swapValue++;
        glfwSwapBuffers(window);
        glfwPollEvents();

    } // Check if the ESC key was pressed or the window was closed
    while( glfwGetKey(window, GLFW_KEY_ESCAPE ) != GLFW_PRESS &&
           glfwWindowShouldClose(window) == 0 );

    glfwTerminate();
}

bool MFGLView::paintSingleFrame(){
    m_focused = glfwGetWindowAttrib(window, GLFW_FOCUSED);
    if(!m_focused)
        usleep(100000);
    // Clear the screen
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

    // Compute the MVP matrix from keyboard and mouse input
    computeMatricesFromInputs();
    draw();

    // Swap buffers
    glfwSwapBuffers(window);
    glfwPollEvents();

    // Check if the ESC key was pressed or the window was closed
    if( !(glfwGetKey(window, GLFW_KEY_ESCAPE ) != GLFW_PRESS &&
            glfwWindowShouldClose(window) == 0) )
    {
        glfwTerminate();
        return false;
    }
    return true;
}

void MFGLView::setSingleStepRender(bool singleStep){
    m_singleStep = singleStep;
}

void MFGLView::draw(){
    glMatrixMode(GL_MODELVIEW);
    glLoadIdentity();

    glRotatef(rotationX,rotAxeXCam[0],rotAxeXCam[1],rotAxeXCam[2]);
    glRotatef(rotationY,rotAxeYCam[0],rotAxeYCam[1],rotAxeYCam[2]);
    glRotatef(rotationZ,rotAxeZCam[0],rotAxeZCam[1],rotAxeZCam[2]);

    glTranslatef(position[0],position[1],position[2]);

    drawCoordinateSystem();
    drawFloor();
    //drawCameraVectors();


    //draw map
    if(bufferValid){
        // Index buffer
        glEnableClientState(GL_VERTEX_ARRAY);
        glEnableClientState(GL_INDEX_ARRAY );

        glPushMatrix();
        void *pdata = (void*)(&(mPColorData[0]));
        if(mColorBufferValid){
            glEnableClientState(GL_COLOR_ARRAY);
            glColorPointer(3,GL_FLOAT,0,pdata);
        }
        glVertexPointer(3,GL_FLOAT,0,mPMapData);

        // Draw the triangles !
        glDrawElements(
                    GL_TRIANGLES,      // mode
                    m_mapIndices->size(),    // count
                    GL_UNSIGNED_INT,   // type
                    m_mapIndices->data()    // element array buffer offset
                );
        glPopMatrix();
        glDisableClientState(GL_VERTEX_ARRAY);
        glDisableClientState(GL_INDEX_ARRAY );
        glDisableClientState(GL_COLOR_ARRAY );
    }

    //draw objects with indices
    glEnableClientState(GL_VERTEX_ARRAY);
    glEnableClientState(GL_INDEX_ARRAY );
    for(uint i=0; i<m_objectPVector.size() ; i++){
        //m_objectPVector contains all vertice pointers of all objects
        glPushMatrix();
        glMultMatrixf(m_objectTransformation[i]);

        glColor3b((GLbyte)(80+(i*10+30)%150),(GLbyte)(30+(i*20+80)%200),(GLbyte)(40+(i*3+50)%200));
        glClearColor(150,150,0,150);
        glVertexPointer(3,GL_FLOAT,0,m_objectPVector[i]->data());
        int count = m_objectIndicesPVector[i]->size();
        glDrawElements(
                    GL_TRIANGLES,      // mode
                    count,    // count
                    GL_UNSIGNED_INT,   // type
                    m_objectIndicesPVector[i]->data()// pointer to indexbuffer
                );
        glPopMatrix();
    }
    glDisableClientState(GL_VERTEX_ARRAY);
    glDisableClientState(GL_INDEX_ARRAY );
}

void MFGLView::setMapData(std::vector<GLuint> *mapIndices,GLfloat* mapData, int mapSize, MF_struct_MapConfiguration mapConfig){
    bufferValid = true;
    m_mapIndices = mapIndices;
    mMapSize = mapSize;
    mPMapData = mapData;
    mMapConfig = mapConfig;
}

void MFGLView::computeMatricesFromInputs(){
    // Compute time difference between current and last frame
    currentTime = glfwGetTime();

    deltaTime = float(currentTime - lastTime);

    glfwGetCursorPos(window, &xpos, &ypos);// Get mouse position
    std::cout<<"xpos="<<xpos<<" ypos="<<ypos<<std::endl;

    glfwSetCursorPos(window, width/2, height/2);// Reset mouse position for next frame
    if(glfwGetMouseButton(window , GLFW_MOUSE_BUTTON_1)){
        rotationX -= mouseSpeed * float( height/2 - ypos );
        rotationZ -= mouseSpeed * float(width/2 - xpos );
    }

    GLfloat matrix[16];
    glGetFloatv( GL_MODELVIEW_MATRIX, matrix );
    DOF = vec3(matrix[2],matrix[6],matrix[10]);
    RIGHT = vec3(matrix[0],matrix[4],matrix[8]);

    // Move forward
    if (glfwGetKey( window, GLFW_KEY_W ) == GLFW_PRESS){
        position += DOF * deltaTime * speed;
    }
    // Move backward
    if (glfwGetKey( window, GLFW_KEY_S ) == GLFW_PRESS){
        position += -DOF * deltaTime * speed;
    }
    // Strafe right
    if (glfwGetKey( window, GLFW_KEY_D ) == GLFW_PRESS){
        position += -RIGHT * deltaTime * speed;
    }
    // Strafe left
    if (glfwGetKey( window, GLFW_KEY_A ) == GLFW_PRESS){
        position += RIGHT * deltaTime * speed;
    }

    if (glfwGetKey( window, GLFW_KEY_LEFT_SHIFT) == GLFW_PRESS){
        if(speed <=30.0)
            speed+=0.2;
    }
    if (glfwGetKey( window, GLFW_KEY_LEFT_CONTROL ) == GLFW_PRESS){
        if(speed >=0.4)
            speed-=0.2;
    }

    // For the next frame, the "last time" will be "now"
    lastTime = currentTime;
}
void MFGLView::drawPlanes(){

}

void MFGLView::drawCameraVectors(){
    GLfloat matrix[16];
    glGetFloatv( GL_MODELVIEW_MATRIX, matrix );
    glLineWidth(5.0);

    glBegin(GL_LINES);
    glColor3f(1.0,1.0,0.0);
    glVertex3f(matrix[0],matrix[4],matrix[8]);
    glVertex3f(matrix[0]*5,matrix[4]*5,matrix[8]*5);
    glColor3f(0.0,1.0,1.0);
    glVertex3f(matrix[1],matrix[5],matrix[9]);
    glVertex3f(matrix[1]*5,matrix[5]*5,matrix[9]*5);

    glColor3f(1.0,0.0,1.0);
    glVertex3f(matrix[2],matrix[6],matrix[10]);
    glVertex3f(matrix[2]*5,matrix[6]*5,matrix[10]*5);
    glEnd();
}
void MFGLView::drawFloor(){
    for(float x= -20; x<20;x++){
        glLineWidth(3);
        glColor4f(0.0,0.0,0.0,0.5);
        glBegin(GL_LINES);
        glVertex3f(x,-20.0,-0.01);
        glVertex3f(x,20.0,-0.01);
        glEnd();
        for(float x2=x+0.2;(x2<x+1) && (x<20);x2+=0.2){
            glLineWidth(2);
            glColor3f(0.2,0.2,0.2);
            glBegin(GL_LINES);
            glVertex3f(x2,-20.0,-0.01);
            glVertex3f(x2,20.0,-0.01);
            glEnd();
        }
    }
    for(float y=-20 ; y<=20 ; y++){
        glLineWidth(3);
        glColor3f(0.0,0.0,0.0);
        glBegin(GL_LINES);
        glVertex3f(-20,y,-0.01);
        glVertex3f(20,y,-0.01);
        glEnd();
        for(float y2=y+0.2 ; (y2<y+1)&&(y<20) ; y2+=0.2){
            glLineWidth(2);
            glColor4f(0.2,0.2,0.2,0.5);
            glBegin(GL_LINES);
            glVertex3f(-20,y2,-0.01);
            glVertex3f(20,y2,-0.01);
            glEnd();
        }
    }
}
int MFGLView::addPlane(GLfloat *lowerLeft, GLfloat *lowerRight, GLfloat *upperRight, GLfloat *upperLeft){

}

void MFGLView::drawCoordinateSystem(){
    //draw Koordinate System;
    glLineWidth(5.0);
    glBegin(GL_LINES);
    glColor3f(1.0f,0.0f,0.0f);//x is red
    glVertex3f(-100.0,0.0,0.0);
    glColor3f(1.0f,0.5f,0.5f);
    glVertex3f(100.0,0.0,0.0);
    glColor3f(0.0f,1.0f,0.0f);//y is green
    glVertex3f(0.0,-100.0,0.0);
    glColor3f(0.2f,1.0f,0.2f);
    glVertex3f(0.0,100.0,0.0);
    glColor3f(0.0f,0.0f,1.0f);//z is blue
    glVertex3f(0.0,0.0,-100.0);
    glColor3f(0.2f,0.2f,1.0f);
    glVertex3f(0.0,0.0,100.0);
    glEnd();
}
