#ifndef MF_NOISE_SETTINGS_H
#define MF_NOISE_SETTINGS_H

#include "mf_noise.h"
#include "mf_noise_generator.h"
#include "mf_noise_transformer.h"
#include "mf_noise_selector.h"
#include "mf_noise_combinor.h"
#include "mf_noise_modifier.h"
#include <stdio.h>
#include <QWidget>
#include <QListWidget>
#include <QListWidgetItem>
#include <QInputDialog>


namespace Ui {
class MFNoiseSettings;
}

class MFNoiseSettings : public QWidget
{
    Q_OBJECT

public:
    explicit MFNoiseSettings(QWidget *parent = 0);
    void makeGeneratorModule(QString text);
    void makeCombinorModule(QString text);
    void makeModifierModule(QString text);
    void makeSelectorModule(QString text);
    void makeTransformerModule(QString text);

    void deleteGeneratorModule();
    void deleteCombinorModule();
    void deleteModifierModule();
    void deleteSelectorModule();
    void deleteTransformerModule();
    ~MFNoiseSettings();


private slots:
    void on_btnNewNoise_clicked();
    void on_btnDelete_clicked();
    void on_btnSave_clicked();
    void on_btnOpen_clicked();

    void on_comboNoiseModule_currentIndexChanged(int index);

    void on_listModifier_activated(const QModelIndex &index);
    void on_listGenerator_activated(const QModelIndex &index);
    void on_listCombiner_activated(const QModelIndex &index);
    void on_listSelector_activated(const QModelIndex &index);

    void on_listTransformer_activated(const QModelIndex &index);

private:
    Ui::MFNoiseSettings *ui;

    void hideAll();
    //the items in the vector will be connected to the items in QListWidget
    //the connection will be tracked through the name of the QListWidgetItem "Number_NoiseName" first Element 0_bla ...
    std::vector<MFNoiseGenerator*> mNoiseListGenerator;
    std::vector<MFNoiseCombinor*> mNoiseListCombiner;
    std::vector<MFNoiseModifier*> mNoiseListModifier;
    std::vector<MFNoiseSelector*> mNoiseListSelector;
    std::vector<MFNoiseTransformer*> mNoiseListTransformer;

    //the active list widget is used to edit the active list (one out of five)
    //with this pointer theres no need for a long switch case code
    QListWidget                 *mPActiveWidgetList;//pointer to the active widget

    MFNoiseGenerator            *mNoiseGeneratorDummy;//for the initial widget
    MFNoiseGenerator            *mPActiveNoiseGenerator; //traching of the active noise edit
    MFNoiseCombinor             *mNoiseCombinerDummy;//for the initial widget
    MFNoiseCombinor             *mPActiveNoiseCombiner;//traching of the active combiner
    MFNoiseModifier             *mNoiseModifierDummy;//for the initial widget
    MFNoiseModifier             *mPActiveNoiseModifier;//traching of the active combiner
    MFNoiseSelector             *mNoiseSelectorDummy;//for the initial widget
    MFNoiseSelector             *mPActiveNoiseSelector;//traching of the active combiner
    MFNoiseTransformer          *mNoiseTransformerDummy;
    MFNoiseTransformer          *mPActiveNoiseTransformer;
};

#endif // MF_NOISE_SETTINGS_H
