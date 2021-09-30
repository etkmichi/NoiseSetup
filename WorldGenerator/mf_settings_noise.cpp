#include "mf_settings_noise.h"
#include "ui_mf_noise_settings.h"
#include <fstream>
#include <string>
#include <iostream>


#include <QCheckBox>

MFNoiseSettings::MFNoiseSettings(QWidget *parent) :
    QWidget(parent),
    ui(new Ui::MFNoiseSettings)
{
    ui->setupUi(this);
    this->setWindowOpacity(0.5);
    ui->comboNoiseModule->setCurrentIndex(0);
    mPActiveWidgetList = ui->listGenerator;
    mNoiseGeneratorDummy = new MFNoiseGenerator(0,QString("DummyG"));
    mNoiseCombinerDummy = new MFNoiseCombinor(0,tr("DummyC"));
    mNoiseModifierDummy = new MFNoiseModifier(0,tr("DummyM"));
    mNoiseSelectorDummy = new MFNoiseSelector(0,tr("DummyS"));
    mNoiseTransformerDummy = new MFNoiseTransformer(0,tr("DummyT"));
    ui->layoutVEdit->insertWidget(0,mNoiseGeneratorDummy);
    mPActiveNoiseGenerator  = mNoiseGeneratorDummy;
    mPActiveNoiseCombiner   = mNoiseCombinerDummy;
    mPActiveNoiseModifier   = mNoiseModifierDummy;
    mPActiveNoiseSelector   = mNoiseSelectorDummy;
    mPActiveNoiseTransformer = mNoiseTransformerDummy;
    hideAll();
    mPActiveNoiseGenerator->show();
    mPActiveNoiseGenerator->setDisabled(true);

    this->adjustSize();
}

MFNoiseSettings::~MFNoiseSettings()
{
    for(uint i=0;i<mNoiseListGenerator.size();i++){
        delete(mNoiseListGenerator[i]);
        mNoiseListGenerator[i]=0;
    }
    mNoiseListGenerator.clear();
    for(uint i=0;i<mNoiseListCombiner.size();i++){
        mNoiseListCombiner[i]->~MFNoiseCombinor();
        mNoiseListCombiner[i]->deleteLater();
        mNoiseListCombiner[i]=0;
    }
    mNoiseListCombiner.clear();
    delete ui;
}

void MFNoiseSettings::on_btnNewNoise_clicked()
{
    //change the active list widget to configure it
    bool ok;
    QString text = QInputDialog::getText(this, tr("QInputDialog::getText()"),
                                         tr("Module name:"), QLineEdit::Normal,
                                         "",&ok);
    if(!ok)return;
    switch(ui->comboNoiseModule->currentIndex()){
    case 0://Generator Module
        makeGeneratorModule(text);
        break;
    case 1://Combinor
        makeCombinorModule(text);
        break;
    case 2://Modifier
        makeModifierModule(text);
        break;
    case 3://Selector
        makeSelectorModule(text);
        break;
    case 4://Transformer
        makeTransformerModule(text);
        break;

    default:
        makeGeneratorModule(text);
        break;
    }
}

void MFNoiseSettings::on_btnDelete_clicked()
{
    //change the active list widget to configure it
    switch(ui->comboNoiseModule->currentIndex()){
    case 0://Generator Module
        deleteGeneratorModule();
        break;
    case 1://Combinor
        deleteCombinorModule();
        break;
    case 2://Modifier
        deleteModifierModule();
        break;
    case 3://Selector
        deleteSelectorModule();
        break;
    case 4://Transformer
        deleteTransformerModule();
        break;

    default:
        break;
    }
}



