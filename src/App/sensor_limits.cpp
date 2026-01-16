#include "sensor_limits.h"
#include "ui_sensor_limits.h"
#include "mainwindow.h"
#include <cmath>

extern int lang;
extern int temp_unit;

extern int temp_min;
extern int temp_max;

extern int hum_min;
extern int hum_max;

static int to_unit(int n, int sel_temp_unit)
{
    switch (sel_temp_unit)
    {
        case MainWindow::C_TEMP_UNIT:
            return n;
        case MainWindow::F_TEMP_UNIT:
            return std::round(((n - 32) * 5)/9);
        case MainWindow::K_TEMP_UNIT:
            return n - 273;
    }

    return n;
}

static int from_unit(int n, int sel_temp_unit)
{
    switch (sel_temp_unit)
    {
        case MainWindow::C_TEMP_UNIT:
            return n;
        case MainWindow::F_TEMP_UNIT:
            return std::round(n * (9.0 / 5.0) + 32);
        case MainWindow::K_TEMP_UNIT:
            return std::round(n + 273.15);
    }

    return n;
}

sensor_limits::sensor_limits(QWidget *parent) : QDialog(parent), ui(new Ui::sensor_limits)
{
    ui->setupUi(this);

    this->setMaximumSize(this->width(), this->height());
    this->setMinimumSize(this->width(), this->height());

    lang_changed_slot();
}

sensor_limits::~sensor_limits()
{
    delete ui;
}

void sensor_limits::on_pushButton_ok_clicked()
{
    int sel_temp_unit = 0;

    switch (ui->comboBox->currentIndex())
    {
        case 0:
            sel_temp_unit = MainWindow::C_TEMP_UNIT; break;
        case 1:
            sel_temp_unit = MainWindow::F_TEMP_UNIT; break;
        case 2:
            sel_temp_unit = MainWindow::K_TEMP_UNIT; break;
    }

    temp_min = to_unit(ui->lineEdit_temp_min->text().toInt(), sel_temp_unit);

    switch (ui->comboBox_2->currentIndex())
    {
        case 0:
            sel_temp_unit = MainWindow::C_TEMP_UNIT; break;
        case 1:
            sel_temp_unit = MainWindow::F_TEMP_UNIT; break;
        case 2:
            sel_temp_unit = MainWindow::K_TEMP_UNIT; break;
    }

    temp_max = to_unit(ui->lineEdit_temp_max->text().toInt(), sel_temp_unit);

    hum_min = ui->lineEdit_hum_min->text().toInt();
    hum_max = ui->lineEdit_hum_max->text().toInt();

    emit limits_changed_signal();

    //this->setWindowTitle(QString::number(temp_min) + " " + QString::number(temp_max));

    this->destroy();
}

void sensor_limits::lang_changed_slot()
{
    switch (lang)
    {
        case MainWindow::RU_LANG:
            this->setWindowTitle("Пределы срабатывания");

            ui->groupBox->setTitle("Температура");
            ui->label->setText("Нижний предел");
            ui->label_2->setText("Верхний предел");

            ui->groupBox_2->setTitle("Влажность");
            ui->label_3->setText("Нижний предел");
            ui->label_4->setText("Верхний предел");

            ui->pushButton_ok->setText("Ок");
            break;
        case MainWindow::EN_LANG:
            this->setWindowTitle("Sensor limits");

            ui->groupBox->setTitle("Temperature");
            ui->label->setText("Min value");
            ui->label_2->setText("Max value");

            ui->groupBox_2->setTitle("Humidity");
            ui->label_3->setText("Min value");
            ui->label_4->setText("Max value");

            ui->pushButton_ok->setText("Ok");
            break;
    }
}

void sensor_limits::showEvent(QShowEvent *event)
{
    ui->lineEdit_temp_min->setText(QString::number(from_unit(temp_min, temp_unit)));
    ui->lineEdit_temp_max->setText(QString::number(from_unit(temp_max, temp_unit)));

    ui->lineEdit_hum_min->setText(QString::number(hum_min));
    ui->lineEdit_hum_max->setText(QString::number(hum_max));

    switch (temp_unit)
    {
        case MainWindow::C_TEMP_UNIT:
            ui->comboBox->setCurrentIndex(0);
            ui->comboBox_2->setCurrentIndex(0);
            break;
        case MainWindow::F_TEMP_UNIT:
            ui->comboBox->setCurrentIndex(1);
            ui->comboBox_2->setCurrentIndex(1);
            break;
        case MainWindow::K_TEMP_UNIT:
            ui->comboBox->setCurrentIndex(2);
            ui->comboBox_2->setCurrentIndex(2);
            break;
    }
}
