#include "mf_settings_color.h"
#include "ui_mf_color_settings.h"

MFColorSettings::MFColorSettings(QWidget *parent) :
    QDialog(parent),
    ui(new Ui::MFColorSettings)
{
    ui->setupUi(this);
    this->setWindowTitle("Color settings");

    //check if valid gui settings
    QColor color = Qt::black;
    QString colorString = tr("R:")+QString::number(color.red())+tr(" G:")+QString::number(color.green())+tr(" B:")+QString::number(color.blue());

    ui->listWidget->addItem(colorString);
    ui->listWidget->item(0)->setBackground(QBrush(color));
    mGradientColors.insert(mGradientColors.cbegin(),glm::vec3(color.redF(),color.greenF(),color.blueF()));
    colorCounter++;

    color = Qt::red;
    colorString = tr("R:")+QString::number(color.red())+tr(" G:")+QString::number(color.green())+tr(" B:")+QString::number(color.blue());

    ui->listWidget->addItem(colorString);
    ui->listWidget->item(1)->setBackground(QBrush(color));
    mGradientColors.insert(mGradientColors.cbegin()+1,glm::vec3(color.redF(),color.greenF(),color.blueF()));
    colorCounter++;

    color = Qt::yellow;
    colorString = tr("R:")+QString::number(color.red())+tr(" G:")+QString::number(color.green())+tr(" B:")+QString::number(color.blue());

    ui->listWidget->addItem(colorString);
    ui->listWidget->item(2)->setBackground(QBrush(color));
    mGradientColors.insert(mGradientColors.cbegin()+2,glm::vec3(color.redF(),color.greenF(),color.blueF()));
    colorCounter++;

    ui->listWidget->addItem("last position");
    emit colorListChanged();
}

MFColorSettings::~MFColorSettings()
{
    delete ui;
}

void MFColorSettings::on_pushButtonAdd_clicked()
{
    if(colorCounter<100){
        //check if valid gui settings
        int pos = ui->listWidget->currentRow();
        if( pos<0 || pos > ui->listWidget->count())
            pos=ui->listWidget->count()-1;

        QColor color;
        if(colorCounter<1)
            color = QColorDialog::getColor(Qt::black,this,tr("Choos color"));
        else
            color = QColorDialog::getColor(QColor(mGradientColors[pos][0]*255,\
                                            mGradientColors[pos][1]*255,\
                                            mGradientColors[pos][2]*255),0,tr("Choos color"));

        if(!color.isValid())
            return;
        QString colorString = tr("R:")+QString::number(color.red())+tr(" G:")+QString::number(color.green())+tr(" B:")+QString::number(color.blue());



        ui->listWidget->insertItem(pos,colorString);
        ui->listWidget->item(pos)->setBackground(QBrush(color));
        mGradientColors.insert(mGradientColors.cbegin()+pos,glm::vec3(color.redF(),color.greenF(),color.blueF()));
        colorCounter++;
        emit colorListChanged();
    }
}
int MFColorSettings::gradientCount(){
    return mGradientColors.size();
}

void MFColorSettings::on_pushButtonEdit_clicked()
{
    int pos = ui->listWidget->currentRow();
    if(pos<=-1 || pos>=(ui->listWidget->count()-1))
        return;

    QColor color =QColorDialog::getColor(QColor(mGradientColors[pos][0]*255,\
                                         mGradientColors[pos][1]*255,\
                                         mGradientColors[pos][2]*255),0,tr("Change color"));
    if(!color.isValid())
        return;

    QString colorString = tr("R:")+QString::number(color.red())+tr(" G:")+QString::number(color.green())+tr(" B:")+QString::number(color.blue());



    ui->listWidget->takeItem(pos);
    ui->listWidget->insertItem(pos,colorString);
    ui->listWidget->item(pos)->setBackground(QBrush(color));
    ui->listWidget->setCurrentRow(pos);

    mGradientColors[pos] = glm::vec3(color.redF(),color.greenF(),color.blueF());
    emit colorListChanged();
}

void MFColorSettings::on_pushButtonDelete_clicked()
{
    int pos = ui->listWidget->currentRow();
    if(pos==-1 || pos>=ui->listWidget->count()-1)
        return;

    ui->listWidget->takeItem(pos);
    mGradientColors.erase(mGradientColors.cbegin()+pos);
    emit colorListChanged();
}

std::vector<glm::vec3> MFColorSettings::getColorVector(){
    return mGradientColors;
}

void MFColorSettings::setColorVector(std::vector<glm::vec3> colors){
    ui->listWidget->clear();
    for(int i =0; i< colors.size();i++){
        int pos = ui->listWidget->count();
        if( pos<0 || pos > ui->listWidget->count())
            pos=ui->listWidget->count()-1;

        QColor color;
        color = QColor( colors[i][0]*255,
                        colors[i][1]*255,
                        colors[i][2]*255);

        if(!color.isValid())
            return;

        QString colorString =   tr("R:")+QString::number(color.red())+
                                tr(" G:")+QString::number(color.green())+
                                tr(" B:")+QString::number(color.blue());

        ui->listWidget->insertItem(i,colorString);
        ui->listWidget->item(i)->setBackground(QBrush(color));
        colorCounter++;
    }
    mGradientColors = colors;
    ui->listWidget->addItem("last position");
    emit colorListChanged();
}

void MFColorSettings::on_pushButtonUp_clicked()
{
    int pos = ui->listWidget->currentRow();
    if(pos<=0 || pos>=(ui->listWidget->count()-1))
        return;
    ui->listWidget->insertItem(pos-1,ui->listWidget->takeItem(pos));
    ui->listWidget->setCurrentRow(pos-1);
    mGradientColors.insert(mGradientColors.cbegin()+pos-1,mGradientColors[pos]);
    mGradientColors.erase(mGradientColors.cbegin()+pos+1);
    emit colorListChanged();
}

void MFColorSettings::on_pushButtonDown_clicked()
{
    int pos = ui->listWidget->currentRow();
    if(pos<=-1 || pos>=(ui->listWidget->count()-2))
        return;
    ui->listWidget->insertItem(pos+1,ui->listWidget->takeItem(pos));
    ui->listWidget->setCurrentRow(pos+1);
    mGradientColors.insert(mGradientColors.cbegin()+pos+2,glm::vec3(mGradientColors[pos]));
    mGradientColors.erase(mGradientColors.cbegin()+pos);
    emit colorListChanged();
}