void MFNoiseSettings::on_btnSave_clicked()
{
    QString name =QFileDialog::getSaveFileName();
    if(name.isEmpty())
        return;
    std::ofstream write(name.toStdString().data());

    for(uint i = 0; i<mNoiseListGenerator.size();i++){
        qint32 check = 0xCAFEBABE;
        write.write((char*)(&check),4);

        int size = sizeof(MFNoiseGenerator::ms_generatorSettings);
        MFNoiseGenerator::ms_generatorSettings settings = (((MFNoiseGenerator*)(mNoiseListGenerator[i]))->getGeneratorSettings());
        write.write((char*)(&size),4);
        write.write((char*)(&settings),size);

        size = sizeof(glm::vec3)*settings.colors.size();
        write.write((char*)(&size),4);
        write.write((char*)(settings.colors.data()),size);

        size = settings.name.size();
        write.write((char*)(&size),4);
        write.write(settings.name.data(),size);
    }
    int end_check = 0x000CEBAB;
    write.write((char*)(&end_check),4);
    write.close();
}

void MFNoiseSettings::on_btnOpen_clicked()
{
    //get file access
    QString name =QFileDialog ::getOpenFileName();
    if(name.isEmpty())
        return;
    std::ifstream read(name.toStdString().data());

    //determine the size
    read.seekg(0,std::ios::end);
    int size = read.tellg();
    read.close();
    read.open(name.toStdString().data());

    //copy the data
    if(size >1024*1024*5)
        return;
    char *data = ((char*)std::calloc(size,1));
    read.read(data,size);
    //data: |4byte compare|4byte size settings|size_settings byte settings|4 byte size color|size_color byte colors|repeat
    quint32 compare_0=(*((quint32*)(data)));
    quint32 compare_1=0xCAFEBABE;
    MFNoiseGenerator::ms_generatorSettings settings;
    while(compare_0 == compare_1){
        //settings
        int settings_size = *((int*)(data+4));//size of settings is stored in the second int
        settings =*((MFNoiseGenerator::ms_generatorSettings*)((data+8)));
        //colors
        int colors_size = *((int*)(data+8+settings_size));
        std::vector<glm::vec3> colors;
        for(int i =0;i<colors_size;i+=sizeof(glm::vec3)){
            glm::vec3 vec;
            vec = *((glm::vec3*)(data+12+settings_size+i));
            colors.push_back(vec);
        }
        settings.colors = colors;

        //name
        int size_char =*((int*)(data+12+settings_size+colors_size));
        std::string name=std::string((data+16+settings_size+colors_size),size_char);
        settings.name = name;
        //compare for others
        compare_0 = *((int*)(data+16+settings_size+colors_size+size_char));
        if(compare_0 == compare_1){
            data = data+16+settings_size+colors_size+size_char;
        }
        //make a generator module
        makeGeneratorModule(QString::fromStdString(name));
        //the last activated module:
        mPActiveNoiseGenerator->setGeneratorSettings(settings);
    }
    read.close();

}

//the noise module drop down menu will change the active QListWdgt and vector<MFNoiseGenerator*>
void MFNoiseSettings::on_comboNoiseModule_currentIndexChanged(int index)
{
    //change the active list widget to configure it
    switch(index){
    case 0://Generator Module
        mPActiveWidgetList = ui->listGenerator;
        break;
    case 1://Combinor
        mPActiveWidgetList = ui->listCombiner;
        break;
    case 2://Modifier
        mPActiveWidgetList = ui->listModifier;
        break;
    case 3://Selector
        mPActiveWidgetList = ui->listSelector;
        break;
    case 4://Transformer
        mPActiveWidgetList = ui->listTransformer;
        break;

    default:
        mPActiveWidgetList = ui->listGenerator;
        break;
    }
}


