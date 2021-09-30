#include "mf_noise_transformer.h"
#include "ui_mf_noise_transformer.h"

MFNoiseTransformer::MFNoiseTransformer(int indexID, QString name, QWidget *parent) :
    QWidget(parent),
    ui(new Ui::MFNoiseTransformer),
    mTransformerDisplace(),
    mTransformerRotatePoint(),
    mTransformerScalePoint(),
    mTransformerTranslatePoint(),
    mTransformerTurbulence()


{
    ui->setupUi(this);
    this->setParent(parent);
    mName = name;
    mIndexID = indexID;
    mPGeneratorVector   =0;
    mPCombinerVector    =0;
    mPModifierVector    =0;
    mPSelectorVector    =0;
    mPTransformerVector =0;

    mMapGen = 0;
    mSigCtl = new MFSignalCtrl();
    mMapThread = new MFThreadNoise(mSigCtl,mMapGen,this);
    mMapThread->start();

    mColorSettings=new MFColorSettings();


    //Init diagrams
    mInput1Diag = new MFXYDiagramm(this);
    mOutputDiag = new MFXYDiagramm(this);
    mDisplacementDiag = new MFXYDiagramm(this);
    mInput1Diag->setDiagramTitel(tr("Input"));
    mInput1Diag->setMinimumHeight(150);
    mInput1Diag->setMinimumWidth(300);
    mInput1Diag->update();
    mDisplacementDiag->setDiagramTitel(tr("Disp. X,Y,Z"));
    mDisplacementDiag->setMinimumHeight(150);
    mDisplacementDiag->setMinimumWidth(300);
    mDisplacementDiag->update();
    mOutputDiag->setDiagramTitel(tr("Output"));
    mOutputDiag->setMinimumHeight(150);
    mOutputDiag->setMinimumWidth(300);

    //init noise objects
    mDummyIn1 = new noise::module::Checkerboard();
    mDummyIn2 = new noise::module::Cylinders();
    mDummyIn3 = new noise::module::Perlin();
    mDummyIn4 = new noise::module::Const();
    mDummyOut = new noise::module::Perlin();

    mOutputCache = new noise::module::Cache();//will be used as input of other modules
    mActiveModuleInput1 = mDummyIn1;//will be set to the source module of cache
    mActiveModuleDisp1 = mDummyIn2;
    mActiveModuleDisp2 = mDummyIn3;
    mActiveModuleDisp3 = mDummyIn4;
    setInputModule(mActiveModuleInput1);//in1
    setDispControlModules();//displacements from 1-3
    mOutputCache->SetSourceModule(0,mTransformerDisplace);



    //init some other ui things

    ui->verticalLayoutColors->addWidget(mColorSettings);
    mColorSettings->adjustSize();
    mColorSettings->show();
    ui->comboBoxTransformerType->blockSignals(true);
    ui->comboBoxTransformerType->setCurrentIndex(-1);
    ui->comboBoxTransformerType->blockSignals(false);
    ui->lineEditName->setText(mName);
    ui->layoutVInputNoise->insertWidget(1,mInput1Diag);
    ui->layoutVInputNoise->insertWidget(2,mDisplacementDiag);
    ui->layoutVOut->insertWidget(1,mOutputDiag);



    //Map Settings
    QRegExp regExp("[1-9][0-9]{0,2}[,][1-9][0-9]{0,2}");
    ui->sizeXY->setText("10,10");
    ui->ResolutionXY->setText("70,70");
    ui->posXY->setText("0,0");
    ui->sizeXY->setValidator(new QRegExpValidator(regExp,this));
    ui->ResolutionXY->setValidator(new QRegExpValidator(regExp,this));
    ui->ResolutionXY->setValidator(new QRegExpValidator(regExp,this));

    connect(mSigCtl,SIGNAL(mapCalculated()),this,SLOT(slotMapCalculated()),Qt::QueuedConnection);
    connect(mColorSettings,SIGNAL(colorListChanged()),this,SLOT(updateDiagrams()));

    ui->comboBoxType1->setEnabled(false);
    ui->comboBoxDisp1->setEnabled(false);
    ui->comboBoxDisp2->setEnabled(false);
    ui->comboBoxDisp3->setEnabled(false);
}
MFNoiseTransformer::~MFNoiseTransformer()
{
    delete ui;
}
void MFNoiseTransformer::updateDiagrams(){
    drawXYDiagram(mOutputDiag,ui->sliderOutput->value(),mOutputCache);
    drawXYDiagram(mInput1Diag,ui->sliderSelector->value(),mActiveModuleInput1);
    drawXYDiagram(mDisplacementDiag,ui->sliderSelector->value(),mActiveModuleDisp1);
}

void MFNoiseTransformer::slotMapCalculated(){
    mMapThread->addTask(TASK_VIEW_MAP);
}

