#ifndef MF_NOISE_TRANSFORMER_H
#define MF_NOISE_TRANSFORMER_H
#include "mf_noise_generator.h"
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
class MFNoiseGenerator;
class MFNoiseModifier;
class MFNoiseSelector;

namespace Ui {
class MFNoiseTransformer;
}

class MFNoiseTransformer : public QWidget
{
    Q_OBJECT

public:
    explicit MFNoiseTransformer(int indexID,QString name,QWidget *parent=0);
    ~MFNoiseTransformer();
    QString getName(){return mName;}
    int getIndexID(){return mIndexID;}

    //myID is used to check if the calling module is used as input
    noise::module::Module* getSourceModule(noise::module::Module *mPInputCheck);

    double getNoiseValue(double x = 0.0, double y = 0.0, double z = 0.0);

    //!!!this vectors must be set before using the gui!!!
    void setCombinerVectorPointer(std::vector<MFNoiseCombinor*> *list){mPCombinerVector = list;}
    void setGeneratorVectorPointer(std::vector<MFNoiseGenerator*> *list){mPGeneratorVector = list;}
    void setModifierVectorPointer(std::vector<MFNoiseModifier*> *list){mPModifierVector = list;}
    void setSelectorVectorPointer(std::vector<MFNoiseSelector*> *list){mPSelectorVector = list;}
    void setTransformerVectorPointer(std::vector<MFNoiseTransformer*> *list){mPTransformerVector = list;}

    //updates the input list
    void updateInputLists();


private slots:
    void slotMapCalculated();
    void on_buttonView_clicked();
    void on_comboBoxTransformerType_currentIndexChanged(int index);
    void on_comboBoxType1_currentIndexChanged(int index);

    void slotXAngleChanged(double angle);
    void slotYAngleChanged(double angle);
    void slotZAngleChanged(double angle);

    void slotXScaleChanged(double scale);
    void slotYScaleChanged(double scale);
    void slotZScaleChanged(double scale);

    void slotXTransChanged(double trans);
    void slotYTransChanged(double trans);
    void slotZTransChanged(double trans);

    void slotFrequencyChanged(double val);
    void slotPowerChanged(double val);
    void slotRoughnessChanged(int val);
    void slotSeedChanged(int val);

    void on_comboBoxDisp1_currentIndexChanged(int index);
    void on_comboBoxDisp2_currentIndexChanged(int index);
    void on_comboBoxDisp3_currentIndexChanged(int index);

    void on_listWidgetInput1_activated(const QModelIndex &index);

    void on_listWidgetXDisp_activated(const QModelIndex &index);

    void on_listWidgetYDisp_activated(const QModelIndex &index);

    void on_listWidgetZDisp_activated(const QModelIndex &index);

    void on_sliderOutput_sliderMoved(int position);
    void on_sliderSelector_sliderMoved(int position);

private:
    void unMakeDisplaceWidgets();
    void makeDisplaceWidgets();
    void makeRotateWidgets();
    void makeScalePointWidgets();
    void makeTranslatePointWidgets();
    void makeTurbulenceWidgets();
    void updateDiagrams();
    void setDispControlModules();

    //returns a pointer to the input module
    noise::module::Module* getActiveIputModule(int noiseType,int listRow);

    bool readInputValues();
    bool readFromLineEdit(QStringList *data, QLineEdit *lineEdit);
    //diese funktion prüft rekursiv alle inputs von pointer, um herrauszufinden, ob pointer bereits als input verwendet wird.
    //falls pointer bereits als input verwendet wird, wird false zurückgegeben
    bool checkForSameInputPointer(noise::module::Module *pointerToCheck, noise::module::Module *srcModule);
    bool checkVectorPointers();
    void drawXYDiagram(MFXYDiagramm *diagram, double max,noise::module::Module *module);
    void setInputModule(noise::module::Module *input);
    noise::module::Module *mActiveModuleOutput;
    Ui::MFNoiseTransformer *ui;

    QString mName;
    int     mIndexID;
    int     m_type1Index = -1, m_type2Index = -1,  m_typeSlector = -1;
    double  mUpperBound = 1.0, mLowerBound = -1.0, mEdgeFalloff = 0.0;
    std::vector<MFNoiseGenerator*>  *mPGeneratorVector;
    std::vector<MFNoiseCombinor*>   *mPCombinerVector;
    std::vector<MFNoiseModifier*>   *mPModifierVector;
    std::vector<MFNoiseSelector*>   *mPSelectorVector;
    std::vector<MFNoiseTransformer*> *mPTransformerVector;

    noise::module::Cache  *mOutputCache;
    noise::module::Module *mActiveModuleDisp1;
    noise::module::Module *mActiveModuleDisp2;
    noise::module::Module *mActiveModuleDisp3;
    noise::module::Module *mActiveModuleInput1;
    noise::module::Module *mDummyIn1;
    noise::module::Module *mDummyIn2;
    noise::module::Module *mDummyIn3;
    noise::module::Module *mDummyIn4;
    noise::module::Module *mDummyOut;

    MFXYDiagramm *mInput1Diag;
    MFXYDiagramm *mDisplacementDiag;
    MFXYDiagramm *mOutputDiag;

    noise::module::Displace     mTransformerDisplace;
    noise::module::RotatePoint  mTransformerRotatePoint;
    noise::module::ScalePoint   mTransformerScalePoint;
    noise::module::TranslatePoint mTransformerTranslatePoint;
    noise::module::Turbulence   mTransformerTurbulence;

    //color
    MFColorSettings *mColorSettings;

    //map
    MFSignalCtrl    *mSigCtl;
    MFThreadNoise   * mMapThread;
    MFMapGenerator  * mMapGen;
    MF_struct_MapConfiguration mMapConfig;
};

#endif // MF_NOISE_TRANSFORMER_H