void MFNoiseSettings::makeCombinorModule(QString text){
    hideAll();
    //make a unique id to connect the object in the vector to the list item
    int indexID=mNoiseListCombiner.size(); //deleting an item must rename all following items
    QString itemText=QString::number(indexID) +"_"+text;

    //Add  noise to the active list and to the vector (the item has to be found in both)
    QListWidgetItem *item = new QListWidgetItem(itemText);
    ui->listCombiner->insertItem(ui->listCombiner->count(),item);
    //to the vector
    MFNoiseCombinor *aCombiner = new MFNoiseCombinor(indexID,text,this);
    aCombiner->setGeneratorVectorPointer(&mNoiseListGenerator);
    aCombiner->setCombinerVectorPointer(&mNoiseListCombiner);
    aCombiner->setModifierVectorPointer(&mNoiseListModifier);
    aCombiner->setSelectorVectorPointer(&mNoiseListSelector);
    aCombiner->setTransformerVectorPointer(&mNoiseListTransformer);
    mPActiveNoiseCombiner = aCombiner;

    mNoiseListCombiner.push_back(aCombiner);

    //show the new noise edit on the right layout and remove the last one
    ui->layoutVEdit->takeAt(0);
    ui->layoutVEdit->insertWidget(0,mPActiveNoiseCombiner);
    hideAll();
    mPActiveNoiseCombiner->show();
}
void MFNoiseSettings::on_listCombiner_activated(const QModelIndex &index)
{
    //Clicking will change the editing dialog to the current item at index
    //remove and hide the last item from the layout
    ui->layoutVEdit->takeAt(0);
    hideAll();
    //set the active list widget
    mPActiveWidgetList = ui->listCombiner;
    //get the index of to the edit (its saved in the name)
    int vectorIndex = QString(ui->listCombiner->currentItem()->text().split("_").at(0)).toInt();

    //between this and ui->layoutVEdit->insertWidget(0,mPActiveNoiseCombiner); has to bee some other code
    mPActiveNoiseCombiner = mNoiseListCombiner[vectorIndex];
    ui->comboNoiseModule->setCurrentIndex(INDEX_COMBINER);
    ui->layoutVEdit->insertWidget(0,mPActiveNoiseCombiner);
    mPActiveNoiseCombiner->updateInputLists();
    mPActiveNoiseCombiner->show();
}
void MFNoiseSettings::deleteCombinorModule(){
    if(!ui->listCombiner->currentIndex().isValid() || (mPActiveNoiseCombiner == mNoiseCombinerDummy))
        return;
    uint size = mNoiseListCombiner.size();
    QListWidget *listW=ui->listCombiner;
    //get the connection of the list item to the vector<MFNoiseGenerator> index and delete it
    uint vectorIndex = listW->currentItem()->text().split("_").at(0).toInt();
    ui->layoutVEdit->takeAt(0);
    hideAll();
    mNoiseListCombiner[vectorIndex]->hide();
    mNoiseListCombiner[vectorIndex]->deleteLater();
    mNoiseListCombiner.erase(mNoiseListCombiner.begin()+vectorIndex);
    listW->currentItem()->~QListWidgetItem();

    //rename the list items to have a track to the vector items
    for(uint i = 0; i < mNoiseListCombiner.size() ; i++)
    {
        //go through the whole list and rename all indexes which ar higher than vectorIndex
        int index = listW->item(i)->text().split("_").at(0).toInt();
        if(index > vectorIndex){//all indexes above
            QString rmStr =listW->item(i)->text().split("_").at(0)+tr("_");//remove string
            QString exStr = QString::number(index-1)+tr("_");//exchange string
            QString text = listW->item(i)->text().replace(rmStr,exStr);
            listW->item(i)->setText(text);
        }
    }
    if(vectorIndex>=size){
        mPActiveNoiseCombiner = mNoiseCombinerDummy;
        mNoiseCombinerDummy->setEnabled(false);
    }else{
        mPActiveNoiseCombiner = mNoiseListCombiner[vectorIndex];
    }

    listW->setCurrentRow(vectorIndex);
    ui->layoutVEdit->insertWidget(0,mPActiveNoiseCombiner);
    mPActiveNoiseCombiner->show();
}