void MFNoiseTransformer::on_comboBoxTransformerType_currentIndexChanged(int index)
{
    unMakeDisplaceWidgets();
    switch(index){
    case INDEX_DISPLACEMENT:
        mOutputCache->SetSourceModule(0,mTransformerDisplace);
        makeDisplaceWidgets();
        ui->comboBoxDisp1->setEnabled(true);
        ui->comboBoxDisp2->setEnabled(true);
        ui->comboBoxDisp3->setEnabled(true);
        break;
    case INDEX_ROTATEPOINT:
        mOutputCache->SetSourceModule(0,mTransformerRotatePoint);
        makeRotateWidgets();
        break;
    case INDEX_SCALEPOINT:
        mOutputCache->SetSourceModule(0,mTransformerScalePoint);
        makeScalePointWidgets();
        break;
    case INDEX_TRANSLATEPOINT:
        mOutputCache->SetSourceModule(0,mTransformerTranslatePoint);
        makeTranslatePointWidgets();
        break;
    case INDEX_TURBULENCE:
        mOutputCache->SetSourceModule(0,mTransformerTurbulence);
        makeTurbulenceWidgets();
        break;
    default:
        mOutputCache->SetSourceModule(0,mTransformerDisplace);
        makeDisplaceWidgets();
        break;
    }

    drawXYDiagram(mOutputDiag,double(ui->sliderOutput->value()),mOutputCache);
    ui->comboBoxType1->setEnabled(true);
}
void MFNoiseTransformer::makeTurbulenceWidgets(){
    QLabel *label1= new QLabel(tr("Frequency:"));
    QLabel *label2= new QLabel(tr("Power:"));
    QLabel *label3= new QLabel(tr("Roughness:"));
    QLabel *label4= new QLabel(tr("Seed:"));
    QDoubleSpinBox *sb1 = new QDoubleSpinBox();
    QDoubleSpinBox *sb2 = new QDoubleSpinBox();
    QSpinBox *sb3 = new QSpinBox();
    QSpinBox *sb4 = new QSpinBox();
    sb1->setRange(-10000,10000);
    sb2->setRange(-10000,10000);
    sb3->setRange(-10000,10000);
    sb4->setRange(-10000,10000);
    sb1->setSingleStep(0.1);
    sb2->setSingleStep(0.1);
    sb1->setValue(mTransformerTurbulence.GetFrequency());
    sb2->setValue(mTransformerTurbulence.GetPower());
    sb3->setValue(mTransformerTurbulence.GetRoughnessCount());
    sb3->setValue(mTransformerTurbulence.GetSeed());
    ui->groupBoxSettings->layout()->addWidget(label1);
    ui->groupBoxSettings->layout()->addWidget(sb1);
    ui->groupBoxSettings->layout()->addWidget(label2);
    ui->groupBoxSettings->layout()->addWidget(sb2);
    ui->groupBoxSettings->layout()->addWidget(label3);
    ui->groupBoxSettings->layout()->addWidget(sb3);
    ui->groupBoxSettings->layout()->addWidget(label4);
    ui->groupBoxSettings->layout()->addWidget(sb4);
    connect(sb1,SIGNAL(valueChanged(double)),this,SLOT(slotFrequencyChanged(double)));
    connect(sb2,SIGNAL(valueChanged(double)),this,SLOT(slotPowerChanged(double)));
    connect(sb3,SIGNAL(valueChanged(int)),this,SLOT(slotRoughnessChanged(int)));
    connect(sb4,SIGNAL(valueChanged(int)),this,SLOT(slotSeedChanged(int)));
    connect(ui->comboBoxTransformerType,SIGNAL(currentIndexChanged(int)),sb1,SLOT(deleteLater()));
    connect(ui->comboBoxTransformerType,SIGNAL(currentIndexChanged(int)),sb2,SLOT(deleteLater()));
    connect(ui->comboBoxTransformerType,SIGNAL(currentIndexChanged(int)),sb3,SLOT(deleteLater()));
    connect(ui->comboBoxTransformerType,SIGNAL(currentIndexChanged(int)),label1,SLOT(deleteLater()));
    connect(ui->comboBoxTransformerType,SIGNAL(currentIndexChanged(int)),label2,SLOT(deleteLater()));
    connect(ui->comboBoxTransformerType,SIGNAL(currentIndexChanged(int)),label3,SLOT(deleteLater()));
}
void MFNoiseTransformer::slotFrequencyChanged(double val){
    mTransformerTurbulence.SetFrequency(val);
    drawXYDiagram(mOutputDiag,double(ui->sliderOutput->value()),mOutputCache);
}
void MFNoiseTransformer::slotPowerChanged(double val){
    mTransformerTurbulence.SetPower(val);
    drawXYDiagram(mOutputDiag,double(ui->sliderOutput->value()),mOutputCache);
}
void MFNoiseTransformer::slotRoughnessChanged(int val){
    mTransformerTurbulence.SetRoughness(val);
    drawXYDiagram(mOutputDiag,double(ui->sliderOutput->value()),mOutputCache);
}
void MFNoiseTransformer::slotSeedChanged(int val){
    mTransformerTurbulence.SetSeed(val);
    drawXYDiagram(mOutputDiag,double(ui->sliderOutput->value()),mOutputCache);
}

