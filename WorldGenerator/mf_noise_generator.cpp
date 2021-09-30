#include "mf_noise_generator.h"
#include "ui_mf_noise_generator.h"

MFNoiseGenerator::MFNoiseGenerator(int ID, QString name, QWidget *parent) :
    QWidget(parent),
    ui(new Ui::MFNoiseGenerator)
{
    ui->setupUi(this);

    mColorSettings=new MFColorSettings();
    mMapGen = 0;
    mSigCtl = new MFSignalCtrl();
    mMapThread = new MFThreadNoise(mSigCtl,mMapGen,this);
    mMapThread->start();

    mID = ID;
    mName = name;

    mDiagramm = new MFXYDiagramm(this);
    mNoiseBillow=new noise::module::Billow();
    mNoiseCheckerboard=new noise::module::Checkerboard();
    mNoiseConst=new noise::module::Const();
    mNoiseCylinders=new noise::module::Cylinders();
    mNoisePerlin=new noise::module::Perlin();
    mNoiseRidgedMulti=new noise::module::RidgedMulti();
    mNoiseSpheres=new noise::module::Spheres();
    mNoiseVoronoi=new noise::module::Voronoi();

    mActiveModule = mNoisePerlin;

    layoutSettings = new QVBoxLayout();
    connect(mSigCtl,SIGNAL(mapCalculated()),this,SLOT(slotMapCalculated()));
    connect(mColorSettings,SIGNAL(colorListChanged()),this,SLOT(drawXYDiagramm()));

    uiSetup();


}

double MFNoiseGenerator::getNoiseValue(double x, double y, double z){
    return mActiveModule->GetValue(x,y,z);
}

void MFNoiseGenerator::slotMapCalculated(){
    mMapThread->addTask(TASK_VIEW_MAP);
}

void MFNoiseGenerator::setListItemTracker(QListWidgetItem *listItem){
    mListItemTracker = listItem;
}

MFNoiseGenerator::~MFNoiseGenerator()
{
    delete mDiagramm;
    delete mNoiseBillow;
    delete mNoiseCheckerboard;
    delete mNoiseConst;
    delete mNoiseCylinders;
    delete mNoisePerlin;
    delete mNoiseRidgedMulti;
    delete mNoiseSpheres;
    delete mNoiseVoronoi;
    delete ui;
}
void MFNoiseGenerator::uiSetup(){
    ui->verticalLayoutColors->addWidget(mColorSettings);
    mColorSettings->adjustSize();
    mColorSettings->show();
    ui->sizeXY->setText("10,10");
    ui->ResolutionXY->setText("70,70");
    ui->posXY->setText("0,0");

    QRegExp regExp("[1-9][0-9]{0,2}[,][1-9][0-9]{0,2}");
    ui->sizeXY->setValidator(new QRegExpValidator(regExp,this));
    ui->ResolutionXY->setValidator(new QRegExpValidator(regExp,this));
    ui->ResolutionXY->setValidator(new QRegExpValidator(regExp,this));

    adjustSize();

    this->setFixedHeight(1024);
    ui->verticalLayoutSettings->insertLayout(ui->verticalLayoutSettings->count()-2,layoutSettings);

    ui->lineEditName->setText(mName);

    mDiagramm->setFixedHeight(200);
    this->layout()->addWidget(mDiagramm);

    ui->buttonView->setDisabled(false);
    ui->comboBoxNoiseType->setCurrentIndex(-1);
}
void MFNoiseGenerator::setGeneratorSettings(ms_generatorSettings settings){
    mName = QString::fromStdString(settings.name);
    mFrequency = settings.frequency;
    mLactunarity = settings.lactunarity;
    mDisplacement = settings.displacement;
    mConstValue = settings.constValue;
    mPersistence = settings.persistence;
    mSeed = settings.seed;
    mOctaveCount = settings.octaves;
    mQuality = noise::NoiseQuality(settings.quality);
    mColorSettings->setColorVector(settings.colors);

    switch (settings.generatorType) {
    case INDEX_BILLOW:
        ui->comboBoxNoiseType->setCurrentIndex(INDEX_BILLOW);
        break;
    case INDEX_CHESS:
        ui->comboBoxNoiseType->setCurrentIndex(INDEX_CHESS);
        break;
    case INDEX_CONST:
        ui->comboBoxNoiseType->setCurrentIndex(2);
        break;
    case INDEX_CYLINDERS:
        ui->comboBoxNoiseType->setCurrentIndex(3);
        break;
    case INDEX_PERLIN:
        ui->comboBoxNoiseType->setCurrentIndex(4);
        break;
    case INDEX_RIDGED:
        ui->comboBoxNoiseType->setCurrentIndex(5);
        break;
    case INDEX_SPHERES:
        ui->comboBoxNoiseType->setCurrentIndex(6);
        break;
    case INDEX_VORONI:
        ui->comboBoxNoiseType->setCurrentIndex(7);
        break;
    default:
        break;
    }
    ui->lineEditName->setText(mName);
//    slotFrequencyChanged(settings.frequency);
//    slotLacunarityChanged(settings.lactunarity);
//    slotDisplacementChanged(settings.displacement);
//    slotConstValueChanged(settings.constValue);
//    slotPersistenceChanged(settings.persistence);
//    slotSeedChanged(settings.seed);
//    slotOctaveCountChanged(settings.octaves);
//    slotQualityChanged(noise::NoiseQuality(settings.quality));
}
MFNoiseGenerator::ms_generatorSettings MFNoiseGenerator::getGeneratorSettings(){
    ms_generatorSettings settings;
    settings.name = mName.toStdString();
    settings.frequency = mFrequency;
    settings.lactunarity = mLactunarity;
    settings.displacement = mDisplacement;
    settings.constValue = mConstValue;
    settings.persistence = mPersistence;
    settings.seed = mSeed;
    settings.octaves = mOctaveCount;
    settings.quality = mQuality;
    settings.generatorType = m_generatorType;
    settings.colors = mColorSettings->getColorVector();
    return settings;
}