void MFNoiseSettings::makeModifierModule(QString text){
    hideAll();
    //make a unique id to connect the object in the vector to the list item
    int indexID=mNoiseListModifier.size(); //deleting an item must rename all following items
    QString itemText=QString::number(indexID) +"_"+text;

    //Add  noise to the active list and to the vector (the item has to be found in both)
    QListWidgetItem *item = new QListWidgetItem(itemText);
    ui->listModifier->insertItem(ui->listModifier->count(),item);
    //to the vector
    MFNoiseModifier *aModifier = new MFNoiseModifier(indexID,text,this);
    aModifier->setGeneratorVectorPointer(&mNoiseListGenerator);
    aModifier->setCombinorVectorPointer(&mNoiseListCombiner);
    aModifier->setModifierVectorPointer(&mNoiseListModifier);
    aModifier->setSelectorVectorPointer(&mNoiseListSelector);
    aModifier->setTransformerVectorPointer(&mNoiseListTransformer);
    mPActiveNoiseModifier = aModifier;

    mNoiseListModifier.push_back(aModifier);

    //show the new noise edit on the right layout and remove the last one
    ui->layoutVEdit->takeAt(0);
    ui->layoutVEdit->insertWidget(0,mPActiveNoiseModifier);
    hideAll();
    mPActiveNoiseModifier->show();
}
void MFNoiseSettings::on_listModifier_activated(const QModelIndex &index)
{
    //Clicking will change the editing dialog to the current item at index
    //remove and hide the last item from the layout
    ui->layoutVEdit->takeAt(0);
    hideAll();
    //set the active list widget
    mPActiveWidgetList = ui->listModifier;
    //get the index of to the edit (its saved in the name)
    int vectorIndex = QString(mPActiveWidgetList->currentItem()->text().split("_").at(0)).toInt();

    //between this and ui->layoutVEdit->insertWidget(0,mPActiveNoiseGenerator); has to bee some other code
    //because a reason
    mPActiveNoiseModifier = mNoiseListModifier[vectorIndex];
    ui->comboNoiseModule->setCurrentIndex(INDEX_MODIFIER);
    ui->layoutVEdit->insertWidget(0,mPActiveNoiseModifier);
    mPActiveNoiseModifier->updateInputLists();
    mPActiveNoiseModifier->show();
}
void MFNoiseSettings::deleteModifierModule(){
    QListWidget *listW=ui->listModifier;
    if(!listW->currentIndex().isValid() || (mPActiveNoiseModifier == mNoiseModifierDummy))
        return;
    uint size = mNoiseListCombiner.size();
    //get the connection of the list item to the vector<MFNoiseGenerator> index and delete it
    uint vectorIndex = listW->currentItem()->text().split("_").at(0).toInt();
    ui->layoutVEdit->takeAt(0);
    hideAll();
    mNoiseListModifier[vectorIndex]->hide();
    mNoiseListModifier[vectorIndex]->deleteLater();
    mNoiseListModifier.erase(mNoiseListModifier.begin()+vectorIndex);
    listW->currentItem()->~QListWidgetItem();

    //rename the list items to have a track to the vector items
    for(uint i = 0; i < mNoiseListModifier.size() ; i++)
    {
        //go through the whole list and rename all indexes which ar higher than vectorIndex
        int index = listW->item(i)->text().split("_").at(0).toInt();
        if(index > vectorIndex){//all indexes above
            QString rmStr =listW->item(i)->text().split("_").at(0)+tr("_");//remove string
            QString exStr = QString::number(index-1)+tr("_");//exchange string
            QString text = listW->item(i)->text().replace(rmStr,exStr);
            listW->item(i)->setText(text);
        }
    }
    if(vectorIndex>=size){
        mPActiveNoiseModifier = mNoiseModifierDummy;
        mNoiseModifierDummy->setEnabled(false);
    }else{
        mPActiveNoiseModifier = mNoiseListModifier[vectorIndex];
    }

    listW->setCurrentRow(vectorIndex);
    ui->layoutVEdit->insertWidget(0,mPActiveNoiseModifier);
    mPActiveNoiseModifier->show();
}