void MFNoiseTransformer::makeTranslatePointWidgets(){
    QLabel *label1= new QLabel(tr("x-Translation:"));
    QLabel *label2= new QLabel(tr("y-Translation:"));
    QLabel *label3= new QLabel(tr("z-Translation:"));
    QDoubleSpinBox *sb1 = new QDoubleSpinBox();
    QDoubleSpinBox *sb2 = new QDoubleSpinBox();
    QDoubleSpinBox *sb3 = new QDoubleSpinBox();
    sb1->setRange(-10000,10000);
    sb2->setRange(-10000,10000);
    sb3->setRange(-10000,10000);
    sb1->setSingleStep(0.1);
    sb2->setSingleStep(0.1);
    sb3->setSingleStep(0.1);
    sb1->setValue(mTransformerTranslatePoint.GetXTranslation());
    sb2->setValue(mTransformerTranslatePoint.GetYTranslation());
    sb3->setValue(mTransformerTranslatePoint.GetZTranslation());
    ui->groupBoxSettings->layout()->addWidget(label1);
    ui->groupBoxSettings->layout()->addWidget(sb1);
    ui->groupBoxSettings->layout()->addWidget(label2);
    ui->groupBoxSettings->layout()->addWidget(sb2);
    ui->groupBoxSettings->layout()->addWidget(label3);
    ui->groupBoxSettings->layout()->addWidget(sb3);
    connect(sb1,SIGNAL(valueChanged(double)),this,SLOT(slotXTransChanged(double)));
    connect(sb2,SIGNAL(valueChanged(double)),this,SLOT(slotYTransChanged(double)));
    connect(sb3,SIGNAL(valueChanged(double)),this,SLOT(slotZTransChanged(double)));
    connect(ui->comboBoxTransformerType,SIGNAL(currentIndexChanged(int)),sb1,SLOT(deleteLater()));
    connect(ui->comboBoxTransformerType,SIGNAL(currentIndexChanged(int)),sb2,SLOT(deleteLater()));
    connect(ui->comboBoxTransformerType,SIGNAL(currentIndexChanged(int)),sb3,SLOT(deleteLater()));
    connect(ui->comboBoxTransformerType,SIGNAL(currentIndexChanged(int)),label1,SLOT(deleteLater()));
    connect(ui->comboBoxTransformerType,SIGNAL(currentIndexChanged(int)),label2,SLOT(deleteLater()));
    connect(ui->comboBoxTransformerType,SIGNAL(currentIndexChanged(int)),label3,SLOT(deleteLater()));
}
void MFNoiseTransformer::slotXTransChanged(double trans){
    mTransformerTranslatePoint.SetXTranslation(trans);
    drawXYDiagram(mOutputDiag,double(ui->sliderOutput->value()),mOutputCache);
}
void MFNoiseTransformer::slotYTransChanged(double trans){
    mTransformerTranslatePoint.SetYTranslation(trans);
    drawXYDiagram(mOutputDiag,double(ui->sliderOutput->value()),mOutputCache);
}
void MFNoiseTransformer::slotZTransChanged(double trans){
    mTransformerTranslatePoint.SetZTranslation(trans);
    drawXYDiagram(mOutputDiag,double(ui->sliderOutput->value()),mOutputCache);
}