void MFNoiseGenerator::on_buttonDone_clicked()
{
    this->hide();
    mName = ui->lineEditName->text();
    if(mListItemTracker!= 0){
        QString idStr = mListItemTracker->text().split("_").at(0)+"_"+mName;
        mListItemTracker->setText(idStr);
    }
    emit okButtonClicked();
}

void MFNoiseGenerator::on_comboBoxNoiseType_currentIndexChanged(int index)
{
    switch(index)
    {
    case 0://makeBillowWidgets
        m_generatorType = 0;
        makeBillowWidgets();
        mActiveModule = mNoiseBillow;
        break;

    case 1://makeChessBrdWidgets
        m_generatorType = 1;
        makeChessBrdWidgets();
        mActiveModule = mNoiseCheckerboard;
        this->adjustSize();
        break;

    case 2://makeConstWidgets
        m_generatorType = 2;
        makeConstWidgets();
        mActiveModule = mNoiseConst;
        break;

    case 3://makeCylinderWidgets
        m_generatorType = 3;
        makeCylinderWidgets();
        mActiveModule = mNoiseCylinders;
        break;

    case 4://makePerlinWidgets
        m_generatorType = 4;
        makePerlinWidgets();
        mActiveModule = mNoisePerlin;
        break;

    case 5://makeRidgedWidgets
        m_generatorType = 5;
        makeRidgedWidgets();
        mActiveModule = mNoiseRidgedMulti;
        break;

    case 6://makeSpheresWidgets
        m_generatorType = 6;
        makeSpheresWidgets();
        mActiveModule = mNoiseSpheres;
        break;

    case 7://makeVoroniWidgets
        m_generatorType = 7;
        makeVoroniWidgets();
        mActiveModule = mNoiseVoronoi;
        break;
    default:
        break;
    }
    adjustSize();
    drawXYDiagramm();
}

void MFNoiseGenerator::makeBillowWidgets(){
    //frequency
    addFrequencyWidgets();
    //lacunarity make mNoiseBillow->SetLacunarity(double);
    addLacunarityWidgets();
    //Noise Quality mNoiseBillow->SetNoiseQuality(0.0);
    addNoiseQualityWidgets();
    //OctaveCount    mNoiseBillow->SetOctaveCount(0.0);
    addOctaveCountWidgets();
    addPersistenceWidgets();
    addSeedWidgets();
    addSourceModuleWidgets();
}
void MFNoiseGenerator::makeVoroniWidgets(){
    //frequency
    addFrequencyWidgets();
    addSeedWidgets();
    addSourceModuleWidgets();
    addDisplacementWidgets();
}
void MFNoiseGenerator::makePerlinWidgets(){
    //frequency
    addFrequencyWidgets();
    //lacunarity make mNoiseBillow->SetLacunarity(double);
    addLacunarityWidgets();
    //Noise Quality mNoiseBillow->SetNoiseQuality(0.0);
    addNoiseQualityWidgets();
    //OctaveCount    mNoiseBillow->SetOctaveCount(0.0);
    addOctaveCountWidgets();
    addPersistenceWidgets();
    addSeedWidgets();
    addSourceModuleWidgets();
}
void MFNoiseGenerator::makeRidgedWidgets(){
    //frequency
    addFrequencyWidgets();
    //lacunarity make mNoiseBillow->SetLacunarity(double);
    addLacunarityWidgets();
    //Noise Quality mNoiseBillow->SetNoiseQuality(0.0);
    addNoiseQualityWidgets();
    //OctaveCount    mNoiseBillow->SetOctaveCount(0.0);
    addOctaveCountWidgets();

    addSeedWidgets();
    addSourceModuleWidgets();
}
void MFNoiseGenerator::makeSpheresWidgets(){
    addFrequencyWidgets();
}
void MFNoiseGenerator::makeChessBrdWidgets(){
    QLabel *aLabel = new QLabel(tr("No settings"));
    layoutSettings->addWidget(aLabel);
    connect(ui->comboBoxNoiseType,SIGNAL(currentIndexChanged(int)),aLabel,SLOT(deleteLater()));
}
void MFNoiseGenerator::makeConstWidgets(){
    addConstValueWidgets();
}
void MFNoiseGenerator::makeCylinderWidgets(){
    addFrequencyWidgets();
}

