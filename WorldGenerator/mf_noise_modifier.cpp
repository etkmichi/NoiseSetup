#include "mf_noise_modifier.h"
#include "ui_mf_noise_modifier.h"

MFNoiseModifier::MFNoiseModifier( int indexID,QString name,QWidget *parent) :
    QWidget(parent),
    ui(new Ui::MFNoiseModifier),
    mModifierAbs(),
    mModifierClamp(),
    mModifierExponent(),
    mModifierInvert(),
    mModifierScaleBias(),
    mModifierTerrace(),
    mModifierCurve(),
    mInputDiag(),
    mOutputDiag()
{
    ui->setupUi(this);
    ui->lineEditName->setText(name);
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
    mColorSettings->adjustSize();
    ui->verticalLayoutColors->addWidget(mColorSettings);
    mColorSettings->show();

    //Init diagrams
    mInputDiag.setDiagramTitel(tr("Input"));
    mInputDiag.setMinimumHeight(150);
    mInputDiag.setMinimumWidth(300);
    mInputDiag.update();
    mOutputDiag.setDiagramTitel(tr("Output"));
    mOutputDiag.setMinimumHeight(150);
    mOutputDiag.setMinimumWidth(300);
    ui->layoutVInputNoise->insertWidget(1,&mInputDiag);
    ui->layoutVOut->insertWidget(1,&mOutputDiag);

    ui->comboBoxType1->setEnabled(false);
    this->setParent(parent);

    //init noise objects
    mOutputCache = new noise::module::Cache();//will be used as input of other modules
    mInputModule = new noise::module::Perlin();//will be set to the source module of cache
    setInputModule(mInputModule);
    mOutputCache->SetSourceModule(0,mModifierAbs);

    //init noise modules with some random values
    mModifierCurve.ClearAllControlPoints();
    mModifierCurve.AddControlPoint(0.1,0.1);
    mModifierCurve.AddControlPoint(0.4,0.8);
    mModifierCurve.AddControlPoint(0.5,0.1);
    mModifierCurve.AddControlPoint(1.0,1.0);
    mModifierTerrace.ClearAllControlPoints();
    mModifierTerrace.AddControlPoint(0.1);
    mModifierTerrace.AddControlPoint(0.5);

    //init some other ui things
    QRegExp regExp("[1-9][0-9]{0,2}[,][1-9][0-9]{0,2}");
    ui->sizeXY->setText("10,10");
    ui->ResolutionXY->setText("70,70");
    ui->posXY->setText("0,0");
    ui->sizeXY->setValidator(new QRegExpValidator(regExp,this));
    ui->ResolutionXY->setValidator(new QRegExpValidator(regExp,this));
    ui->ResolutionXY->setValidator(new QRegExpValidator(regExp,this));

    connect(mSigCtl,SIGNAL(mapCalculated()),this,SLOT(slotMapCalculated()),Qt::QueuedConnection);
    connect(mColorSettings,SIGNAL(colorListChanged()),this,SLOT(updateDiagrams()));

    //init modifier setting widgets
    mListPoints = 0;
}
void MFNoiseModifier::setGeneratorVectorPointer(std::vector<MFNoiseGenerator*> *list){
    mPGeneratorVector = list;
}
void MFNoiseModifier::setCombinorVectorPointer(std::vector<MFNoiseCombinor *> *list){
    mPCombinerVector = list;
}

MFNoiseModifier::~MFNoiseModifier()
{
    delete ui;
}