void MFNoiseTransformer::makeScalePointWidgets(){
    QLabel *label1= new QLabel(tr("x-Scale:"));
    QLabel *label2= new QLabel(tr("y-Scale:"));
    QLabel *label3= new QLabel(tr("z-Scale:"));
    QDoubleSpinBox *sb1 = new QDoubleSpinBox();
    QDoubleSpinBox *sb2 = new QDoubleSpinBox();
    QDoubleSpinBox *sb3 = new QDoubleSpinBox();
    sb1->setRange(-10000,10000);
    sb2->setRange(-10000,10000);
    sb3->setRange(-10000,10000);
    sb1->setSingleStep(0.1);
    sb2->setSingleStep(0.1);
    sb3->setSingleStep(0.1);
    sb1->setValue(mTransformerScalePoint.GetXScale());
    sb2->setValue(mTransformerScalePoint.GetYScale());
    sb3->setValue(mTransformerScalePoint.GetZScale());
    ui->groupBoxSettings->layout()->addWidget(label1);
    ui->groupBoxSettings->layout()->addWidget(sb1);
    ui->groupBoxSettings->layout()->addWidget(label2);
    ui->groupBoxSettings->layout()->addWidget(sb2);
    ui->groupBoxSettings->layout()->addWidget(label3);
    ui->groupBoxSettings->layout()->addWidget(sb3);
    connect(sb1,SIGNAL(valueChanged(double)),this,SLOT(slotXScaleChanged(double)));
    connect(sb2,SIGNAL(valueChanged(double)),this,SLOT(slotYScaleChanged(double)));
    connect(sb3,SIGNAL(valueChanged(double)),this,SLOT(slotZScaleChanged(double)));
    connect(ui->comboBoxTransformerType,SIGNAL(currentIndexChanged(int)),sb1,SLOT(deleteLater()));
    connect(ui->comboBoxTransformerType,SIGNAL(currentIndexChanged(int)),sb2,SLOT(deleteLater()));
    connect(ui->comboBoxTransformerType,SIGNAL(currentIndexChanged(int)),sb3,SLOT(deleteLater()));
    connect(ui->comboBoxTransformerType,SIGNAL(currentIndexChanged(int)),label1,SLOT(deleteLater()));
    connect(ui->comboBoxTransformerType,SIGNAL(currentIndexChanged(int)),label2,SLOT(deleteLater()));
    connect(ui->comboBoxTransformerType,SIGNAL(currentIndexChanged(int)),label3,SLOT(deleteLater()));
}
void MFNoiseTransformer::slotXScaleChanged(double scale){
    mTransformerScalePoint.SetXScale(scale);
    drawXYDiagram(mOutputDiag,double(ui->sliderOutput->value()),mOutputCache);
}
void MFNoiseTransformer::slotYScaleChanged(double scale){
    mTransformerScalePoint.SetYScale(scale);
    drawXYDiagram(mOutputDiag,double(ui->sliderOutput->value()),mOutputCache);
}
void MFNoiseTransformer::slotZScaleChanged(double scale){
    mTransformerScalePoint.SetZScale(scale);
    drawXYDiagram(mOutputDiag,double(ui->sliderOutput->value()),mOutputCache);
}

void MFNoiseTransformer::makeRotateWidgets(){
    QLabel *label1= new QLabel(tr("x-Angle:"));
    QLabel *label2= new QLabel(tr("y-Angle:"));
    QLabel *label3= new QLabel(tr("z-Angle:"));
    QDoubleSpinBox *sb1 = new QDoubleSpinBox();
    QDoubleSpinBox *sb2 = new QDoubleSpinBox();
    QDoubleSpinBox *sb3 = new QDoubleSpinBox();
    sb1->setRange(0,M_PI*2);
    sb2->setRange(0,M_PI*2);
    sb3->setRange(0,M_PI*2);
    sb1->setSingleStep(0.15);
    sb2->setSingleStep(0.15);
    sb3->setSingleStep(0.15);
    sb1->setValue(mTransformerRotatePoint.GetXAngle());
    sb2->setValue(mTransformerRotatePoint.GetYAngle());
    sb3->setValue(mTransformerRotatePoint.GetZAngle());
    ui->groupBoxSettings->layout()->addWidget(label1);
    ui->groupBoxSettings->layout()->addWidget(sb1);
    ui->groupBoxSettings->layout()->addWidget(label2);
    ui->groupBoxSettings->layout()->addWidget(sb2);
    ui->groupBoxSettings->layout()->addWidget(label3);
    ui->groupBoxSettings->layout()->addWidget(sb3);
    connect(sb1,SIGNAL(valueChanged(double)),this,SLOT(slotXAngleChanged(double)));
    connect(sb2,SIGNAL(valueChanged(double)),this,SLOT(slotYAngleChanged(double)));
    connect(sb3,SIGNAL(valueChanged(double)),this,SLOT(slotZAngleChanged(double)));
    connect(ui->comboBoxTransformerType,SIGNAL(currentIndexChanged(int)),sb1,SLOT(deleteLater()));
    connect(ui->comboBoxTransformerType,SIGNAL(currentIndexChanged(int)),sb2,SLOT(deleteLater()));
    connect(ui->comboBoxTransformerType,SIGNAL(currentIndexChanged(int)),sb3,SLOT(deleteLater()));
    connect(ui->comboBoxTransformerType,SIGNAL(currentIndexChanged(int)),label1,SLOT(deleteLater()));
    connect(ui->comboBoxTransformerType,SIGNAL(currentIndexChanged(int)),label2,SLOT(deleteLater()));
    connect(ui->comboBoxTransformerType,SIGNAL(currentIndexChanged(int)),label3,SLOT(deleteLater()));
}
void MFNoiseTransformer::slotXAngleChanged(double angle){
    mTransformerRotatePoint.SetXAngle(angle);
    drawXYDiagram(mOutputDiag,double(ui->sliderOutput->value()),mOutputCache);
}
void MFNoiseTransformer::slotYAngleChanged(double angle){
    mTransformerRotatePoint.SetYAngle(angle);
    drawXYDiagram(mOutputDiag,double(ui->sliderOutput->value()),mOutputCache);
}
void MFNoiseTransformer::slotZAngleChanged(double angle){
    mTransformerRotatePoint.SetZAngle(angle);
    drawXYDiagram(mOutputDiag,double(ui->sliderOutput->value()),mOutputCache);
}