void MFNoiseGenerator::addConstValueWidgets(){
    QGroupBox *groupB = new QGroupBox(tr("Set constant value"));
    QVBoxLayout *lay=new QVBoxLayout(groupB);
    QSpinBox *intIn = new QSpinBox(groupB);

    groupB->setFlat(false);
    lay->addWidget(intIn);
    intIn->setRange(-2000,2000);
    intIn->setValue(mConstValue);

    connect(intIn,SIGNAL(valueChanged(int)),this,SLOT(slotConstValueChanged(int)));
    connect(ui->comboBoxNoiseType,SIGNAL(currentIndexChanged(int)),groupB,SLOT(deleteLater()));

    layoutSettings->addWidget(groupB);
}

void MFNoiseGenerator::addLacunarityWidgets(){
    QGroupBox *groupB = new QGroupBox(tr("Set lacunarity:"));
    QVBoxLayout *lay=new QVBoxLayout(groupB);
    QSlider *slider= new QSlider(Qt::Horizontal,groupB);
    QSpinBox *valueView = new QSpinBox(groupB);
    valueView->setRange(MIN_LACUNARITY*1000,MAX_LACUNARITY*1000);
    slider->setRange(MIN_LACUNARITY*1000,MAX_LACUNARITY*1000);

    connect(slider,SIGNAL(valueChanged(int)),this,SLOT(slotLacunarityChanged(int)));
    connect(slider,SIGNAL(valueChanged(int)),valueView,SLOT(setValue(int)));
    connect(valueView,SIGNAL(valueChanged(int)),slider,SLOT(setValue(int)));
    connect(ui->comboBoxNoiseType,SIGNAL(currentIndexChanged(int)),groupB,SLOT(deleteLater()));

    groupB->setFlat(false);
    lay->addWidget(slider);
    lay->addWidget(valueView);

    slider->setSliderPosition((mLactunarity));

    layoutSettings->addWidget(groupB);
}

void MFNoiseGenerator::addNoiseQualityWidgets(){
    QGroupBox *groupB = new QGroupBox(tr("Set noise quality:"));
    QVBoxLayout *lay=new QVBoxLayout(groupB);
    QComboBox *combo=new QComboBox(groupB);

    connect(combo,SIGNAL(currentIndexChanged(int)),this,SLOT(slotQualityChanged(int)));
    connect(ui->comboBoxNoiseType,SIGNAL(currentIndexChanged(int)),groupB,SLOT(deleteLater()));

    groupB->setFlat(false);
    lay->addWidget(combo);
    combo->addItem(tr("QUALITY_FAST"));
    combo->addItem(tr("QUALITY_STD "));
    combo->addItem(tr("QUALITY_BEST"));
    combo->setCurrentIndex(1);
    layoutSettings->addWidget(groupB);
}

void MFNoiseGenerator::addOctaveCountWidgets(){
    QGroupBox *groupB = new QGroupBox(tr("Set octave count:"));
    QVBoxLayout *lay=new QVBoxLayout(groupB);
    QSpinBox *intIn = new QSpinBox(groupB);
    intIn->setRange(1,30);

    connect(intIn,SIGNAL(valueChanged(int)),this,SLOT(slotOctaveCountChanged(int)));
    connect(ui->comboBoxNoiseType,SIGNAL(currentIndexChanged(int)),groupB,SLOT(deleteLater()));

    lay->addWidget(intIn);
    intIn->setValue(mOctaveCount);

    layoutSettings->addWidget(groupB);
}

