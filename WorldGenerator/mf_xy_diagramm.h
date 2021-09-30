#ifndef MFXYDIAGRAMM_H
#define MFXYDIAGRAMM_H

#include <QWidget>
#include <QPainter>
#include <glm/glm.hpp>


namespace Ui {
class MFXYDiagramm;
}

class MFXYDiagramm : public QWidget
{
    Q_OBJECT

public:
    explicit MFXYDiagramm(QWidget *parent = 0);
    ~MFXYDiagramm();
    void paintEvent(QPaintEvent *event);
    void setColorGradientData(std::vector<glm::vec3> colorData);
    void setPointVector(std::vector<glm::vec3> pointVector, double maxPointX = 1.0,double maxPointY = 1.0);
    void setDiagramTitel(QString diagTitel){mTitel = diagTitel;}

private:
    Ui::MFXYDiagramm *ui;
    std::vector<glm::vec3> mColorVector;
    std::vector<glm::vec3> mPointVector;
    int     minX=-1,maxX=1, xPos = 0,
            minY=-1,maxY=1, yPos = 0,
            minZ=-1,maxZ=1, zPos = 0;//z is used for moving through an 3D array
    float   ticIntervalX = 0.1,
            ticIntervalY = 0.1;
    float mConversionFactorX = 1.0,mMaxValX = 1.0;
    float mConversionFactorY = 1.0,mMaxValY = 1.0;


    QString mTitel;
    QPen mPen;
};

#endif // MFXYDIAGRAMM_H