void MFNoiseTransformer::unMakeDisplaceWidgets(){
    ui->groupBoxDispX->setVisible(false);
    ui->groupBoxDispY->setVisible(false);
    ui->groupBoxDispZ->setVisible(false);
    mDisplacementDiag->setVisible(false);
}

void MFNoiseTransformer::makeDisplaceWidgets(){
    ui->groupBoxDispX->setVisible(true);
    ui->groupBoxDispY->setVisible(true);
    ui->groupBoxDispZ->setVisible(true);
    mDisplacementDiag->setVisible(true);
}

void MFNoiseTransformer::on_comboBoxType1_currentIndexChanged(int index)
{
    //change the active list widget to configure it
    m_type1Index = index;
    if(!checkVectorPointers())return;
    switch(index){
    case INDEX_GENERATOR:
        ui->listWidgetInput1->clear();
        for(uint i =0;i<mPGeneratorVector[0].size();i++)
            ui->listWidgetInput1->addItem(mPGeneratorVector[0][i]->getName());
        break;
    case INDEX_COMBINER:
        ui->listWidgetInput1->clear();
        for(uint i =0;i<mPCombinerVector[0].size();i++)
            ui->listWidgetInput1->addItem(mPCombinerVector[0][i]->getName());
        break;
    case INDEX_MODIFIER:
        ui->listWidgetInput1->clear();
        for(uint i =0;i<mPModifierVector[0].size();i++)
            ui->listWidgetInput1->addItem(mPModifierVector[0][i]->getName());
        break;
    case INDEX_SELECTOR:
        ui->listWidgetInput1->clear();
        for(uint i =0;i<mPSelectorVector[0].size();i++)
            ui->listWidgetInput1->addItem(mPSelectorVector[0][i]->getName());
        break;
    case INDEX_TRANSFORMER:
        ui->listWidgetInput1->clear();
        for(uint i =0;i<mPTransformerVector[0].size();i++)
            ui->listWidgetInput1->addItem(mPTransformerVector[0][i]->getName());
        break;

    default:
        break;
    }
}
void MFNoiseTransformer::on_comboBoxDisp1_currentIndexChanged(int index)
{
    m_type1Index = index;
    if(!checkVectorPointers())return;
    switch(index){
    case INDEX_GENERATOR:
        ui->listWidgetXDisp->clear();
        for(uint i =0;i<mPGeneratorVector[0].size();i++)
            ui->listWidgetXDisp->addItem(mPGeneratorVector[0][i]->getName());
        break;
    case INDEX_COMBINER:
        ui->listWidgetXDisp->clear();
        for(uint i =0;i<mPCombinerVector[0].size();i++)
            ui->listWidgetXDisp->addItem(mPCombinerVector[0][i]->getName());
        break;
    case INDEX_MODIFIER:
        ui->listWidgetXDisp->clear();
        for(uint i =0;i<mPModifierVector[0].size();i++)
            ui->listWidgetXDisp->addItem(mPModifierVector[0][i]->getName());
        break;
    case INDEX_SELECTOR:
        ui->listWidgetXDisp->clear();
        for(uint i =0;i<mPSelectorVector[0].size();i++)
            ui->listWidgetXDisp->addItem(mPSelectorVector[0][i]->getName());
        break;
    case INDEX_TRANSFORMER:
        ui->listWidgetXDisp->clear();
        for(uint i =0;i<mPTransformerVector[0].size();i++)
            ui->listWidgetXDisp->addItem(mPTransformerVector[0][i]->getName());
        break;

    default:
        break;
    }
}

void MFNoiseTransformer::on_comboBoxDisp2_currentIndexChanged(int index)
{
    m_type1Index = index;
    if(!checkVectorPointers())return;
    switch(index){
    case INDEX_GENERATOR:
        ui->listWidgetYDisp->clear();
        for(uint i =0;i<mPGeneratorVector[0].size();i++)
            ui->listWidgetYDisp->addItem(mPGeneratorVector[0][i]->getName());
        break;
    case INDEX_COMBINER:
        ui->listWidgetYDisp->clear();
        for(uint i =0;i<mPCombinerVector[0].size();i++)
            ui->listWidgetYDisp->addItem(mPCombinerVector[0][i]->getName());
        break;
    case INDEX_MODIFIER:
        ui->listWidgetYDisp->clear();
        for(uint i =0;i<mPModifierVector[0].size();i++)
            ui->listWidgetYDisp->addItem(mPModifierVector[0][i]->getName());
        break;
    case INDEX_SELECTOR:
        ui->listWidgetYDisp->clear();
        for(uint i =0;i<mPSelectorVector[0].size();i++)
            ui->listWidgetYDisp->addItem(mPSelectorVector[0][i]->getName());
        break;
    case INDEX_TRANSFORMER:
        ui->listWidgetYDisp->clear();
        for(uint i =0;i<mPTransformerVector[0].size();i++)
            ui->listWidgetYDisp->addItem(mPTransformerVector[0][i]->getName());
        break;

    default:
        break;
    }
}

