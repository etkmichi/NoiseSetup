#include "mf_noise_combinor.h"
#include "ui_mf_noise_combinor.h"

MFNoiseCombinor::MFNoiseCombinor( int indexID,QString name,QWidget *parent) :
    ui(new Ui::MFNoiseCombinor),
    mCombinerAdd(),
    mCombinerMax(),
    mCombinerMin(),
    mCombinerMultiply(),
    mCombinerPower(),
    mInput1Diag(),
    mInput2Diag(),
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
    mInput1Diag.setDiagramTitel(tr("Input 1"));
    mInput1Diag.setMinimumHeight(150);
    mInput1Diag.setMinimumWidth(300);
    mInput1Diag.update();
    mInput2Diag.setDiagramTitel(tr("Input 2"));
    mInput2Diag.setMinimumHeight(150);
    mInput2Diag.setMinimumWidth(300);
    mInput2Diag.update();
    mOutputDiag.setDiagramTitel(tr("Output"));
    mOutputDiag.setMinimumHeight(150);
    mOutputDiag.setMinimumWidth(300);
    ui->layoutVInputNoise->insertWidget(1,&mInput1Diag);
    ui->layoutVInputNoise->insertWidget(2,&mInput2Diag);
    ui->layoutVOut->insertWidget(1,&mOutputDiag);

    ui->comboBoxType1->setEnabled(false);
    ui->comboBoxType2->setEnabled(false);
    this->setParent(parent);

    //init noise objects
    mOutputCache = new noise::module::Cache();//will be used as input of other modules
    mActiveModuleInput1 = new noise::module::Perlin();//will be set to the source module of cache
    mActiveModuleInput2 = new noise::module::Perlin();
    setInputModule(0,mActiveModuleInput1);
    setInputModule(1,mActiveModuleInput2);
    mOutputCache->SetSourceModule(0,mCombinerAdd);

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

}


void MFNoiseCombinor::updateDiagrams(){
    drawXYDiagram(&mOutputDiag,double(ui->sliderOutput->value()),mOutputCache);
    drawXYDiagram(&mInput1Diag,double(ui->sliderInput1->value()),mActiveModuleInput1);
    drawXYDiagram(&mInput2Diag,double(ui->sliderInput2->value()),mActiveModuleInput2);
}

MFNoiseCombinor::~MFNoiseCombinor()
{
    delete ui;
}

void MFNoiseCombinor::slotMapCalculated(){
    mMapThread->addTask(TASK_VIEW_MAP);
}
void MFNoiseCombinor::on_comboBoxCombiner_currentIndexChanged(int index)
{
    switch(index){
    case 0://Add
        mOutputCache->SetSourceModule(0,mCombinerAdd);
        break;
    case 1://Max
        mOutputCache->SetSourceModule(0,mCombinerMax);
        break;
    case 2://Min
        mOutputCache->SetSourceModule(0,mCombinerMin);
        break;
    case 3://Multiply
        mOutputCache->SetSourceModule(0,mCombinerMultiply);
        break;
    case 4://Power
        mOutputCache->SetSourceModule(0,mCombinerPower);
        break;
    default://Add
        mOutputCache->SetSourceModule(0,mCombinerAdd);
        break;
    }

    drawXYDiagram(&mOutputDiag,double(ui->sliderOutput->value()),mOutputCache);
    ui->comboBoxType1->setEnabled(true);
    ui->comboBoxType2->setEnabled(true);
}

void MFNoiseCombinor::on_comboBoxType1_currentIndexChanged(int index)
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

void MFNoiseCombinor::on_comboBoxType2_currentIndexChanged(int index)
{
    //change the active list widget to configure it
    m_type2Index = index;
    if(!checkVectorPointers())return;
    switch(index){
    case 0://Generator Module
        ui->listWidgetInput2->clear();
        for(uint i =0;i<mPGeneratorVector[0].size();i++)
            ui->listWidgetInput2->addItem(mPGeneratorVector[0][i]->getName());
        break;
    case 1://Combinor
        ui->listWidgetInput2->clear();
        for(uint i =0;i<mPCombinerVector[0].size();i++)
            ui->listWidgetInput2->addItem(mPCombinerVector[0][i]->getName());
        break;
    case 2://Modifier
        ui->listWidgetInput2->clear();
        for(uint i =0;i<mPModifierVector[0].size();i++)
            ui->listWidgetInput2->addItem(mPModifierVector[0][i]->getName());
        break;
    case 3://Selector
        ui->listWidgetInput2->clear();
        for(uint i =0;i<mPSelectorVector[0].size();i++)
            ui->listWidgetInput2->addItem(mPSelectorVector[0][i]->getName());
        break;
    case 4://Transformer
        ui->listWidgetInput2->clear();
        for(uint i =0;i<mPTransformerVector[0].size();i++)
            ui->listWidgetInput2->addItem(mPTransformerVector[0][i]->getName());
        break;

    default:
        break;
    }
}

