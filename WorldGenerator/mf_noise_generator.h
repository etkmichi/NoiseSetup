#ifndef MF_NOISE_GENERATOR_H
#define MF_NOISE_GENERATOR_H

#include <QWidget>
#include <QVBoxLayout>
#include <QGroupBox>
#include <QLineEdit>
#include <QSpinBox>
#include <QDoubleSpinBox>
#include <QComboBox>
#include <QListWidgetItem>

#include "noise/noise.h"
#include "helper/mf_def.h"
#include "mf_settings_color.h"
#include "mf_map_generator.h"
#include "mf_signal_ctrl.h"
#include "mf_thread_noise.h"
#include "mf_xy_diagramm.h"


namespace Ui {
class MFNoiseGenerator;
}

class MFNoiseGenerator : public QWidget
{
    Q_OBJECT
public:
    struct ms_generatorSettings{


        //map Settings
        int frequency,
        generatorType,
        lactunarity,
        constValue,
        persistence,
        displacement,
        seed,
        octaves,
        quality;
        std::string name;
        std::vector<glm::vec3> colors;
    };
    explicit MFNoiseGenerator(int ID, QString name, QWidget *parent = 0);
    ~MFNoiseGenerator();
    void setListItemTracker(QListWidgetItem *listItem);
    void setName(QString name){mName = name;}
    void setGeneratorSettings(ms_generatorSettings settings);
    ms_generatorSettings getGeneratorSettings();
    void setID(int ID){mID = ID;}
    noise::module::Module *getSourceModule(){return mActiveModule;}
    double getNoiseValue(double x = 0.0, double y = 0.0, double z = 0.0);
    int getID(){return mID;}
    QString getName(){return mName;}

signals:
    void okButtonClicked();
public slots:
    void drawXYDiagramm();

private slots:
    void on_buttonDone_clicked();
    void on_comboBoxNoiseType_currentIndexChanged(int index);

    void slotFrequencyChanged(int value);
    void slotLacunarityChanged(int value);
    void slotOctaveCountChanged(int value);
    void slotPersistenceChanged(int value);
    void slotSeedChanged(int value);
    void slotQualityChanged(int preDefine);
    void slotConstValueChanged(int value);
    void slotDisplacementChanged(int value);

    void slotMapCalculated();

    void on_buttonView_clicked();


    void on_sliderHRange_valueChanged(int value);



private:
    bool readInputValues();
    bool readFromLineEdit(QStringList *data, QLineEdit *lineEdit);
    void uiSetup();
    void makeBillowWidgets();
    void makeChessBrdWidgets();
    void makeConstWidgets();
    void makeCylinderWidgets();
    void makePerlinWidgets();
    void makeRidgedWidgets();
    void makeSpheresWidgets();
    void makeVoroniWidgets();

    void addConstValueWidgets();
    void addLacunarityWidgets();
    void addFrequencyWidgets();
    void addNoiseQualityWidgets();
    void addOctaveCountWidgets();
    void addPersistenceWidgets();
    void addSeedWidgets();
    void addSourceModuleWidgets();
    void addDisplacementWidgets();

    noise::module::Billow       *mNoiseBillow;
    noise::module::Checkerboard *mNoiseCheckerboard;
    noise::module::Const        *mNoiseConst;
    noise::module::Cylinders    *mNoiseCylinders;
    noise::module::Perlin       *mNoisePerlin;
    noise::module::RidgedMulti  *mNoiseRidgedMulti;
    noise::module::Spheres      *mNoiseSpheres;
    noise::module::Voronoi      *mNoiseVoronoi;

    QListWidgetItem *mListItemTracker=0;
    QVBoxLayout *layoutSettings=0;
    Ui::MFNoiseGenerator *ui=0;
    QString mName;

    //noise settings
    noise::module::Module       *mActiveModule;
    int mFrequency = 1000,
    mLactunarity = 2000,
    mConstValue=0,
    mPersistence=500,
    mDisplacement=1000,
    mSeed=0,
    mOctaveCount = 6;
    noise::NoiseQuality mQuality = noise::QUALITY_STD;
    int m_generatorType;

    //map
    MFXYDiagramm *mDiagramm;
    MFSignalCtrl *mSigCtl;
    MFThreadNoise* mMapThread;
    MFColorSettings *mColorSettings;
    MFMapGenerator* mMapGen;
    MF_struct_MapConfiguration mMapConfig;

    int mID;

};

#endif // MF_NOISE_GENERATOR_H
