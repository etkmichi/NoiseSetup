#ifndef MAINWINDOW_H
#define MAINWINDOW_H


#include <QMainWindow>
#include <QLineEdit>
#include <QStringList>
#include <QLabel>
#include <QThread>
#include "mf_settings_noise.h"
#include "mf_signal_ctrl.h"

#include "mf_settings_color.h"
#include "mf_map_generator.h"
#include "mf_thread_noise.h"
#include "helper/mf_structs.h"
#include "helper/mf_def.h"

namespace Ui {
class MainWindow;
}

class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    explicit MainWindow(QWidget *parent = 0);
    ~MainWindow();

signals:
    void saving();

private slots:
    void on_buttonSaveAs_clicked();
    void updateStatusBar();
    void enableButtons();
    void stopCalcuation();

private:
    Ui::MainWindow *ui;
    MFNoiseSettings *mNoiseSettings;

    bool saveFile(const QString &fileName);
    void createStatusBar();

    std::string errMsg;


    //GUI
    QLabel *statusLabel,*formulaLabel;
    QString statusText, formulaText;
};

#endif // MAINWINDOW_H
