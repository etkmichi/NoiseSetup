#include "mf_map_generator.h"
#include <QFile>
#include <QTextStream>
#include <fstream>
#include <iostream>

MFMapGenerator::MFMapGenerator()
{
    map=0;

    mColorMapBuffer = 0;

    mMapConfig.posX = 0;
    mMapConfig.posY = 0;
    mMapConfig.posZ = 0;
    mMapConfig.resX = 3;
    mMapConfig.resY = 3;
    mMapConfig.resZ = 3;
    mMapConfig.sizeX=40;
    mMapConfig.sizeY=40;
    mMapConfig.sizeZ = 0;

    sizeX =mMapConfig.sizeX;
    sizeY =mMapConfig.sizeY;
    sizeZ =mMapConfig.sizeZ;

    resX=mMapConfig.resX;
    resY=mMapConfig.resY;
    resZ=mMapConfig.resZ;

    posX=mMapConfig.posX;
    posY=mMapConfig.posY;
    posZ=mMapConfig.posZ;

    mColorCount = 0;
    m_initColorVector = new std::vector<glm::vec3>;
    m_initColorVector->push_back(glm::vec3(0.3,0.3,0.3));
    m_initColorVector->push_back(glm::vec3(0.5,0.5,0.5));
    m_initColorVector->push_back(glm::vec3(1.0,0,0));
    m_initColorVector->push_back(glm::vec3(0,1.0,1.0));
    m_initColorVector->push_back(glm::vec3(0.7,0.7,0.7));
    setColorVector(*m_initColorVector);


    m_initNoise =new noise::module::Perlin();
    m_initNoise->SetFrequency(0.3);
    mSourceModule = m_initNoise;


}

MFMapGenerator::~MFMapGenerator(){
    //mutex.unlock();
    if(map != NULL)
        free(map);
    mGradientColors.clear();
    m_oGLIndices.clear();
    delete(m_initNoise);
    m_initColorVector->clear();
    delete(m_initColorVector);
}
void MFMapGenerator::startCalculation(){
    mutex.lock();
    generateMapBuffer();
    calculateMap();
    mutex.unlock();
}

void MFMapGenerator::generateMapBuffer(){
    //Buffer for the vertex data
    size = (sizeX+1) * (sizeY+1) * resX * resY * 3; //For every point 3x32Bit float valuesin byte (vector);
    if(map){
        free(map);
        map=NULL;
    }
    mPVoidMap =(calloc(size,sizeof(float)));
    map = (float*)(mPVoidMap);

    if(map==NULL){
        std::cout<<"Couldn't allocate the memory"<<std::endl;
        map=NULL;
        return;
    }

    //buffer for the color data
    if(mColorMapBuffer){
        free(mColorMapBuffer);
        mColorMapBuffer=NULL;
    }

    mColorMapBuffer =(float*)(calloc(size,sizeof(float)));

    if(mColorMapBuffer==NULL){
        std::cout<<"Couldn't allocate the memory"<<std::endl;
        mColorMapBuffer=NULL;
        return;
    }


    statusLabel="generate map buffer";
    statusMessage=(QString::number((size/1000000.0),'g',5)).append(" MByte of memory allocated\n");
    std::cout<<statusMessage.toStdString()<<std::endl;
}
void MFMapGenerator::setSourceModule(noise::module::Module **sourceModule){
    mSourceModule = *sourceModule;
    if(mSourceModule == 0)
        mSourceModule = new noise::module::Perlin();
}

