#include "mainwindow.h"
#include "ui_mainwindow.h"

MainWindow::MainWindow(QWidget *parent) :
    QMainWindow(parent),
    ui(new Ui::MainWindow)
{
    ui->setupUi(this);
    m_noiseGUI = new MFNoiseSettings();
    m_noiseGUI->show();
}

MainWindow::~MainWindow()
{
    delete m_noiseGUI;
    delete ui;
}