void MFNoiseGenerator::addPersistenceWidgets(){
    QGroupBox *groupB = new QGroupBox(tr("Set persistence:"));
    QVBoxLayout *lay=new QVBoxLayout(groupB);
    QSlider *slider= new QSlider(Qt::Horizontal,groupB);
    QSpinBox *valueView = new QSpinBox(groupB);
    valueView->setRange(0,2000);
    slider->setRange(0,2000);

    connect(slider,SIGNAL(valueChanged(int)),this,SLOT(slotPersistenceChanged(int)));
    connect(slider,SIGNAL(valueChanged(int)),valueView,SLOT(setValue(int)));
    connect(valueView,SIGNAL(valueChanged(int)),slider,SLOT(setValue(int)));
    connect(ui->comboBoxNoiseType,SIGNAL(currentIndexChanged(int)),groupB,SLOT(deleteLater()));

    groupB->setFlat(false);
    lay->addWidget(slider);
    lay->addWidget(valueView);

    slider->setSliderPosition(mPersistence);
    layoutSettings->addWidget(groupB);
}

void MFNoiseGenerator::addSeedWidgets(){
    QGroupBox *groupB = new QGroupBox(tr("Set Seed"));
    QVBoxLayout *lay=new QVBoxLayout(groupB);
    QSpinBox *seedIn = new QSpinBox(groupB);
    seedIn->setRange(0,0x7FFFFFFF);

    connect(seedIn,SIGNAL(valueChanged(int)),this,SLOT(slotSeedChanged(int)));
    connect(ui->comboBoxNoiseType,SIGNAL(currentIndexChanged(int)),groupB,SLOT(deleteLater()));

    groupB->setFlat(false);
    lay->addWidget(seedIn);

    seedIn->setValue(mSeed);
    layoutSettings->addWidget(groupB);
}

void MFNoiseGenerator::addSourceModuleWidgets(){
}

void MFNoiseGenerator::addFrequencyWidgets(){
    QGroupBox *groupB = new QGroupBox(tr("Set frequency:"));
    QVBoxLayout *lay=new QVBoxLayout(groupB);
    QSlider *slider= new QSlider(Qt::Horizontal,groupB);
    QSpinBox *valueView = new QSpinBox(groupB);
    slider->setRange(0,10000);
    valueView->setRange(1,10000);

    connect(slider,SIGNAL(valueChanged(int)),this,SLOT(slotFrequencyChanged(int)));
    connect(slider,SIGNAL(valueChanged(int)),valueView,SLOT(setValue(int)));
    connect(valueView,SIGNAL(valueChanged(int)),slider,SLOT(setValue(int)));
    connect(ui->comboBoxNoiseType,SIGNAL(currentIndexChanged(int)),groupB,SLOT(deleteLater()));

    groupB->setFlat(false);
    lay->addWidget(slider);
    lay->addWidget(valueView);
    slider->setSliderPosition(mFrequency);
    layoutSettings->addWidget(groupB);
}

void MFNoiseGenerator::addDisplacementWidgets(){
    QGroupBox *groupB = new QGroupBox(tr("Set displacement:"));
    QVBoxLayout *lay=new QVBoxLayout(groupB);
    QSlider *slider= new QSlider(Qt::Horizontal,groupB);
    QSpinBox *valueView = new QSpinBox(groupB);
    valueView->setRange(0,10000);
    slider->setRange(0,10000);

    connect(slider,SIGNAL(valueChanged(int)),this,SLOT(slotDisplacementChanged(int)));
    connect(slider,SIGNAL(valueChanged(int)),valueView,SLOT(setValue(int)));
    connect(valueView,SIGNAL(valueChanged(int)),slider,SLOT(setValue(int)));
    connect(ui->comboBoxNoiseType,SIGNAL(currentIndexChanged(int)),groupB,SLOT(deleteLater()));

    lay->addWidget(slider);
    lay->addWidget(valueView);
    slider->setSliderPosition(mDisplacement);
    layoutSettings->addWidget(groupB);
}

//frequency slot
void MFNoiseGenerator::slotFrequencyChanged(int value){
    mNoiseBillow->SetFrequency(1.0*value/1000.0);
    mNoisePerlin->SetFrequency(1.0*value/1000.0);
    mNoiseCylinders->SetFrequency(1.0*value/1000.0);
    mNoiseRidgedMulti->SetFrequency(1.0*value/1000.0);
    mNoiseSpheres->SetFrequency(1.0*value/1000.0);
    mNoiseVoronoi->SetFrequency(1.0*value/1000.0);
    mFrequency = value;
    drawXYDiagramm();
}
//lacunarity slot
void MFNoiseGenerator::slotLacunarityChanged(int value){
    mNoiseBillow->SetLacunarity(value/1000.0);
    mNoisePerlin->SetLacunarity(value/1000.0);
    mNoiseRidgedMulti->SetLacunarity(value/1000.0);
    mLactunarity = value;
    drawXYDiagramm();
}

