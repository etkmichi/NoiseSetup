#ifndef MF_NOISE_MODIFIER_H
#define MF_NOISE_MODIFIER_H
#include <QWidget>
#include <QListWidget>
#include <QMessageBox>
#include <noise/noise.h>
#include "mf_noise_combinor.h"
#include "mf_noise_generator.h"
#include "mf_noise_selector.h"
#include "mf_noise_transformer.h"
#include "mf_xy_diagramm.h"

class MFNoiseCombinor;
class MFNoiseSelector;
class MFNoiseTransformer;
namespace Ui {
class MFNoiseModifier;
}

class MFNoiseModifier : public QWidget
{
    Q_OBJECT

public:
    explicit MFNoiseModifier(int indexID,QString name,QWidget *parent=0);
    ~MFNoiseModifier();

    QString getName(){return mName;}
    int getIndexID(){return mIndexID;}

    //myID is used to check if the calling module is used as input
    noise::module::Module* getSourceModule(noise::module::Module *mPInputCheck);

    double getNoiseValue(double x = 0.0, double y = 0.0, double z = 0.0);

    //!!!this vectors must be set before using the gui!!!
    void setCombinorVectorPointer(std::vector<MFNoiseCombinor*> *list);
    void setGeneratorVectorPointer(std::vector<MFNoiseGenerator*> *list);
    void setModifierVectorPointer(std::vector<MFNoiseModifier*> *list){mPModifierVector = list;}
    void setSelectorVectorPointer(std::vector<MFNoiseSelector*> *list){mPSelectorVector = list;}
    void setTransformerVectorPointer(std::vector<MFNoiseTransformer*> *list){mPTransformerVector = list;}

    //updates the input list
    void updateInputLists();

    //returns a pointer to the input module
    noise::module::Module*  getActiveIputModule();

private slots:
    void on_comboBoxModifier_currentIndexChanged(int index);
    void on_comboBoxType1_currentIndexChanged(int index);
    void on_listWidgetInput1_activated(const QModelIndex &index);
    void on_sliderInput1_sliderMoved(int position);
    void on_sliderOutput_sliderMoved(int position);
    void on_buttonView_clicked();
    void slotMapCalculated();
    void updateDiagrams();

    void slotLowerBoundChanged(double lower);
    void slotUpperBoundChanged(double upper);
    void slotExponentChanged(double exponent);
    void slotCurveAdd();
    void slotCurveEdit();
    void slotCurveRemove();

    void slotTerraceAdd();
    void slotTerraceRemove();
    void slotTerraceEdit();
    void slotTerraceInvert(bool toggle);
    void slotTerraceMake();

    void slotBiasChanged(double bias);
    void slotScaleChanged(double scale);
private:
    double mClambLowerBound =-1.0, mClambUpperBound=+1.0;
    bool readInputValues();
    bool readFromLineEdit(QStringList *data, QLineEdit *lineEdit);
    //diese funktion prüft rekursiv alle inputs von pointer, um herrauszufinden, ob pointer bereits als input verwendet wird.
    //falls pointer bereits als input verwendet wird, wird false zurückgegeben
    bool checkForSameInputPointer(noise::module::Module *pointerToCheck, noise::module::Module *srcModule);
    bool checkVectorPointers();
    void drawXYDiagram(MFXYDiagramm *diagram, double max,noise::module::Module *module);
    void setInputModule(noise::module::Module *input);

    Ui::MFNoiseModifier *ui;
    QString mName;
    int mIndexID;
    int mInputIndex = -1;

    std::vector<MFNoiseGenerator*>   *mPGeneratorVector;
    std::vector<MFNoiseCombinor*>    *mPCombinerVector;
    std::vector<MFNoiseModifier*>    *mPModifierVector;
    std::vector<MFNoiseSelector*>    *mPSelectorVector;
    std::vector<MFNoiseTransformer*> *mPTransformerVector;
    noise::module::Cache    *mOutputCache;
    noise::module::Module   *mInputModule;
    MFXYDiagramm mInputDiag;
    MFXYDiagramm mOutputDiag;
    noise::module::Abs          mModifierAbs;
    noise::module::Clamp        mModifierClamp;
    noise::module::Curve        mModifierCurve;
    noise::module::Exponent     mModifierExponent;
    noise::module::Invert       mModifierInvert;
    noise::module::ScaleBias    mModifierScaleBias;
    noise::module::Terrace      mModifierTerrace;

    void makeClambWidgets();
    void makeCurveWidgets();
    void makeExponentWidgets();
    void makeScaleBiasWidgets();
    void makeTerraceWidgets();

    //color
    MFColorSettings *mColorSettings;

    //map
    MFSignalCtrl *mSigCtl;
    MFThreadNoise* mMapThread;
    MFMapGenerator* mMapGen;
    MF_struct_MapConfiguration mMapConfig;

    //Widgets for modifiers
    QListWidget *mListPoints;
    double mExponent = 1.0;
    double mScale = 1.0, mBias = 0.0;

};

#endif // MF_NOISE_MODIFIER_H