void MFNoiseTransformer::on_comboBoxDisp3_currentIndexChanged(int index)
{
    m_type1Index = index;
    if(!checkVectorPointers())return;
    switch(index){
    case INDEX_GENERATOR:
        ui->listWidgetZDisp->clear();
        for(uint i =0;i<mPGeneratorVector[0].size();i++)
            ui->listWidgetZDisp->addItem(mPGeneratorVector[0][i]->getName());
        break;
    case INDEX_COMBINER:
        ui->listWidgetZDisp->clear();
        for(uint i =0;i<mPCombinerVector[0].size();i++)
            ui->listWidgetZDisp->addItem(mPCombinerVector[0][i]->getName());
        break;
    case INDEX_MODIFIER:
        ui->listWidgetZDisp->clear();
        for(uint i =0;i<mPModifierVector[0].size();i++)
            ui->listWidgetZDisp->addItem(mPModifierVector[0][i]->getName());
        break;
    case INDEX_SELECTOR:
        ui->listWidgetZDisp->clear();
        for(uint i =0;i<mPSelectorVector[0].size();i++)
            ui->listWidgetZDisp->addItem(mPSelectorVector[0][i]->getName());
        break;
    case INDEX_TRANSFORMER:
        ui->listWidgetZDisp->clear();
        for(uint i =0;i<mPTransformerVector[0].size();i++)
            ui->listWidgetZDisp->addItem(mPTransformerVector[0][i]->getName());
        break;

    default:
        break;
    }
}
bool MFNoiseTransformer::checkVectorPointers(){
    if(mPGeneratorVector   == 0||
            mPCombinerVector    == 0||
            mPModifierVector    == 0||
            mPSelectorVector    == 0||
            mPTransformerVector == 0)
        return false;
    return true;
}

void MFNoiseTransformer::on_listWidgetInput1_activated(const QModelIndex &index)
{
    int noiseType = ui->comboBoxType1->currentIndex();
    int listRow = ui->listWidgetInput1->currentRow();
    mActiveModuleInput1 = getActiveIputModule(noiseType,listRow);
    bool check = checkForSameInputPointer(mActiveModuleInput1,mActiveModuleDisp1);
    check = (check||checkForSameInputPointer(mActiveModuleInput1,mActiveModuleDisp2));
    check = (check||checkForSameInputPointer(mActiveModuleInput1,mActiveModuleDisp3));
    if(    (mActiveModuleInput1 == mOutputCache)
        || (mActiveModuleInput1 == mActiveModuleDisp1)
        || (mActiveModuleInput1 == mActiveModuleDisp2)
        || (mActiveModuleInput1 == mActiveModuleDisp3)
        ||  mActiveModuleInput1 == NULL)
    {
        mActiveModuleInput1 = mDummyIn1;
        ui->listWidgetInput1->item(index.row())->setBackgroundColor(QColor(255,100,100));
        ui->listWidgetInput1->item(index.row())->setSelected(false);
        return;
    }
    ui->listWidgetInput1->item(index.row())->setBackgroundColor(QColor(255,255,255));
    //draw the input1 diagram
    setInputModule(mActiveModuleInput1);
    updateDiagrams();
}
noise::module::Module* MFNoiseTransformer::getActiveIputModule(int noiseType,int listRow){
    //returns the marked module in the input list
    switch(noiseType){
    case INDEX_GENERATOR:
        return mPGeneratorVector[0][listRow]->getSourceModule();
    case INDEX_COMBINER:
        return mPCombinerVector[0][listRow]->getSourceModule(mOutputCache);
    case INDEX_MODIFIER:
        return mPModifierVector[0][listRow]->getSourceModule(mOutputCache);
    case INDEX_SELECTOR:
        return mPSelectorVector[0][listRow]->getSourceModule(mOutputCache);
    case INDEX_TRANSFORMER:
        return mPTransformerVector[0][listRow]->getSourceModule(mOutputCache);
    }
}


void MFNoiseTransformer::on_listWidgetXDisp_activated(const QModelIndex &index)
{
    int noiseType = ui->comboBoxDisp1->currentIndex();
    int listRow = ui->listWidgetXDisp->currentRow();
    mActiveModuleDisp1 = getActiveIputModule(noiseType,listRow);
    bool check = checkForSameInputPointer(mActiveModuleDisp1,mActiveModuleInput1);
    check = (check||checkForSameInputPointer(mActiveModuleDisp1,mActiveModuleDisp2));
    check = (check||checkForSameInputPointer(mActiveModuleDisp1,mActiveModuleDisp3));
    if(    (mActiveModuleDisp1 == mOutputCache)
        || (mActiveModuleDisp1 == mActiveModuleInput1)
        || (mActiveModuleDisp1 == mActiveModuleDisp2)
        || (mActiveModuleDisp1 == mActiveModuleDisp3)
        ||  mActiveModuleDisp1 == NULL)
    {
        mActiveModuleDisp1 = mDummyIn2;
        ui->listWidgetXDisp->item(index.row())->setBackgroundColor(QColor(255,100,100));
        ui->listWidgetXDisp->item(index.row())->setSelected(false);
        return;
    }
    ui->listWidgetXDisp->item(index.row())->setBackgroundColor(QColor(255,255,255));
    //draw the input1 diagram
    setDispControlModules();
    updateDiagrams();
}

