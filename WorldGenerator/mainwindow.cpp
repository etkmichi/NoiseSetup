#include "mainwindow.h"
#include "ui_mainwindow.h"
#include "iostream"
#include "noise/noise.h"
#include <stdlib.h>
#include <QFileDialog>
#include <QTextStream>

MainWindow::MainWindow(QWidget *parent) :
    QMainWindow(parent),
    ui(new Ui::MainWindow),
    statusText("Idle"),
    formulaText("----")
{
    ui->setupUi(this);

    mNoiseSettings =new MFNoiseSettings();
    mNoiseSettings->setGeometry(this->width()+20,40,600,800);
    mNoiseSettings->show();

    this->setMinimumSize(800,600);
}

MainWindow::~MainWindow()
{
    delete ui;
}

void MainWindow::stopCalcuation(){
//    ui->buttonStop->setEnabled(false);
//    //mSigCtl->emitStopThread();
//    int counter =0;
//    mMapThread->terminate();
//    while(!mMapThread->isFinished())counter--;

//    if(mMapThread != 0){
//        mMapThread->~MFThreadNoise();
//    }

//    if(mMapGen != 0){
//        mMapGen->~MFMapGenerator();
//    }

//    mMapGen = new MFMapGenerator(this);
//    mMapThread = new MFThreadNoise(mSigCtl,mMapGen,this);

//    mMapThread->start();
//    enableButtons();
//    ui->buttonView->setEnabled(false);
//    ui->buttonSaveAs->setEnabled(false);
}

void MainWindow::enableButtons(){
//    ui->buttonGenerate->setEnabled(true);
//    ui->actionS_ave_As->setEnabled(true);
//    ui->buttonSaveAs->setEnabled(true);
//    ui->actionGenerate->setEnabled(true);
//    ui->buttonStop->setEnabled(false);
//    ui->buttonView->setEnabled(true);
}

void MainWindow::createStatusBar(){
//    statusLabel = new QLabel(" Idle ");
//    statusLabel->setAlignment(Qt::AlignCenter);

//    formulaLabel = new QLabel;
//    formulaLabel->setIndent(3);

//    statusBar()->addWidget(statusLabel);
//    statusBar()->addWidget(formulaLabel,1);
}
void MainWindow::updateStatusBar(){
//    statusLabel->setText(statusText);
//    formulaLabel->setText(formulaText);
}


void MainWindow::on_buttonSaveAs_clicked()
{
//    ui->buttonGenerate->setDisabled(true);
//    ui->actionS_ave_As->setDisabled(true);
//    ui->buttonSaveAs->setDisabled(true);
//    ui->buttonStop->setEnabled(true);
//    ui->buttonView->setDisabled(true);

//    QString fileName = QFileDialog::getSaveFileName(this, tr("Save File"),
//                                 "/home/michl/untitled.obj",
//                                 tr("Images (*.obj)"));
    //mMapThread->saveAs(fileName);
}


