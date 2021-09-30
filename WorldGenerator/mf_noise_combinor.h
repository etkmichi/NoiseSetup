#ifndef MF_NOISE_COMBINOR_H
#define MF_NOISE_COMBINOR_H
#include <QWidget>
#include <noise/noise.h>
#include "mf_noise_generator.h"
#include "mf_noise_modifier.h"

#include "mf_noise_selector.h"
#include "mf_noise_transformer.h"
#include "mf_xy_diagramm.h"

namespace Ui {
class MFNoiseCombinor;
}
//forward declaration because fucking compiler or qt or whatever
class MFNoiseModifier;
class MFNoiseSelector;
class MFNoiseGenerator;
class MFNoiseTransformer;
class MFNoiseCombinor : public QWidget
{
    Q_OBJECT

public:
    explicit MFNoiseCombinor(int indexID,QString name,QWidget *parent=0);
    ~MFNoiseCombinor();

    //!!!this vectors must be set before using the gui!!!
    void setGeneratorVectorPointer(std::vector<MFNoiseGenerator*> *list){mPGeneratorVector = list;}
    void setCombinerVectorPointer(std::vector<MFNoiseCombinor*> *list){mPCombinerVector = list;}
    void setModifierVectorPointer(std::vector<MFNoiseModifier*> *list){mPModifierVector = list;}
    void setSelectorVectorPointer(std::vector<MFNoiseSelector*> *list){mPSelectorVector = list;}
    void setTransformerVectorPointer(std::vector<MFNoiseTransformer*> *list){mPTransformerVector = list;}
    void updateInputLists();
    QString getName(){return mName;}
    int getIndexID(){return mIndexID;}

    noise::module::Module*  getActiveIput1Module();
    noise::module::Module*  getActiveIput2Module();

    //myID is used to check if the calling module is used as input
    noise::module::Module* getSourceModule(noise::module::Module *myID);
    double getNoiseValue(double x = 0.0, double y = 0.0, double z = 0.0);

private slots:
    void on_comboBoxCombiner_currentIndexChanged(int index);
    void on_comboBoxType1_currentIndexChanged(int index);
    void on_comboBoxType2_currentIndexChanged(int index);

    void on_listWidgetInput1_activated(const QModelIndex &index);
    void on_listWidgetInput2_activated(const QModelIndex &index);

    void on_sliderInput1_sliderMoved(int position);

    void on_sliderInput2_sliderMoved(int position);

    void on_sliderOutput_sliderMoved(int position);

    void on_buttonView_clicked();

    void slotMapCalculated();
    void updateDiagrams();

private:
    bool readInputValues();
    bool readFromLineEdit(QStringList *data, QLineEdit *lineEdit);
    //diese funktion prüft rekursiv alle inputs von pointer, um herrauszufinden, ob pointer bereits als input verwendet wird.
    //falls pointer bereits als input verwendet wird, wird false zurückgegeben
    bool checkForSameInputPointer(noise::module::Module *pointerToCheck, noise::module::Module *srcModule);
    bool checkVectorPointers();
    void drawXYDiagram(MFXYDiagramm *diagram, double max,noise::module::Module *module);
    void setInputModule(int index, noise::module::Module *input);

    Ui::MFNoiseCombinor *ui;
    QString mName;
    int mIndexID;
    int m_type1Index = -1, m_type2Index = -1;
    std::vector<MFNoiseGenerator*> *mPGeneratorVector;
    std::vector<MFNoiseCombinor*> *mPCombinerVector;
    std::vector<MFNoiseModifier*> *mPModifierVector;
    std::vector<MFNoiseSelector*> *mPSelectorVector;
    std::vector<MFNoiseTransformer*> *mPTransformerVector;
    noise::module::Cache *mOutputCache;
    noise::module::Module *mActiveModuleInput1;
    noise::module::Module *mActiveModuleInput2;
    noise::module::Module *mActiveModule;
    MFXYDiagramm mInput1Diag;
    MFXYDiagramm mInput2Diag;
    MFXYDiagramm mOutputDiag;
    noise::module::Add mCombinerAdd;
    noise::module::Max mCombinerMax;
    noise::module::Min mCombinerMin;
    noise::module::Multiply mCombinerMultiply;
    noise::module::Power mCombinerPower;

    //color
    MFColorSettings *mColorSettings;

    //map
    MFSignalCtrl *mSigCtl;
    MFThreadNoise* mMapThread;
    MFMapGenerator* mMapGen;
    MF_struct_MapConfiguration mMapConfig;
};

#endif // MF_NOISE_COMBINOR_H