void MFNoiseTransformer::on_listWidgetYDisp_activated(const QModelIndex &index)
{
    int noiseType = ui->comboBoxDisp2->currentIndex();
    int listRow = ui->listWidgetYDisp->currentRow();
    mActiveModuleDisp2 = getActiveIputModule(noiseType,listRow);
    bool check = checkForSameInputPointer(mActiveModuleDisp2,mActiveModuleInput1);
    check = (check||checkForSameInputPointer(mActiveModuleDisp2,mActiveModuleDisp1));
    check = (check||checkForSameInputPointer(mActiveModuleDisp2,mActiveModuleDisp3));
    if(    (mActiveModuleDisp2 == mOutputCache)
        || (mActiveModuleDisp2 == mActiveModuleInput1)
        || (mActiveModuleDisp2 == mActiveModuleDisp1)
        || (mActiveModuleDisp2 == mActiveModuleDisp3)
        ||  mActiveModuleDisp2 == NULL)
    {
        mActiveModuleDisp2 = mDummyIn3;
        ui->listWidgetYDisp->item(index.row())->setBackgroundColor(QColor(255,100,100));
        ui->listWidgetYDisp->item(index.row())->setSelected(false);
        return;
    }
    ui->listWidgetYDisp->item(index.row())->setBackgroundColor(QColor(255,255,255));
    //draw the input1 diagram
    setDispControlModules();
    updateDiagrams();
}

void MFNoiseTransformer::on_listWidgetZDisp_activated(const QModelIndex &index)
{
    int noiseType = ui->comboBoxDisp3->currentIndex();
    int listRow = ui->listWidgetZDisp->currentRow();
    mActiveModuleDisp3 = getActiveIputModule(noiseType,listRow);
    bool check = checkForSameInputPointer(mActiveModuleDisp3,mActiveModuleInput1);
    check = (check||checkForSameInputPointer(mActiveModuleDisp3,mActiveModuleDisp1));
    check = (check||checkForSameInputPointer(mActiveModuleDisp3,mActiveModuleDisp2));
    if(    (mActiveModuleDisp3 == mOutputCache)
        || (mActiveModuleDisp3 == mActiveModuleInput1)
        || (mActiveModuleDisp3 == mActiveModuleDisp1)
        || (mActiveModuleDisp3 == mActiveModuleDisp2)
        ||  mActiveModuleDisp3 == NULL)
    {
        mActiveModuleDisp3 = mDummyIn4;
        ui->listWidgetZDisp->item(index.row())->setBackgroundColor(QColor(255,100,100));
        ui->listWidgetZDisp->item(index.row())->setSelected(false);
        return;
    }
    ui->listWidgetZDisp->item(index.row())->setBackgroundColor(QColor(255,255,255));
    //draw the input1 diagram
    setDispControlModules();
    updateDiagrams();
}
noise::module::Module* MFNoiseTransformer::getSourceModule(noise::module::Module *myID)
{
    //we have to check if the calling module is used as input module
    //if it is used as a input it will cause a crash
    //myId is staiing the same and
    bool checker = checkForSameInputPointer(myID,mOutputCache);

    if(checker){
        //we return NULL
        return NULL;
    }
    return (mOutputCache);
}
///////////////////////////////////////
bool MFNoiseTransformer::checkForSameInputPointer(noise::module::Module *pointerToCheck, noise::module::Module *srcModule){
    int inputCount = srcModule->GetSourceModuleCount();
    bool check = false;
    for(int i=0;i<inputCount;i++){
        noise::module::Module *inputSource =(noise::module::Module*)(&(srcModule->GetSourceModule(i)));
        noise::module::Module *originalOutput = pointerToCheck;
        if(inputSource == NULL)
            check = false;
        if(inputSource==originalOutput)
            return true;
        if(inputSource->GetSourceModuleCount()>0)
            check = checkForSameInputPointer(pointerToCheck,inputSource);
        if(check)
            return true;
    }
    return check;
}
void MFNoiseTransformer::drawXYDiagram(MFXYDiagramm *diagram, double max,noise::module::Module *module){
    std::vector<glm::vec3> map;
    double yValue;
    noise::module::Module *pModule = module;
    double xMax = max/10.0;
    double yMax = module->GetValue(0.0,0.0,0.0);
    double dispCounter = 0.0;
    for(double i = -xMax; i<xMax;i+=0.01)
    {
        yValue = pModule->GetValue(i,0,0);
        map.push_back(glm::vec3(i,yValue,dispCounter));//dispCounter will set the color
        if(abs(yValue)>yMax)
            yMax = abs(yValue);
        if( i >=(xMax-0.01) && (pModule == mActiveModuleDisp1)){
            dispCounter++;//dispCounter will increase if its a displace module
            i = -xMax;//reset i
            pModule = mActiveModuleDisp2;
        }
        else if(i >=(xMax-0.01) && (pModule == mActiveModuleDisp2)){
            dispCounter++;//dispCounter will increase if its a displace module
            i = -xMax;//reset i
            pModule = mActiveModuleDisp3;
        }
    }

    diagram->setPointVector(map,xMax,yMax);
    if(mColorSettings->gradientCount()){
        MFMapGenerator *colorGradGen = new MFMapGenerator();
        colorGradGen->setColorVector(mColorSettings->getColorVector());
        diagram->setColorGradientData(colorGradGen->getGradientColorVector());
        delete(colorGradGen);
    }
    diagram->setMinimumHeight(150);
    diagram->setMinimumWidth(300);
    diagram->update();
    diagram->show();
}
void MFNoiseTransformer::on_sliderSelector_sliderMoved(int position)
{
    drawXYDiagram(mInput1Diag,position,mActiveModuleInput1);
    drawXYDiagram(mDisplacementDiag,position,mActiveModuleDisp1);
}
void MFNoiseTransformer::on_sliderOutput_sliderMoved(int position)
{
    drawXYDiagram(mOutputDiag,position,(mOutputCache));
}

