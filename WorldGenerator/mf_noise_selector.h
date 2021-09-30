#ifndef MF_NOISE_SELECTOR_H
#define MF_NOISE_SELECTOR_H
#include "mf_noise_generator.h"
#include <QWidget>
#include <QListWidget>
#include <QMessageBox>
#include <noise/noise.h>
#include "mf_noise_combinor.h"
#include "mf_noise_generator.h"
#include "mf_noise_transformer.h"
#include "mf_xy_diagramm.h"

class MFNoiseCombinor;
class MFNoiseGenerator;
class MFNoiseModifier;
class MFNoiseTransformer;
namespace Ui {
class MFNoiseSelector;
}

class MFNoiseSelector : public QWidget
{
    Q_OBJECT

public:
    explicit MFNoiseSelector(int indexID,QString name,QWidget *parent=0);
    ~MFNoiseSelector();
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

    //returns a pointer to the input module
    noise::module::Module*  getActiveIput1Module();
    noise::module::Module*  getActiveIput2Module();
    noise::module::Module* getActiveSelectorModule();

private slots:
    void on_comboBoxSelectorType_currentIndexChanged(int index);
    void on_comboBoxType1_currentIndexChanged(int index);
    void on_comboBoxType2_currentIndexChanged(int index);
    void on_comboBoxSelectorModule_currentIndexChanged(int index);

    void on_listWidgetSelector_activated(const QModelIndex &index);
    void on_listWidgetInput1_activated(const QModelIndex &index);
    void on_listWidgetInput2_activated(const QModelIndex &index);

    void on_sliderSelector_sliderMoved(int position);
    void on_sliderOutput_sliderMoved(int position);

    void on_buttonView_clicked();

    void slotMapCalculated();
    void updateDiagrams();

    void slotLowerBoundChanged(double lower);
    void slotUpperBoundChanged(double upper);
    void slotEdgeFalloffChanged(double edgeFalloff);
private:
    void makeSelectorWidgets();
    bool readInputValues();
    bool readFromLineEdit(QStringList *data, QLineEdit *lineEdit);
    //diese funktion prüft rekursiv alle inputs von pointer, um herrauszufinden, ob pointer bereits als input verwendet wird.
    //falls pointer bereits als input verwendet wird, wird false zurückgegeben
    bool checkForSameInputPointer(noise::module::Module *pointerToCheck, noise::module::Module *srcModule);
    bool checkVectorPointers();
    void drawXYDiagram(MFXYDiagramm *diagram, double max,noise::module::Module *module);
    void setInputModule(int index, noise::module::Module *input);


    noise::module::Module *mActiveModuleOutput;
    Ui::MFNoiseSelector *ui;

    QString mName;
    int mIndexID;
    int m_type1Index = -1, m_type2Index = -1, m_typeSlector = -1;
    double mUpperBound = 1.0,mLowerBound = -1.0, mEdgeFalloff = 0.0;
    std::vector<MFNoiseGenerator*> *mPGeneratorVector;
    std::vector<MFNoiseCombinor*> *mPCombinerVector;
    std::vector<MFNoiseModifier*> *mPModifierVector;
    std::vector<MFNoiseSelector*> *mPSelectorVector;
    std::vector<MFNoiseTransformer*> *mPTransformerVector;

    noise::module::Cache *mOutputCache;
    noise::module::Module *mActiveModuleInput1;
    noise::module::Module *mActiveModuleInput2;
    noise::module::Module *mActiveSelector;

    MFXYDiagramm mInput1Diag;
    MFXYDiagramm mInput2Diag;
    MFXYDiagramm mSelectorDiag;
    MFXYDiagramm mOutputDiag;

    noise::module::Select mSelectorSelect;
    noise::module::Blend mSelectorBlend;

    //color
    MFColorSettings *mColorSettings;

    //map
    MFSignalCtrl *mSigCtl;
    MFThreadNoise* mMapThread;
    MFMapGenerator* mMapGen;
    MF_struct_MapConfiguration mMapConfig;
};

#endif // MF_NOISE_SELECTOR_H


