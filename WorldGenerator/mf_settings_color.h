#ifndef MF_COLOR_SETTINGS_H
#define MF_COLOR_SETTINGS_H

#include <QDialog>
#include <QColorDialog>
#include <glm/glm.hpp>

namespace Ui {
class MFColorSettings;
}

class MFColorSettings : public QDialog
{
    Q_OBJECT

public:
    explicit MFColorSettings(QWidget *parent = 0);
    ~MFColorSettings();
    std::vector<glm::vec3> getColorVector();
    void setColorVector(std::vector<glm::vec3> colors);
    int gradientCount();
signals:
    void colorListChanged();


private slots:
    void on_pushButtonAdd_clicked();

    void on_pushButtonEdit_clicked();

    void on_pushButtonDelete_clicked();

    void on_pushButtonUp_clicked();

    void on_pushButtonDown_clicked();

private:
    std::vector<glm::vec3> mGradientColors;
    int colorCounter=0;
    Ui::MFColorSettings *ui;
};

#endif // MF_COLOR_SETTINGS_H