void MFMapGenerator::calculateMap(){
    float inkX = 1.0/resX, inkY = 1.0/resY;
    int arrayCounter = 0;
    xCounter=0;
    yCounter=0;
    float zValue;
    glm::vec3 color;    int arrayVals = 3*sizeX*sizeY*resX*resY;
    if(size==0)
        return;
    mMax =mSourceModule->GetValue (posX, posY, posZ);
    mMin =mSourceModule->GetValue (posX, posY, posZ)-0.01;
    float px=0.0+posX;
    int yPoints = resY*sizeY;
    int xPoints = resX*sizeX;
    m_vertexCount = 0;
    m_triangleCount = 0;
    for(int i = 0;i<xPoints;i++){
        xCounter++;
        yCounter=0;
        //Every vertex has got x,y,z koordinates and rgb colors
        float py=0.0+posY;
        for(int j = 0; j<yPoints ; j++){
            zValue=(float)mSourceModule->GetValue(px+posX, py+posY, 0.0f+posZ);

            if(zValue > mMax)
                mMax = zValue;
            if(zValue < mMin)
                mMin = zValue;

            color = getColor(zValue);

            yCounter++;

            map[arrayCounter]=px;
            mColorMapBuffer[arrayCounter]=color[0];

            arrayCounter++;//next value is y
            map[arrayCounter]=py;
            mColorMapBuffer[arrayCounter]=color[1];

            arrayCounter++;//next value is z;
            map[arrayCounter]=zValue;
            mColorMapBuffer[arrayCounter]=color[2];

            arrayCounter++;
            py+=inkY;
            m_vertexCount++;

            if((j<(yPoints-1)) && (i <(xPoints-1)))//last row and column doesn't have following points -> j+1 with j = sizeY*resY
            {
                m_oGLIndices.push_back(j+i*yPoints);//this point
                m_oGLIndices.push_back(j+(i+1)*yPoints);//next row the second point
                m_oGLIndices.push_back(j+i*yPoints+1);//next point

                m_oGLIndices.push_back(j+i*yPoints+1);//this point
                m_oGLIndices.push_back(j+(i+1)*yPoints);//next row first point
                m_oGLIndices.push_back(j+(i+1)*yPoints+1);//next row second point
                m_triangleCount+=2;
            }

        }
        px+=inkX;
        if((i%500)==0){
            std::cout << "\r";
            statusLabel="generating map: ";
            statusMessage=(QString::number(arrayCounter*100.0/arrayVals,'g',5)).append(" % finished\n");
            std::cout<<statusLabel.toStdString()<<statusMessage.toStdString()<<endl;
        }

    }
    if(mMax == mMin)
        mMin-=0.001;
    statusMessage=(QString::number(100)).append(" % finished\n");
    mapArrayEntries=arrayCounter;
    std::cout<<statusLabel.toStdString()<<statusMessage.toStdString()<<endl;
    std::cout<<"map was calculated with "<<std::to_string(arrayCounter)\
            <<" 32bit values"<<std::endl;
}

glm::vec3 MFMapGenerator::getColor(float height){
    int my = 0;
    if(height < 0)
        my =std::abs(int((height-mMin)*mColorCount/(mMax-mMin)));

    if(height >= 0)
        my = int((height-mMin)*mColorCount/(mMax-mMin));
    if(my<mGradientColors.size()&&my>=0)
        return mGradientColors[my];
    if(my<0)
        return mGradientColors[0];

    return mGradientColors[mGradientColors.size()-1];
}

void MFMapGenerator::setColorVector(std::vector<glm::vec3> colorVector){
    mutex.lock();
    mGradientColors.clear();
    int cVSize = colorVector.size();
    if(cVSize<=1){
        mutex.unlock();
        return;
    }

    int colorIndex =0;
    float s = 1.00;
    mColorCount = 0;
    for(int i=1; i<mGradientColorScale*cVSize; i++)
    {
        if(colorIndex>=cVSize-1)
            break;

        glm::vec3 color = s*colorVector[colorIndex] + (1.0f-s)*colorVector[colorIndex+1];
        mGradientColors.push_back(color);
        mColorCount++;
        s = 1.00-(i%mGradientColorScale)*(1.0f/mGradientColorScale);//prozenzsatz für gradienten

        if(((i % mGradientColorScale) == 0))
            colorIndex++;
    }
    mGradientColors[mGradientColors.size()-1]=colorVector[cVSize-1];
    mColorCount++;
    mutex.unlock();
}
std::vector<glm::vec3> MFMapGenerator::getGradientColorVector(){
    mutex.lock();
    return mGradientColors;
    mutex.unlock();
}

void MFMapGenerator::getMapData(float **mapData){
    mutex.lock();
    *mapData =map;
    mutex.unlock();
}

float** MFMapGenerator::getColorData(){
    mutex.lock();
    return &mColorMapBuffer;
    mutex.unlock();
}

MF_struct_MapConfiguration MFMapGenerator::getMapConfig(){
    return mMapConfig;
}

int MFMapGenerator::getMapByteSize(){

    return int(sizeX*sizeY*resX*resY*3*sizeof(float));

}