//octave count
void MFNoiseGenerator::slotOctaveCountChanged(int value){
    mNoiseBillow->SetOctaveCount(value);
    mNoisePerlin->SetOctaveCount(value);
    mNoiseRidgedMulti->SetOctaveCount(value);
    mOctaveCount = value;
    drawXYDiagramm();
}

//persistence of waves
void MFNoiseGenerator::slotPersistenceChanged(int value){
    mNoiseBillow->SetPersistence(1.0*value/1000.0);
    mNoisePerlin->SetPersistence(1.0*value/1000.0);
    mPersistence = value;
    drawXYDiagramm();
}

//seed
void MFNoiseGenerator::slotSeedChanged(int value){
    mNoiseBillow->SetSeed(value);
    mNoisePerlin->SetSeed(value);
    mNoiseRidgedMulti->SetSeed(value);
    mNoiseVoronoi->SetSeed(value);
    mSeed = value;
    drawXYDiagramm();
}
void MFNoiseGenerator::slotConstValueChanged(int value)
{
    mNoiseConst->SetConstValue(value/1000.0);
    mConstValue = value;
    drawXYDiagramm();
}

void MFNoiseGenerator::slotDisplacementChanged(int value)
{
    mNoiseVoronoi->SetDisplacement(value/1000.0);
    mDisplacement = value;
    drawXYDiagramm();
}

void MFNoiseGenerator::slotQualityChanged(int preDefine)
{
    switch(preDefine){
    case 0:
        mNoiseBillow->SetNoiseQuality(noise::QUALITY_FAST);
        mNoiseRidgedMulti->SetNoiseQuality(noise::QUALITY_FAST);
        mNoisePerlin->SetNoiseQuality(noise::QUALITY_FAST);
        mQuality = noise::QUALITY_FAST;
        break;
    case 1:
        mNoiseBillow->SetNoiseQuality(noise::QUALITY_STD);
        mNoiseRidgedMulti->SetNoiseQuality(noise::QUALITY_STD);
        mNoisePerlin->SetNoiseQuality(noise::QUALITY_STD);
        mQuality = noise::QUALITY_STD;
        break;
    case 2:
        mNoiseBillow->SetNoiseQuality(noise::QUALITY_BEST);
        mNoiseRidgedMulti->SetNoiseQuality(noise::QUALITY_BEST);
        mNoisePerlin->SetNoiseQuality(noise::QUALITY_BEST);
        mQuality = noise::QUALITY_BEST;
        break;
    default:
        mNoiseBillow->SetNoiseQuality(noise::QUALITY_STD);
        mNoiseRidgedMulti->SetNoiseQuality(noise::QUALITY_STD);
        mNoisePerlin->SetNoiseQuality(noise::QUALITY_STD);
        mQuality = noise::QUALITY_STD;
        break;
    }
    drawXYDiagramm();
}

void MFNoiseGenerator::on_buttonView_clicked()
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
    mMapGen->setSourceModule(&mActiveModule);

    //the NOISE tread will do the work
    mMapThread->addTask(TASK_GENERATE_MAP);
    mMapThread->continueWhile();
}

bool MFNoiseGenerator::readInputValues(){
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
bool MFNoiseGenerator::readFromLineEdit(QStringList *data, QLineEdit *lineEdit){
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

void MFNoiseGenerator::drawXYDiagramm(){
    std::vector<glm::vec3> map;
    double yValue;
    double xMax = ui->sliderHRange->value()/10.0;
    double yMax = mActiveModule->GetValue(0.0,0.0,0.0);
    for(double i = -xMax; i<xMax;i+=0.01)
    {
        yValue = mActiveModule->GetValue(i,0,0);
        map.push_back(glm::vec3(i,yValue,0.0));
        if(abs(yValue)>yMax)
            yMax = abs(yValue);
    }
    mDiagramm->setDiagramTitel("2D Noise Plot");
    mDiagramm->setPointVector(map,xMax,yMax);
    if(mColorSettings->gradientCount()){
        MFMapGenerator *colorGradGen = new MFMapGenerator();
        colorGradGen->setColorVector(mColorSettings->getColorVector());
        mDiagramm->setColorGradientData(colorGradGen->getGradientColorVector());
        delete(colorGradGen);
    }
    mDiagramm->update();
}

void MFNoiseGenerator::on_sliderHRange_valueChanged(int value)
{
    drawXYDiagramm();
}