void MFNoiseSettings::makeGeneratorModule(QString text){
    //get a name for the noise

    hideAll();
    //make a unique id to connect the object in the vector to the list item
    int indexID=mNoiseListGenerator.size(); //deleting an item must rename all following items
    QString itemText=QString::number(indexID) +"_"+text;

    //Add  noise to the active list and to the vector (the item has to be found in both)
    QListWidgetItem *item = new QListWidgetItem(itemText);
    ui->listGenerator->insertItem(ui->listGenerator->count(),item);
    //to the vector
    MFNoiseGenerator *aNoise = new MFNoiseGenerator(indexID,text,this);
    mPActiveNoiseGenerator = aNoise;

    mNoiseListGenerator.push_back(aNoise);

    //show the new noise edit on the right layout and remove the last one
    ui->layoutVEdit->takeAt(0);
    ui->layoutVEdit->insertWidget(0,mPActiveNoiseGenerator);
    mNoiseGeneratorDummy->hide();
}
void MFNoiseSettings::deleteGeneratorModule(){
    if(!ui->listGenerator->currentIndex().isValid() || (mPActiveNoiseGenerator == mNoiseGeneratorDummy))
        return;

    //get the connection of the list item to the vector<MFNoiseGenerator> index and delete it
    uint vectorIndex = ui->listGenerator->currentItem()->text().split("_").at(0).toInt();
    ui->layoutVEdit->takeAt(0);
    hideAll();
    mNoiseListGenerator[vectorIndex]->hide();
    mNoiseListGenerator[vectorIndex]->deleteLater();
    mNoiseListGenerator.erase(mNoiseListGenerator.begin()+vectorIndex);
    ui->listGenerator->currentItem()->~QListWidgetItem();

    //rename the list items to have a track to the vector items
    for(uint i = 0; i < mNoiseListGenerator.size() ; i++)
    {
        //go through the whole list and rename all indexes which ar higher than vectorIndex
        int index = ui->listGenerator->item(i)->text().split("_").at(0).toInt();
        if(index > vectorIndex){//all indexes above
            QString rmStr =ui->listGenerator->item(i)->text().split("_").at(0)+tr("_");//remove string
            QString exStr = QString::number(index-1)+tr("_");//exchange string
            QString text = ui->listGenerator->item(i)->text().replace(rmStr,exStr);
            ui->listGenerator->item(i)->setText(text);
        }
    }
    if(vectorIndex>=mNoiseListGenerator.size()){
        mPActiveNoiseGenerator = mNoiseGeneratorDummy;
        mNoiseGeneratorDummy->setEnabled(false);
    }else{
        mPActiveNoiseGenerator = mNoiseListGenerator[vectorIndex];
    }

    ui->listGenerator->setCurrentRow(vectorIndex);
    ui->layoutVEdit->insertWidget(0,mPActiveNoiseGenerator);
    mPActiveNoiseGenerator->show();
}
void MFNoiseSettings::on_listGenerator_activated(const QModelIndex &index)
{
    //Clicking will change the editing dialog to the current item at index
    //remove and hide the last item from the layout
    ui->layoutVEdit->takeAt(0);
    hideAll();
    //set the active list widget
    mPActiveWidgetList = ui->listGenerator;
    //get the index of to the edit (its saved in the name)
    int vectorIndex = QString(mPActiveWidgetList->currentItem()->text().split("_").at(0)).toInt();

    //between this and ui->layoutVEdit->insertWidget(0,mPActiveNoiseGenerator); has to bee some other code
    mPActiveNoiseGenerator = mNoiseListGenerator[vectorIndex];
    ui->comboNoiseModule->setCurrentIndex(0);
    ui->layoutVEdit->insertWidget(0,mPActiveNoiseGenerator);
    mPActiveNoiseGenerator->show();
}