void MFNoiseModifier::updateDiagrams(){
    drawXYDiagram(&mOutputDiag,double(ui->sliderOutput->value()),mOutputCache);
    drawXYDiagram(&mInputDiag,double(ui->sliderInput1->value()),mInputModule);
}
void MFNoiseModifier::slotMapCalculated(){
    mMapThread->addTask(TASK_VIEW_MAP);
}
void MFNoiseModifier::setInputModule(noise::module::Module *input){
    mModifierAbs.SetSourceModule(0,*input);
    mModifierClamp.SetSourceModule(0,*input);
    mModifierCurve.SetSourceModule(0,*input);
    mModifierExponent.SetSourceModule(0,*input);
    mModifierInvert.SetSourceModule(0,*input);
    mModifierScaleBias.SetSourceModule(0,*input);
    mModifierTerrace.SetSourceModule(0,*input);
}
void MFNoiseModifier::on_comboBoxModifier_currentIndexChanged(int index)
{
    switch(index){
    case INDEX_ABS:
        mOutputCache->SetSourceModule(0,mModifierAbs);
        break;
    case INDEX_CLAMB:
        makeClambWidgets();
        mOutputCache->SetSourceModule(0,mModifierClamp);
        break;
    case INDEX_CURVE:
        makeCurveWidgets();
        if(mModifierCurve.GetControlPointCount()<4)return;
        mOutputCache->SetSourceModule(0,mModifierCurve);
        break;
    case INDEX_EXPONENT:
        makeExponentWidgets();
        mOutputCache->SetSourceModule(0,mModifierExponent);
        break;
    case INDEX_INVERT:
        mOutputCache->SetSourceModule(0,mModifierInvert);
        break;
    case INDEX_SCALEBIAS:
        makeScaleBiasWidgets();
        mOutputCache->SetSourceModule(0,mModifierScaleBias);
        break;
    case INDEX_TERRACE:
        makeTerraceWidgets();
        mOutputCache->SetSourceModule(0,mModifierTerrace);
        break;
    default:
        mOutputCache->SetSourceModule(0,mModifierAbs);
        break;
    }

    drawXYDiagram(&mOutputDiag,double(ui->sliderOutput->value()),mOutputCache);
    ui->comboBoxType1->setEnabled(true);
}
void MFNoiseModifier::on_comboBoxType1_currentIndexChanged(int index)
{
    //change the active list widget to configure it
    mInputIndex = index;
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

void MFNoiseModifier::updateInputLists(){
    if(!checkVectorPointers())return;
    ui->listWidgetInput1->clear();
    switch(mInputIndex){
    case INDEX_GENERATOR:
        for(uint i =0;i<mPGeneratorVector[0].size();i++)
            ui->listWidgetInput1->addItem(mPGeneratorVector[0][i]->getName());
        break;
    case INDEX_COMBINER:
        for(uint i =0;i<mPCombinerVector[0].size();i++)
            ui->listWidgetInput1->addItem(mPCombinerVector[0][i]->getName());
        break;
    case INDEX_MODIFIER:
        for(uint i =0;i<mPModifierVector[0].size();i++)
            ui->listWidgetInput1->addItem(mPModifierVector[0][i]->getName());
        break;
    case INDEX_SELECTOR:
        for(uint i =0;i<mPSelectorVector[0].size();i++)
            ui->listWidgetInput1->addItem(mPSelectorVector[0][i]->getName());
        break;
    case INDEX_TRANSFORMER:
        for(uint i =0;i<mPTransformerVector[0].size();i++)
            ui->listWidgetInput1->addItem(mPTransformerVector[0][i]->getName());
        break;
    default:
        break;
    }
}

bool MFNoiseModifier::checkVectorPointers(){
    if(mPGeneratorVector   == 0||
            mPCombinerVector    == 0||
            mPModifierVector    == 0||
            mPSelectorVector    == 0||
            mPTransformerVector == 0)
        return false;
    return true;
}

void MFNoiseModifier::on_listWidgetInput1_activated(const QModelIndex &index)
{
    mInputModule = getActiveIputModule();
    if(       (mOutputCache) == mInputModule
            || mInputModule == NULL)
    {
        ui->listWidgetInput1->item(index.row())->setBackgroundColor(QColor(255,100,100));
        ui->listWidgetInput1->item(index.row())->setSelected(false);
        return;
    }
    ui->listWidgetInput1->item(index.row())->setBackgroundColor(QColor(255,255,255));
    //draw the input1 diagram
    drawXYDiagram(&mInputDiag,ui->sliderInput1->value(),mInputModule);
    setInputModule(mInputModule);
    drawXYDiagram(&mOutputDiag,ui->sliderOutput->value(),(mOutputCache));
}

noise::module::Module* MFNoiseModifier::getActiveIputModule(){
    int listType = ui->comboBoxType1->currentIndex();
    switch(listType){
    case INDEX_GENERATOR:
        return mPGeneratorVector[0][ui->listWidgetInput1->currentRow()]->getSourceModule();
        break;
    case INDEX_COMBINER:
        return mPCombinerVector[0][ui->listWidgetInput1->currentRow()]->getSourceModule(mOutputCache);
        break;
    case INDEX_MODIFIER:
        return mPModifierVector[0][ui->listWidgetInput1->currentRow()]->getSourceModule(mOutputCache);
        break;
    case INDEX_SELECTOR:
        return mPSelectorVector[0][ui->listWidgetInput1->currentRow()]->getSourceModule(mOutputCache);
        break;
    case INDEX_TRANSFORMER:
        return mPTransformerVector[0][ui->listWidgetInput1->currentRow()]->getSourceModule(mOutputCache);
        break;
    }
}

void MFNoiseModifier::drawXYDiagram(MFXYDiagramm *diagram, double max,noise::module::Module *module){
    std::vector<glm::vec3> map;
    double yValue;
    double xMax = max/10.0;
    double yMax = module->GetValue(0.1,0.1,0.1);
    for(double i = -xMax; i<xMax;i+=0.01)
    {
        yValue = module->GetValue(i,0,0);
        map.push_back(glm::vec3(i,yValue,0.0));
        if(abs(yValue)>yMax)
            yMax = abs(yValue);
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

noise::module::Module* MFNoiseModifier::getSourceModule(noise::module::Module *myID)
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

bool MFNoiseModifier::checkForSameInputPointer(noise::module::Module *pointerToCheck, noise::module::Module *srcModule){
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

void MFNoiseModifier::on_sliderInput1_sliderMoved(int position)
{
    drawXYDiagram(&mInputDiag,position,mInputModule);
}

void MFNoiseModifier::on_sliderOutput_sliderMoved(int position)
{
    drawXYDiagram(&mOutputDiag,position,(mOutputCache));
}

void MFNoiseModifier::on_buttonView_clicked()
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

bool MFNoiseModifier::readInputValues(){
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

bool MFNoiseModifier::readFromLineEdit(QStringList *data, QLineEdit *lineEdit){
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

void MFNoiseModifier::makeClambWidgets(){
    QLabel *labelLowerBound= new QLabel(tr("Lower bound:"));
    QLabel *labelUpperBound= new QLabel(tr("Upper bound:"));
    QDoubleSpinBox *sbLower = new QDoubleSpinBox();
    QDoubleSpinBox *sbUpper = new QDoubleSpinBox();
    sbLower->setRange(-100000.0,+100000.0);
    sbUpper->setRange(-100000.0,+100000.0);
    ui->groupBoxSettings->layout()->addWidget(labelLowerBound);
    ui->groupBoxSettings->layout()->addWidget(sbLower);
    ui->groupBoxSettings->layout()->addWidget(labelUpperBound);
    ui->groupBoxSettings->layout()->addWidget(sbUpper);
    sbLower->setValue(mClambLowerBound);
    sbUpper->setValue(mClambUpperBound);
    connect(sbLower,SIGNAL(valueChanged(double)),this,SLOT(slotLowerBoundChanged(double)));
    connect(sbUpper,SIGNAL(valueChanged(double)),this,SLOT(slotUpperBoundChanged(double)));
    connect(ui->comboBoxModifier,SIGNAL(currentIndexChanged(int)),sbLower,SLOT(deleteLater()));
    connect(ui->comboBoxModifier,SIGNAL(currentIndexChanged(int)),sbUpper,SLOT(deleteLater()));
    connect(ui->comboBoxModifier,SIGNAL(currentIndexChanged(int)),labelLowerBound,SLOT(deleteLater()));
    connect(ui->comboBoxModifier,SIGNAL(currentIndexChanged(int)),labelUpperBound,SLOT(deleteLater()));
}

void MFNoiseModifier::slotLowerBoundChanged(double lower){
    if(lower>=mClambUpperBound )
        return;
    mClambLowerBound = lower;
    mModifierClamp.SetBounds(mClambLowerBound,mClambUpperBound);
    drawXYDiagram(&mOutputDiag,double(ui->sliderOutput->value()),mOutputCache);
}

void MFNoiseModifier::slotUpperBoundChanged(double upper){
    if(upper<=mClambLowerBound){
        return;
    }
    mClambUpperBound = upper;
    mModifierClamp.SetBounds(mClambLowerBound,mClambUpperBound);
    drawXYDiagram(&mOutputDiag,double(ui->sliderOutput->value()),mOutputCache);
}

void MFNoiseModifier::makeCurveWidgets(){
    QPushButton *add =new QPushButton(tr("Add Point"));
    QPushButton *edit =new QPushButton(tr("Edit"));
    QPushButton *remove =new QPushButton(tr("Delete"));
    mListPoints = new QListWidget(this);
    ui->groupBoxSettings->layout()->addWidget(add);
    ui->groupBoxSettings->layout()->addWidget(edit);
    ui->groupBoxSettings->layout()->addWidget(remove);
    ui->groupBoxSettings->layout()->addWidget(mListPoints);
    const noise::module::ControlPoint* cp = mModifierCurve.GetControlPointArray();
    mListPoints->clear();
    for(int i = 0; i<mModifierCurve.GetControlPointCount();i++){
        QString text =QString("In=") +QString::number(cp[i].inputValue,'f',3)+
                QString(" -> Out=")+QString::number((cp[i].outputValue),'f',3);
        mListPoints->addItem(text);
    }
    connect(add,SIGNAL(clicked(bool)),this,SLOT(slotCurveAdd()));
    connect(remove,SIGNAL(clicked(bool)),this,SLOT(slotCurveRemove()));
    connect(edit,SIGNAL(clicked(bool)),this,SLOT(slotCurveEdit()));
    connect(ui->comboBoxModifier,SIGNAL(currentIndexChanged(int)),add,SLOT(deleteLater()));
    connect(ui->comboBoxModifier,SIGNAL(currentIndexChanged(int)),edit,SLOT(deleteLater()));
    connect(ui->comboBoxModifier,SIGNAL(currentIndexChanged(int)),remove,SLOT(deleteLater()));
    connect(ui->comboBoxModifier,SIGNAL(currentIndexChanged(int)),mListPoints,SLOT(deleteLater()));
    drawXYDiagram(&mOutputDiag,double(ui->sliderOutput->value()),mOutputCache);
}
void MFNoiseModifier::slotCurveAdd(){
    bool ok = false;
    double in;
    double out;
    in = QInputDialog::getDouble(this,tr("Input Value"),tr("Value:"),0,-1000000,1000000,3,&ok);
    if(!ok)return;
    const noise::module::ControlPoint* cp = mModifierCurve.GetControlPointArray();
    for(int i = 0; i<mModifierCurve.GetControlPointCount();i++){
        if(in == cp[i].inputValue)
        {
            QMessageBox msgBox;
            msgBox.setText("Input values must be different from other input valuse!");
            msgBox.exec();
            return;
        }
    }
    out =QInputDialog::getDouble(this,tr("Output Value"),tr("Value:"),0,-1000000,1000000,3,&ok);
    if(!ok)return;
    mModifierCurve.AddControlPoint(in,out);
    mListPoints->clear();
    cp = mModifierCurve.GetControlPointArray();
    for(int i = 0; i<mModifierCurve.GetControlPointCount();i++){
        double in1 = cp[i].inputValue;
        double out1 = cp[i].outputValue;
        QString text =QString("In=") +QString::number(in1,'f',3)+
                QString(" -> Out=")+QString::number(out1,'f',3);
        mListPoints->addItem(text);
    }
    drawXYDiagram(&mOutputDiag,double(ui->sliderOutput->value()),mOutputCache);
}
void MFNoiseModifier::slotCurveEdit(){
    if(!mListPoints->currentIndex().isValid())
        return;
    bool ok = false;
    double in;
    double out;
    in = QInputDialog::getDouble(this,tr("Input Value"),tr("Value:"),0,-1000000,1000000,3,&ok);
    if(!ok)return;
    const noise::module::ControlPoint* cp = mModifierCurve.GetControlPointArray();
    int size = mModifierCurve.GetControlPointCount();
    noise::module::ControlPoint newArray[size];

    out =QInputDialog::getDouble(this,tr("Output Value"),tr("Value:"),0,-1000000,1000000,3,&ok);
    if(!ok)return;

    //change the edited values and add all values from newArray to curve
    //copy the array
    for(int i = 0; i<size;i++)
        newArray[i]=cp[i];

    //change the values of the new array
    int pos = mListPoints->currentRow();
    newArray[pos].inputValue=in;
    newArray[pos].outputValue=out;

    //check for same values
    for(int i = 0; i<size;i++){
        if(in == newArray[i].inputValue && i!=pos)
        {
            QMessageBox msgBox;
            msgBox.setText("Input values must be different from other input valuse!");
            msgBox.exec();
            return;
        }
    }

    //make new curve
    mModifierCurve.ClearAllControlPoints();
    for(int i=0; i<size;i++){
        mModifierCurve.AddControlPoint(newArray[i].inputValue,newArray[i].outputValue);
    }

    //insert to list
    mListPoints->clear();
    cp = mModifierCurve.GetControlPointArray();
    for(int i = 0; i<mModifierCurve.GetControlPointCount();i++){
        double in1 = cp[i].inputValue;
        double out1 = cp[i].outputValue;
        QString text =QString("In=") +QString::number(in1,'f',3)+
                QString(" -> Out=")+QString::number(out1,'f',3);
        mListPoints->addItem(text);
    }
    drawXYDiagram(&mOutputDiag,double(ui->sliderOutput->value()),mOutputCache);
}
void MFNoiseModifier::slotCurveRemove(){
    int size = mModifierCurve.GetControlPointCount();
    if(!mListPoints->currentIndex().isValid() || size<5)
        return;

    const noise::module::ControlPoint* cp = mModifierCurve.GetControlPointArray();
    noise::module::ControlPoint newArray[size-1];

    //change the values of the new array
    int pos = mListPoints->currentRow();

    //change the edited values and add all values from newArray to curve
    //copy the array
    for(int i = 0; i<size-1;i++){
        if(i<pos)
            newArray[i]=cp[i];
        else
            newArray[i]=cp[i+1];
    }

    //make new curve
    mModifierCurve.ClearAllControlPoints();
    for(int i=0; i<size-1;i++){
        mModifierCurve.AddControlPoint(newArray[i].inputValue,newArray[i].outputValue);
    }

    //insert to list
    mListPoints->clear();
    cp = mModifierCurve.GetControlPointArray();
    for(int i = 0; i<mModifierCurve.GetControlPointCount();i++){
        double in1 = cp[i].inputValue;
        double out1 = cp[i].outputValue;
        QString text =QString("In=") +QString::number(in1,'f',3)+
                QString(" -> Out=")+QString::number(out1,'f',3);
        mListPoints->addItem(text);
    }
    mListPoints->setCurrentRow(pos);
    drawXYDiagram(&mOutputDiag,double(ui->sliderOutput->value()),mOutputCache);
}

void MFNoiseModifier::makeExponentWidgets(){
    QLabel *label= new QLabel(tr("Exponent:"));
    QDoubleSpinBox *sbDouble = new QDoubleSpinBox();
    sbDouble->setRange(-1000.0,+1000.0);
    ui->groupBoxSettings->layout()->addWidget(label);
    ui->groupBoxSettings->layout()->addWidget(sbDouble);
    sbDouble->setValue(mExponent);
    mModifierExponent.SetExponent(mExponent);
    connect(sbDouble,SIGNAL(valueChanged(double)),this,SLOT(slotExponentChanged(double)));
    connect(ui->comboBoxModifier,SIGNAL(currentIndexChanged(int)),sbDouble,SLOT(deleteLater()));
    connect(ui->comboBoxModifier,SIGNAL(currentIndexChanged(int)),label,SLOT(deleteLater()));

//    void 	SetExponent (double exponent)
//        Sets the exponent value to apply to the output value from the source module.
}
void MFNoiseModifier::slotExponentChanged(double exponent){
    mExponent = exponent;
    mModifierExponent.SetExponent(mExponent);
    drawXYDiagram(&mOutputDiag,double(ui->sliderOutput->value()),mOutputCache);
}

void MFNoiseModifier::makeScaleBiasWidgets(){
    QLabel *labelBias= new QLabel(tr("Bias/Offset:"));
    QLabel *labelScale= new QLabel(tr("Scaling factor:"));
    QDoubleSpinBox *sbBias = new QDoubleSpinBox();
    QDoubleSpinBox *sbScale = new QDoubleSpinBox();
    sbBias->setRange(-100000.0,+100000.0);
    sbScale->setRange(-100000.0,+100000.0);
    ui->groupBoxSettings->layout()->addWidget(labelBias);
    ui->groupBoxSettings->layout()->addWidget(sbBias);
    ui->groupBoxSettings->layout()->addWidget(labelScale);
    ui->groupBoxSettings->layout()->addWidget(sbScale);
    sbBias->setValue(mBias);
    sbScale->setValue(mScale);
    connect(sbBias,SIGNAL(valueChanged(double)),this,SLOT(slotBiasChanged(double)));
    connect(sbScale,SIGNAL(valueChanged(double)),this,SLOT(slotScaleChanged(double)));
    connect(ui->comboBoxModifier,SIGNAL(currentIndexChanged(int)),sbBias,SLOT(deleteLater()));
    connect(ui->comboBoxModifier,SIGNAL(currentIndexChanged(int)),sbScale,SLOT(deleteLater()));
    connect(ui->comboBoxModifier,SIGNAL(currentIndexChanged(int)),labelBias,SLOT(deleteLater()));
    connect(ui->comboBoxModifier,SIGNAL(currentIndexChanged(int)),labelScale,SLOT(deleteLater()));
}

void MFNoiseModifier::slotBiasChanged(double bias){
    mBias = bias;
    mModifierScaleBias.SetBias(mBias);
    drawXYDiagram(&mOutputDiag,double(ui->sliderOutput->value()),mOutputCache);
}
void MFNoiseModifier::slotScaleChanged(double scale){
    mScale = scale;
    mModifierScaleBias.SetScale(mScale);
    drawXYDiagram(&mOutputDiag,double(ui->sliderOutput->value()),mOutputCache);
}

void MFNoiseModifier::makeTerraceWidgets(){
    QPushButton *add =new QPushButton(tr("Add Point"));
    QPushButton *edit =new QPushButton(tr("Edit"));
    QPushButton *remove =new QPushButton(tr("Delete"));
    QPushButton *invert = new QPushButton(tr("Invert"));
    QPushButton *makeP= new QPushButton(tr("Make Points"));
    mListPoints = new QListWidget(this);
    invert->setCheckable(true);
    ui->groupBoxSettings->layout()->addWidget(add);
    ui->groupBoxSettings->layout()->addWidget(edit);
    ui->groupBoxSettings->layout()->addWidget(remove);
    ui->groupBoxSettings->layout()->addWidget(mListPoints);
    ui->groupBoxSettings->layout()->addWidget(invert);
    ui->groupBoxSettings->layout()->addWidget(makeP);
    const double* cp = mModifierTerrace.GetControlPointArray();
    mListPoints->clear();
    for(int i = 0; i<mModifierTerrace.GetControlPointCount();i++){
        QString text =QString("P%i =") +QString::number(cp[i],'f',3);
        mListPoints->addItem(text);
    }
    connect(add,SIGNAL(clicked(bool)),this,SLOT(slotTerraceAdd()));
    connect(remove,SIGNAL(clicked(bool)),this,SLOT(slotTerraceRemove()));
    connect(edit,SIGNAL(clicked(bool)),this,SLOT(slotTerraceEdit()));
    connect(invert,SIGNAL(toggled(bool)),this,SLOT(slotTerraceInvert(bool)));
    connect(makeP,SIGNAL(clicked(bool)),this,SLOT(slotTerraceMake()));
    connect(ui->comboBoxModifier,SIGNAL(currentIndexChanged(int)),add,SLOT(deleteLater()));
    connect(ui->comboBoxModifier,SIGNAL(currentIndexChanged(int)),edit,SLOT(deleteLater()));
    connect(ui->comboBoxModifier,SIGNAL(currentIndexChanged(int)),remove,SLOT(deleteLater()));
    connect(ui->comboBoxModifier,SIGNAL(currentIndexChanged(int)),invert,SLOT(deleteLater()));
    connect(ui->comboBoxModifier,SIGNAL(currentIndexChanged(int)),makeP,SLOT(deleteLater()));
    connect(ui->comboBoxModifier,SIGNAL(currentIndexChanged(int)),mListPoints,SLOT(deleteLater()));
    drawXYDiagram(&mOutputDiag,double(ui->sliderOutput->value()),mOutputCache);
}
void MFNoiseModifier::slotTerraceInvert(bool toggle){
    mModifierTerrace.InvertTerraces(toggle);
    drawXYDiagram(&mOutputDiag,double(ui->sliderOutput->value()),mOutputCache);
}

void MFNoiseModifier::slotTerraceMake(){
    bool ok = false;
    int in = 2;

    in = QInputDialog::getInt(this,tr("Terrace Value"),tr("Value:"),2,2,1000000,1,&ok);
    if(!ok)return;

    mModifierTerrace.MakeControlPoints(in);
    mListPoints->clear();
    const double* cp = mModifierTerrace.GetControlPointArray();
    for(int i = 0; i<mModifierTerrace.GetControlPointCount();i++){
        double in1 = cp[i];
        QString text =QString("P%i =") +QString::number(in1,'f',3);
        mListPoints->addItem(text);
    }
    drawXYDiagram(&mOutputDiag,double(ui->sliderOutput->value()),mOutputCache);
}

void MFNoiseModifier::slotTerraceAdd(){
    bool ok = false;
    double in;

    in = QInputDialog::getDouble(this,tr("Terrace Value"),tr("Value:"),0,-1000000,1000000,3,&ok);
    if(!ok)return;

    const double* cp = mModifierTerrace.GetControlPointArray();
    for(int i = 0; i<mModifierTerrace.GetControlPointCount();i++){
        if(in == cp[i])
        {
            QMessageBox msgBox;
            msgBox.setText("Input value must differ from other values!");
            msgBox.exec();
            return;
        }
    }

    mModifierTerrace.AddControlPoint(in);
    mListPoints->clear();
    cp = mModifierTerrace.GetControlPointArray();
    for(int i = 0; i<mModifierTerrace.GetControlPointCount();i++){
        double in1 = cp[i];
        QString text =QString("P%i =") +QString::number(in1,'f',3);
        mListPoints->addItem(text);
    }
    drawXYDiagram(&mOutputDiag,double(ui->sliderOutput->value()),mOutputCache);
}
void MFNoiseModifier::slotTerraceEdit(){
    if(!mListPoints->currentIndex().isValid())
        return;
    bool ok = false;
    double in;

    in = QInputDialog::getDouble(this,tr("Terrace Value"),tr("Value:"),0,-1000000,1000000,3,&ok);
    if(!ok)return;
    const double* cp = mModifierTerrace.GetControlPointArray();
    int size = mModifierTerrace.GetControlPointCount();
    double newArray[size];

    //change the edited values and add all values from newArray to curve
    //copy the array
    for(int i = 0; i<size;i++)
        newArray[i]=cp[i];

    //change the values of the new array
    int pos = mListPoints->currentRow();
    newArray[pos]=in;

    //check for same values
    for(int i = 0; i<size;i++){
        if(in == newArray[i] && i!=pos)
        {
            QMessageBox msgBox;
            msgBox.setText("Input values must differ from other values!");
            msgBox.exec();
            return;
        }
    }

    //make new curve
    mModifierTerrace.ClearAllControlPoints();
    for(int i=0; i<size;i++){
        mModifierTerrace.AddControlPoint(newArray[i]);
    }

    //insert to list
    mListPoints->clear();
    cp = mModifierTerrace.GetControlPointArray();
    for(int i = 0; i<mModifierTerrace.GetControlPointCount();i++){
        double in1 = cp[i];
        QString text =QString("P%i =") +QString::number(in1,'f',3);
        mListPoints->addItem(text);
    }
    drawXYDiagram(&mOutputDiag,double(ui->sliderOutput->value()),mOutputCache);
}
void MFNoiseModifier::slotTerraceRemove(){
    int size = mModifierTerrace.GetControlPointCount();
    if(!mListPoints->currentIndex().isValid() || size<3)
        return;

    const double* cp = mModifierTerrace.GetControlPointArray();
    double newArray[size-1];

    //change the values of the new array
    int pos = mListPoints->currentRow();

    //change the edited values and add all values from newArray to curve
    //copy the array
    for(int i = 0; i<size-1;i++){
        if(i<pos)
            newArray[i]=cp[i];
        else
            newArray[i]=cp[i+1];
    }

    //make new curve
    mModifierTerrace.ClearAllControlPoints();
    for(int i=0; i<size-1;i++){
        mModifierTerrace.AddControlPoint(newArray[i]);
    }

    //insert to list
    mListPoints->clear();
    cp = mModifierTerrace.GetControlPointArray();
    for(int i = 0; i<mModifierTerrace.GetControlPointCount();i++){
        double in1 = cp[i];
        QString text =QString("P%i = ") +QString::number(in1,'f',3);
        mListPoints->addItem(text);
    }
    mListPoints->setCurrentRow(pos);
    drawXYDiagram(&mOutputDiag,double(ui->sliderOutput->value()),mOutputCache);
}