void MFNoiseCombinor::updateInputLists(){
    if(!checkVectorPointers())return;
    switch(m_type2Index){
    case INDEX_GENERATOR://Generator Module
        ui->listWidgetInput2->clear();
        for(uint i =0;i<mPGeneratorVector[0].size();i++)
            ui->listWidgetInput2->addItem(mPGeneratorVector[0][i]->getName());
        break;
    case INDEX_COMBINER://Combinor
        ui->listWidgetInput2->clear();
        for(uint i =0;i<mPCombinerVector[0].size();i++)
            ui->listWidgetInput2->addItem(mPCombinerVector[0][i]->getName());
        break;
    case INDEX_MODIFIER://Modifier
        ui->listWidgetInput2->clear();
        for(uint i =0;i<mPModifierVector[0].size();i++)
            ui->listWidgetInput2->addItem(mPModifierVector[0][i]->getName());
        break;
    case INDEX_SELECTOR://Selector
        ui->listWidgetInput2->clear();
        for(uint i =0;i<mPSelectorVector[0].size();i++)
            ui->listWidgetInput2->addItem(mPSelectorVector[0][i]->getName());
        break;
    case INDEX_TRANSFORMER://Transformer
        ui->listWidgetInput2->clear();
        for(uint i =0;i<mPTransformerVector[0].size();i++)
            ui->listWidgetInput2->addItem(mPTransformerVector[0][i]->getName());
        break;

    default:
        break;
    }


    switch(m_type1Index){
    case INDEX_GENERATOR://Generator Module
        ui->listWidgetInput1->clear();
        for(uint i =0;i<mPGeneratorVector[0].size();i++)
            ui->listWidgetInput1->addItem(mPGeneratorVector[0][i]->getName());
        break;
    case INDEX_COMBINER://Combinor
        ui->listWidgetInput1->clear();
        for(uint i =0;i<mPCombinerVector[0].size();i++)
            ui->listWidgetInput1->addItem(mPCombinerVector[0][i]->getName());
        break;
    case INDEX_MODIFIER://Modifier
        ui->listWidgetInput1->clear();
        for(uint i =0;i<mPModifierVector[0].size();i++)
            ui->listWidgetInput1->addItem(mPModifierVector[0][i]->getName());
        break;
    case INDEX_SELECTOR://Selector
        ui->listWidgetInput1->clear();
        for(uint i =0;i<mPSelectorVector[0].size();i++)
            ui->listWidgetInput1->addItem(mPSelectorVector[0][i]->getName());
        break;
    case INDEX_TRANSFORMER://Transformer
        ui->listWidgetInput1->clear();
        for(uint i =0;i<mPTransformerVector[0].size();i++)
            ui->listWidgetInput1->addItem(mPTransformerVector[0][i]->getName());
        break;

    default:
        break;
    }
}

bool MFNoiseCombinor::checkVectorPointers(){
    if(mPGeneratorVector   == 0||
            mPCombinerVector    == 0||
            mPModifierVector    == 0||
            mPSelectorVector    == 0||
            mPTransformerVector == 0)
        return false;
    return true;
}