void MFNoiseSettings::makeTransformerModule(QString text){
    ui->layoutVEdit->takeAt(0);
    hideAll();
    //make a unique id to connect the object in the vector to the list item
    int indexID=mNoiseListTransformer.size(); //deleting an item must rename all following items
    QString itemText=QString::number(indexID) +"_"+text;

    //Add  noise to the active list and to the vector (the item has to be found in both)
    QListWidgetItem *item = new QListWidgetItem(itemText);
    ui->listTransformer->insertItem(ui->listTransformer->count(),item);
    //to the vector
    MFNoiseTransformer *aTransformer = new MFNoiseTransformer(indexID,text,this);
    aTransformer->setGeneratorVectorPointer(&mNoiseListGenerator);
    aTransformer->setCombinerVectorPointer(&mNoiseListCombiner);
    aTransformer->setModifierVectorPointer(&mNoiseListModifier);
    aTransformer->setSelectorVectorPointer(&mNoiseListSelector);
    aTransformer->setTransformerVectorPointer(&mNoiseListTransformer);
    mPActiveNoiseTransformer = aTransformer;

    mNoiseListTransformer.push_back(aTransformer);

    //show the new noise edit on the right layout and remove the last one
    ui->layoutVEdit->insertWidget(0,aTransformer);
}
void MFNoiseSettings::deleteTransformerModule(){
    if(!ui->listTransformer->currentIndex().isValid() || (mPActiveNoiseTransformer == mNoiseTransformerDummy))
        return;

    //get the connection of the list item to the vector<MFNoiseGenerator> index and delete it
    int vectorIndex = ui->listTransformer->currentItem()->text().split("_").at(0).toInt();
    ui->layoutVEdit->takeAt(0);
    mNoiseListTransformer[vectorIndex]->hide();
    hideAll();
    mNoiseListTransformer[vectorIndex]->deleteLater();
    mNoiseListTransformer.erase(mNoiseListTransformer.begin()+vectorIndex);
    ui->listTransformer->currentItem()->~QListWidgetItem();

    //rename the list items to have a track to the vector items
    for(uint i = 0; i < mNoiseListTransformer.size() ; i++)
    {
        //go through the whole list and rename all indexes which ar higher than vectorIndex
        int index = ui->listTransformer->item(i)->text().split("_").at(0).toInt();
        if(index > vectorIndex){//all indexes above
            QString rmStr =ui->listTransformer->item(i)->text().split("_").at(0)+tr("_");//remove string
            QString exStr = QString::number(index-1)+tr("_");//exchange string
            QString text = ui->listTransformer->item(i)->text().replace(rmStr,exStr);
            ui->listTransformer->item(i)->setText(text);
        }
    }
    if(vectorIndex>=mNoiseListTransformer.size()){
        mPActiveNoiseTransformer = mNoiseTransformerDummy;
        mNoiseTransformerDummy->setEnabled(false);
    }else{
        mPActiveNoiseTransformer = mNoiseListTransformer[vectorIndex];
    }

    ui->listTransformer->setCurrentRow(vectorIndex);
    ui->layoutVEdit->insertWidget(0,mPActiveNoiseTransformer);
    mPActiveNoiseTransformer->show();
}