bool MFMapGenerator::saveFile(const QString &fileName){
    mutex.lock();
    if(map==NULL){
        statusLabel = "map pointer == NULL - no map has been generated";
        mutex.unlock();
        return false;
    }

    statusLabel="Save file";
    statusMessage="operation in progress";

    std::cout<<statusMessage.toStdString()<<std::endl;

    QFile file(fileName);
    if (!file.open(QIODevice::WriteOnly))
    {
        statusLabel="Save file";
        statusMessage="operation failed";
        std::cout<<statusMessage.toStdString()<<std::endl;

        errMsg = "file.open(QIODevice::WriteOnly | QIODevice::Text) failed";
        mutex.unlock();
        return false;
    }

    //    int blockSize=4096;
    //    int dataWritten=0;
    //    int dataSize=sizeX*sizeY*resX*resY*sizeof(GLfloat)*3;
    //    while (dataWritten<dataSize) {
    //        if( (dataWritten+blockSize) >= dataSize){
    //            blockSize=dataSize-dataWritten;
    //        }
    //        file.setTextModeEnabled(true);
    //        file.write(&(((char*)(map))[dataWritten]),blockSize);
    //        dataWritten+=blockSize;
    //    }


    QTextStream out(&file);
    //generateObjectFileData();
    out.setNumberFlags(QTextStream::ForcePoint);
    out.setRealNumberNotation(QTextStream::FixedNotation);
    out.setRealNumberPrecision(5);
    out.setFieldAlignment(QTextStream::AlignLeft);
    out <<"#"+QString::number((sizeX+1)*resX*resY*(sizeY+1))+" Vertices == "+QString::number(mapArrayEntries/3)+"?\n";

    int vertexCounter=0;
    // ############################ write vertex data ############################
    for (int i = 0; i < mapArrayEntries ; i+=3){
        //write to File
        out <<"v "<<map[i]<<" "<<map[i+1]<<" "<< map[i+2]<<"\n";
        vertexCounter++;
        if((i%10000)==0)
        {
            statusMessage="Writing vertex data "+QString::number(i*1.0/mapArrayEntries*100.0,'g',2)+"% done with "+QString::number(file.size()/1000000,'g',2)+"MByte";
            std::cout<<statusMessage.toStdString()<<std::endl;

        }
    }
    statusMessage="Writing vertex data "+QString::number(100)+"% done with "+QString::number(file.size()/1000000)+"MByte";
    std::cout<<statusMessage.toStdString()<<std::endl;


    // ############################ write triangles ############################
    out <<"\n#"+QString::number(mapArrayEntries/3/2)+" Triangles\n";

    for(int px=1;px<xCounter-2;px++)
    {
        for(int py=1;py<=yCounter-2;py++)
        {
            out <<"f "<<px*yCounter+py  <<" "     <<px*yCounter+py+1<<" "<< (px+1)*yCounter+py<<"\n";
            out <<"f "<<px*yCounter+py+1<<" "   <<(px+1)*yCounter+py+1<<" "<< (px+1)*yCounter+py<<"\n";
        }
        statusMessage="Writing triangles "\
                +QString::number(px*100.0/xCounter)+"% done with "\
                +QString::number(file.size()/1000000)+"MByte";
        std::cout<<statusMessage.toStdString()<<std::endl;

    }
    statusMessage="Writing triangle data "+QString::number(100)+"% done with "+QString::number(file.size()/1000000)+"MByte";
    std::cout<<statusMessage.toStdString()<<std::endl;


    mutex.unlock();
    return true;
}
void MFMapGenerator::getOGLIndices(std::vector<GLuint> **indices){
    *indices = &m_oGLIndices;
}
int MFMapGenerator::getVertexCount(){
    return m_vertexCount;
}
int MFMapGenerator::getTriangleCount(){
    return m_triangleCount;
}

void MFMapGenerator::getStatus(QString *statLabel, QString *statMessage){
    if(statLabel==0 || statMessage==0)
        return;
    *statLabel=statusLabel;
    *statMessage=statusMessage;
}

bool MFMapGenerator::waitForMutex(){
    mutex.lock();
    return true;
}

bool MFMapGenerator::setMapConfiguration(MF_struct_MapConfiguration configData){
    mutex.lock();
    mMapConfig = configData;

    sizeX =mMapConfig.sizeX;
    sizeY =mMapConfig.sizeY;
    sizeZ =mMapConfig.sizeZ;

    resX=mMapConfig.resX;
    resY=mMapConfig.resY;
    resZ=mMapConfig.resZ;

    posX=mMapConfig.posX;
    posY=mMapConfig.posY;
    posZ=mMapConfig.posZ;

    mutex.unlock();
    return true;
}

bool MFMapGenerator::saveAs(){
    mutex.lock();
    bool ok=false;
    QString text = QInputDialog::getText(0, tr("QInputDialog::getText()"),
                                         tr("File Name:"), QLineEdit::Normal,
                                         QDir::home().path()+QString("/mapData.obj"), &ok);
    if (ok && !text.isEmpty() && (text.split(".").size()==2)){
        mutex.unlock();
        return saveFile(text);
    }
    mutex.unlock();
    return false;
}
