#ifndef MFMAPGENERATOR_H
#define MFMAPGENERATOR_H

#include <GL/glew.h>
#include "noise/noise.h"
#include "helper/mf_structs.h"
#include "helper/mf_def.h"
#include <glm/glm.hpp>
#include <QStatusBar>
#include <QMutex>
#include <QLabel>
#include <QFileDialog>
#include <QInputDialog>
#include <QWidget>
#include <QDialog>
class MFMapGenerator:public QObject
{
Q_OBJECT
public:
    MFMapGenerator();
    ~MFMapGenerator();
    void setSourceModule(noise::module::Module **sourceModule);
    void generateMapBuffer();
    void calculateMap();
    void getOGLIndices(std::vector<GLuint> **indices);
    int getVertexCount();
    int getTriangleCount();
    bool setMapConfiguration(MF_struct_MapConfiguration configData);
    bool saveFile(const QString &fileName);
    bool saveAs();
    void getStatus(QString* statLabel, QString *statMessage);
    void getMapData(float **mapData);
    float** getColorData();
    void setColorVector(std::vector<glm::vec3> colorVector);
    std::vector<glm::vec3> getGradientColorVector();
    MF_struct_MapConfiguration getMapConfig();
    int getMapByteSize();
    bool waitForMutex();


public slots:
    void startCalculation();
signals:
    void statusUpdated();
    void done();

private:
    glm::vec3 getColor(float height);

    //color Calculation
    uint gradientCount=0;
    std::vector<glm::vec3> mGradientColors;
    double mMax = 1.0;
    double mMin = -1.0;
    int mGradientColorScale = 30;
    int mColorCount = 100;
    float* mColorMapBuffer;


    //map calculation
    MF_struct_MapConfiguration mMapConfig;
    noise::module::Module *mSourceModule;
    int size = 0,xCounter=0,yCounter=0;
    int mapArrayEntries;
    float sizeX=100,sizeY=100,sizeZ;
    int resX=100,resY=100,resZ=100;
    float posX=0,posY=0,posZ=0;
    void* mPVoidMap=NULL;
    float* map;
    int m_vertexCount = 0;
    int m_triangleCount = 0;
    std::vector<GLuint> m_oGLIndices;

    //other things
    std::vector<glm::vec3> *m_initColorVector;
    noise::module::Perlin* m_initNoise;
    std::string errMsg;
    QMutex mutex;
    QString statusLabel, statusMessage;
};

#endif // MFMAPGENERATOR_H