void MFNoiseSettings::makeSelectorModule(QString text){
    hideAll();
    //make a unique id to connect the object in the vector to the list item
    int indexID=mNoiseListSelector.size(); //deleting an item must rename all following items
    QString itemText=QString::number(indexID) +"_"+text;

    //Add  noise to the active list and to the vector (the item has to be found in both)
    QListWidgetItem *item = new QListWidgetItem(itemText);
    ui->listSelector->insertItem(ui->listSelector->count(),item);
    //to the vector
    MFNoiseSelector *aSelector = new MFNoiseSelector(indexID,text,this);
    aSelector->setGeneratorVectorPointer(&mNoiseListGenerator);
    aSelector->setCombinerVectorPointer(&mNoiseListCombiner);
    aSelector->setModifierVectorPointer(&mNoiseListModifier);
    aSelector->setSelectorVectorPointer(&mNoiseListSelector);
    aSelector->setTransformerVectorPointer(&mNoiseListTransformer);
    mPActiveNoiseSelector = aSelector;

    mNoiseListSelector.push_back(aSelector);

    //show the new noise edit on the right layout and remove the last one
    ui->layoutVEdit->takeAt(0);
    ui->layoutVEdit->insertWidget(0,mPActiveNoiseSelector);
}
void MFNoiseSettings::deleteSelectorModule(){
    if(!ui->listSelector->currentIndex().isValid() || (mPActiveNoiseSelector == mNoiseSelectorDummy))
        return;

    //get the connection of the list item to the vector<MFNoiseGenerator> index and delete it
    int vectorIndex = ui->listSelector->currentItem()->text().split("_").at(0).toInt();
    ui->layoutVEdit->takeAt(0);
    mNoiseListSelector[vectorIndex]->hide();
    hideAll();
    mNoiseListSelector[vectorIndex]->deleteLater();
    mNoiseListSelector.erase(mNoiseListSelector.begin()+vectorIndex);
    ui->listSelector->currentItem()->~QListWidgetItem();

    //rename the list items to have a track to the vector items
    for(uint i = 0; i < mNoiseListSelector.size() ; i++)
    {
        //go through the whole list and rename all indexes which ar higher than vectorIndex
        int index = ui->listSelector->item(i)->text().split("_").at(0).toInt();
        if(index > vectorIndex){//all indexes above
            QString rmStr =ui->listSelector->item(i)->text().split("_").at(0)+tr("_");//remove string
            QString exStr = QString::number(index-1)+tr("_");//exchange string
            QString text = ui->listSelector->item(i)->text().replace(rmStr,exStr);
            ui->listSelector->item(i)->setText(text);
        }
    }
    if(vectorIndex>=mNoiseListSelector.size()){
        mPActiveNoiseSelector = mNoiseSelectorDummy;
        mNoiseSelectorDummy->setEnabled(false);
    }else{
        mPActiveNoiseSelector = mNoiseListSelector[vectorIndex];
    }

    ui->listSelector->setCurrentRow(vectorIndex);
    ui->layoutVEdit->insertWidget(0,mPActiveNoiseSelector);
    mPActiveNoiseSelector->show();
}
void MFNoiseSettings::on_listSelector_activated(const QModelIndex &index)
{
    //Clicking will change the editing dialog to the current item at index
    //remove and hide the last item from the layout
    ui->layoutVEdit->takeAt(0);
    hideAll();
    //set the active list widget
    mPActiveWidgetList = ui->listSelector;
    //get the index of to the edit (its saved in the name)
    int vectorIndex = QString(mPActiveWidgetList->currentItem()->text().split("_").at(0)).toInt();

    //between this and ui->layoutVEdit->insertWidget(0,mPActiveNoiseGenerator); has to bee some other code
    mPActiveNoiseSelector = mNoiseListSelector[vectorIndex];
    ui->comboNoiseModule->setCurrentIndex(INDEX_SELECTOR);
    ui->layoutVEdit->insertWidget(0,mPActiveNoiseSelector);
    mPActiveNoiseSelector->updateInputLists();
    mPActiveNoiseSelector->show();
}

void MFNoiseSettings::hideAll(){
    mNoiseCombinerDummy->hide();
    mNoiseGeneratorDummy->hide();
    mNoiseModifierDummy->hide();
    mNoiseSelectorDummy->hide();
    mPActiveNoiseCombiner->hide();
    mPActiveNoiseGenerator->hide();
    mPActiveNoiseModifier->hide();
    mPActiveNoiseSelector->hide();
    mNoiseTransformerDummy->hide();
    mPActiveNoiseTransformer->hide();
}

void MFNoiseSettings::on_listTransformer_activated(const QModelIndex &index)
{
    //Clicking will change the editing dialog to the current item at index
    //remove and hide the last item from the layout
    ui->layoutVEdit->takeAt(0);
    hideAll();
    //set the active list widget
    mPActiveWidgetList = ui->listTransformer;
    //get the index of to the edit (its saved in the name)
    int vectorIndex = QString(mPActiveWidgetList->currentItem()->text().split("_").at(0)).toInt();

    //between this and ui->layoutVEdit->insertWidget(0,mPActiveNoiseGenerator); has to bee some other code
    mPActiveNoiseTransformer = mNoiseListTransformer[vectorIndex];
    ui->comboNoiseModule->setCurrentIndex(INDEX_TRANSFORMER);
    ui->layoutVEdit->insertWidget(0,mPActiveNoiseTransformer);
    mPActiveNoiseTransformer->updateInputLists();
    mPActiveNoiseTransformer->show();
}
