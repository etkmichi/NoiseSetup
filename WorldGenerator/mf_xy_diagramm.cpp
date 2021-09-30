#include "mf_xy_diagramm.h"
#include "ui_mf_xy_diagramm.h"

MFXYDiagramm::MFXYDiagramm(QWidget *parent) :
    QWidget(parent),
    ui(new Ui::MFXYDiagramm),
    mTitel(""),
    mPen(Qt::black)
{
    ui->setupUi(this);
    for(int i = 0 ;i<200; i++){
        mPointVector.push_back(glm::vec3(sin(i/20.0),cos(i/20.0),0.0));
    }
    mConversionFactorX = 100.0;
    mConversionFactorY = 100.0;
}

MFXYDiagramm::~MFXYDiagramm()
{
    delete ui;
}
void MFXYDiagramm::setPointVector(std::vector<glm::vec3> pointVector, double maxPointX, double maxPointY){
    mPointVector = pointVector;
    int size =mPointVector.size();

    mMaxValX=maxPointX;
    mMaxValY=maxPointY;

    if(size%2)
        mPointVector.push_back(mPointVector[size-1]);
}

void MFXYDiagramm::setColorGradientData(std::vector<glm::vec3> colorVector){
    mColorVector = colorVector;
}

void MFXYDiagramm::paintEvent(QPaintEvent *event){
    QPainter painter(this);
    painter.fillRect(this->rect(),Qt::darkGray);
    QRect tRect = this->rect();
    minX = tRect.left();
    maxX = tRect.right();
    xPos = tRect.center().x();
    maxY = tRect.bottom();
    minY = tRect.top();
    yPos = tRect.center().y();
    mConversionFactorX = maxX*0.5/mMaxValX;
    mConversionFactorY = (maxY-25)*0.5/mMaxValY;

    //draw plot
    mPen.setWidth(2);
    mPen.setColor(Qt::darkRed);
    painter.setPen(mPen);

    int pX1;
    int pY1;
    int pX2;
    int pY2;
    int colorCounter =0;
    for(uint i = 0;i<mPointVector.size()-1;i++){
        if(mPointVector[i+1][2] == 1.0 && colorCounter < 1){
            mPen.setWidth(2);
            mPen.setColor(Qt::green);
            painter.setPen(mPen);
            colorCounter++;
        }
        else if(mPointVector[i+1][2] == 2.0 && colorCounter < 2){
            mPen.setWidth(2);
            mPen.setColor(Qt::blue);
            painter.setPen(mPen);
            colorCounter++;
        }else{
            pX1 = xPos+(mPointVector[i][0]*mConversionFactorX);
            pY1 = yPos-(mPointVector[i][1]*mConversionFactorY);
            pX2 = xPos+(mPointVector[i+1][0]*mConversionFactorX);
            pY2 = yPos-(mPointVector[i+1][1]*mConversionFactorY);
            painter.drawLine(pX1,pY1,pX2,pY2);
        }
    }

    //draw coord system
    mPen.setColor(Qt::black);
    mPen.setWidth(2);
    painter.setPen(mPen);
    painter.drawLine(minX,yPos,maxX-30,yPos);//x-Axis
    painter.drawLine(maxX-30,yPos,maxX-40,yPos+5);//Arrow
    painter.drawLine(maxX-30,yPos,maxX-40,yPos-5);//Arrow
    painter.drawLine(xPos,minY,xPos,maxY);//y-Axis
    painter.drawLine(xPos,0,xPos+5,10);//Arrow
    painter.drawLine(xPos,0,xPos-5,10);//Arrow

    //draw overlay infos
    QString yMaxString = QString("yMax ="+QString::number(mMaxValY));
    QString xMaxString = QString("xMin ="+QString::number(mMaxValX));
    painter.fillRect(0,0,150,75,QColor(100,100,100,100));
    painter.drawText(0,0,150,25,Qt::AlignVCenter,yMaxString);
    painter.drawText(0,25,150,25,Qt::AlignVCenter,xMaxString);
    xMaxString = QString("xMax ="+QString::number(-mMaxValX));
    painter.drawText(0,50,150,25,Qt::AlignVCenter,xMaxString);
    QColor gradC;
    int cs = mColorVector.size();
    float h = 0.0;
    float inc = maxY * 1.0/cs;
    for(int i = cs-1; i>=0;i--){
        gradC = QColor::fromRgbF(
                    float(mColorVector[i][0]),
                    float(mColorVector[i][1]),
                    float(mColorVector[i][2])
                );
        if(cs<maxY){
            painter.fillRect(maxX-30,int(h),30,10,gradC);
        }else{
            mPen.setColor(gradC);
            painter.setPen(mPen);
            painter.drawLine(maxX-30,int(h),maxX,int(h));
        }
        h+=inc;
    }
    mPen.setColor(Qt::lightGray);
    painter.setPen(mPen);
    painter.drawText(0,maxY-25,150,25,Qt::AlignHCenter,mTitel);

}