void MFNoiseCombinor::on_listWidgetInput1_activated(const QModelIndex &index)
{
    mActiveModuleInput1 = getActiveIput1Module();
    if(        mActiveModuleInput1 == mActiveModuleInput2
            || (mOutputCache) == mActiveModuleInput1
            || (mOutputCache) == mActiveModuleInput2
            || mActiveModuleInput1 == NULL)
    {
        ui->listWidgetInput1->item(index.row())->setBackgroundColor(QColor(255,100,100));
        ui->listWidgetInput1->item(index.row())->setSelected(false);
        return;
    }
    ui->listWidgetInput1->item(index.row())->setBackgroundColor(QColor(255,255,255));
    //draw the input1 diagram
    drawXYDiagram(&mInput1Diag,ui->sliderInput1->value(),mActiveModuleInput1);
    drawXYDiagram(&mInput2Diag,ui->sliderInput2->value(),mActiveModuleInput2);
    setInputModule(0,mActiveModuleInput1);
    drawXYDiagram(&mOutputDiag,ui->sliderOutput->value(),(mOutputCache));
}
noise::module::Module* MFNoiseCombinor::getActiveIput1Module(){
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
void MFNoiseCombinor::drawXYDiagram(MFXYDiagramm *diagram, double max,noise::module::Module *module){
    std::vector<glm::vec3> map;
    double yValue;
    double xMax = max/10.0;
    double yMax = module->GetValue(0.0,0.0,0.0);
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

void MFNoiseCombinor::on_listWidgetInput2_activated(const QModelIndex &index)
{
    mActiveModuleInput2=getActiveIput2Module();
    if(        mActiveModuleInput1 == mActiveModuleInput2
            || (mOutputCache) == mActiveModuleInput2
            || (mOutputCache) == mActiveModuleInput1
            || mActiveModuleInput2 == NULL)
    {
        ui->listWidgetInput2->item(index.row())->setBackgroundColor(QColor(255,100,100));
        ui->listWidgetInput2->item(index.row())->setSelected(false);
        return;
    }
    ui->listWidgetInput2->item(index.row())->setBackgroundColor(QColor(255,255,255));

    //draw the input1 diagram

    drawXYDiagram(&mInput1Diag,ui->sliderInput1->value(),mActiveModuleInput1);
    drawXYDiagram(&mInput2Diag,ui->sliderInput2->value(),mActiveModuleInput2);
    setInputModule(1,mActiveModuleInput2);
    drawXYDiagram(&mOutputDiag,ui->sliderOutput->value(),(mOutputCache));
}
void MFNoiseCombinor::setInputModule(int index, noise::module::Module *input){
    mCombinerAdd.SetSourceModule(index,*input);
    mCombinerMax.SetSourceModule(index,*input);
    mCombinerMin.SetSourceModule(index,*input);
    mCombinerMultiply.SetSourceModule(index,*input);
    mCombinerPower.SetSourceModule(index,*input);
}

noise::module::Module *MFNoiseCombinor::getActiveIput2Module(){
    int listType = ui->comboBoxType2->currentIndex();
    switch(listType){
    case INDEX_GENERATOR:
        return mPGeneratorVector[0][ui->listWidgetInput2->currentRow()]->getSourceModule();
        break;
    case INDEX_COMBINER:
        return mPCombinerVector[0][ui->listWidgetInput2->currentRow()]->getSourceModule(mOutputCache);
        break;
    case INDEX_MODIFIER:
        return mPModifierVector[0][ui->listWidgetInput2->currentRow()]->getSourceModule(mOutputCache);
        break;
    case INDEX_SELECTOR:
        return mPSelectorVector[0][ui->listWidgetInput2->currentRow()]->getSourceModule(mOutputCache);
        break;
    case INDEX_TRANSFORMER:
        return mPTransformerVector[0][ui->listWidgetInput2->currentRow()]->getSourceModule(mOutputCache);
        break;
    }
}
noise::module::Module* MFNoiseCombinor::getSourceModule(noise::module::Module *myID)
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
bool MFNoiseCombinor::checkForSameInputPointer(noise::module::Module *pointerToCheck, noise::module::Module *srcModule){
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


void MFNoiseCombinor::on_sliderInput1_sliderMoved(int position)
{
    drawXYDiagram(&mInput1Diag,position,mActiveModuleInput1);
}

void MFNoiseCombinor::on_sliderInput2_sliderMoved(int position)
{
    drawXYDiagram(&mInput2Diag,position,mActiveModuleInput2);
}

void MFNoiseCombinor::on_sliderOutput_sliderMoved(int position)
{
    drawXYDiagram(&mOutputDiag,position,(mOutputCache));
}

void MFNoiseCombinor::on_buttonView_clicked()
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

bool MFNoiseCombinor::readInputValues(){
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
bool MFNoiseCombinor::readFromLineEdit(QStringList *data, QLineEdit *lineEdit){
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