void MFNoiseTransformer::updateInputLists(){
    if(!checkVectorPointers())return;
    on_comboBoxDisp1_currentIndexChanged(ui->comboBoxDisp1->currentIndex());
    on_comboBoxDisp2_currentIndexChanged(ui->comboBoxDisp2->currentIndex());
    on_comboBoxDisp3_currentIndexChanged(ui->comboBoxDisp3->currentIndex());
    on_comboBoxType1_currentIndexChanged(ui->comboBoxType1->currentIndex());
}

void MFNoiseTransformer::setInputModule(noise::module::Module *input){
    mTransformerDisplace.SetSourceModule(0,*input);
    mTransformerRotatePoint.SetSourceModule(0,*input);
    mTransformerScalePoint.SetSourceModule(0,*input);
    mTransformerTranslatePoint.SetSourceModule(0,*input);
    mTransformerTurbulence.SetSourceModule(0,*input);
}
void MFNoiseTransformer::on_buttonView_clicked()
{
    if(!readInputValues())
    {
        return;
    }
    mMapThread->stop();
    mMapThread->clearTasks();
    if(mMapGen!=NULL)
        mMapGen->~MFMapGenerator();
    mMapGen = new MFMapGenerator();
    mMapThread->setMapGenerator(mMapGen);
    mMapGen->setColorVector(mColorSettings->getColorVector());
    mMapGen->setMapConfiguration(mMapConfig);
    noise::module::Module *pointerModule = (mOutputCache);
    mMapGen->setSourceModule(&pointerModule);

    //the NOISE tread will do the work
    mMapThread->addTask(TASK_GENERATE_MAP);
    mMapThread->continueWhile();
}
bool MFNoiseTransformer::readInputValues(){
    //sizeXY
    QStringList data(ui->sizeXY->text().split(","));

    if(!readFromLineEdit(&data,ui->sizeXY))
        return false;
    mMapConfig.sizeX=data.at(0).toInt();
    mMapConfig.sizeY=data.at(1).toInt();
    mMapConfig.sizeZ=0;
    //OctaveCount    mNoiseBillow->SetOctaveCount(0.0);

    //resolution
    if (!readFromLineEdit(&data,ui->ResolutionXY))
        return false;
    mMapConfig.resX=data.at(0).toInt();
    mMapConfig.resY=data.at(1).toInt();
    mMapConfig.resZ=1;

    //positions
    if (!readFromLineEdit(&data,ui->posXY))
        return false;
    mMapConfig.posX=data.at(0).toInt();
    mMapConfig.posY=data.at(1).toInt();
    mMapConfig.posZ=0;

    return true;
}
void MFNoiseTransformer::setDispControlModules(){
    mTransformerDisplace.SetXDisplaceModule(*mActiveModuleDisp1);
    mTransformerDisplace.SetYDisplaceModule(*mActiveModuleDisp2);
    mTransformerDisplace.SetZDisplaceModule(*mActiveModuleDisp3);
}
bool MFNoiseTransformer::readFromLineEdit(QStringList *data, QLineEdit *lineEdit){
    *data=lineEdit->text().split(",");
    if(data->size() < 2){
        //errMsg="no valid size values, you idiot (take sth like 1000,1000)";
        return false;
    }
    if(data->at(1).isEmpty()){
        lineEdit->setText(ui->sizeXY->text().append("1"));
        *data=lineEdit->text().split(",");
    }
    return true;
}


