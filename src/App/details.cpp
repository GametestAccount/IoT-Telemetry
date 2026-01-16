#include "details.h"
#include "ui_details.h"
#include "mainwindow.h"
#include <cmath>

extern int lang;
extern int temp_unit;

extern int temp_min;
extern int temp_max;

extern int hum_min;
extern int hum_max;

details::details(QWidget *parent) : QDialog(parent), ui(new Ui::details)
{
    ui->setupUi(this);

    this->setMaximumSize(this->width(), this->height());
    this->setMinimumSize(this->width(), this->height());

    timer_temp = new QTimer();
    connect(timer_temp, SIGNAL(timeout()), this, SLOT(timer_temp_alarm()));

    timer_hum = new QTimer();
    connect(timer_hum, SIGNAL(timeout()), this, SLOT(timer_hum_alarm()));

    lang_changed_slot();
    temp_unit_changed_slot();

    ui->pushButton_temp->setHidden(true);
    ui->pushButton_hum->setHidden(true);

    ui->temp_icon->setPixmap(QPixmap(""));
    ui->hum_icon->setPixmap(QPixmap(""));

    mo.setFileName(":/resources/icons8-ветряной-генератор.gif");
    ui->label_3->setMovie(&mo);
    //mo.start();
    ui->label_3->setHidden(true);
    ui->label_4->setHidden(false);

    ui->label_5->setMovie(&mo);
    ui->label_5->setHidden(true);
    ui->label_6->setHidden(false);

    mo.start();

}

details::~details()
{
    delete timer_temp;
    delete timer_hum;

    delete ui;
}

void details::values_changed_slot(int temp, int hum)
{
    ui->label_temp->setText(QString::number(temp) + " °C");
    ui->label_hum->setText(QString::number(hum) + " %");

    if ((temp < temp_min) || (temp > temp_max))
    {
        timer_temp->start(300);

        if (temp < temp_min)
        {
            ui->temp_icon->setPixmap(QPixmap(":/resources/details/icons8-снежинка-64.png"));
            ui->pushButton_temp->setHidden(false);
            ui->pushButton_temp->setText("Включить обогреватель");
        }

        if (temp > temp_max)
        {
            ui->temp_icon->setPixmap(QPixmap(":/resources/details/icons8-солнце-64.png"));
            ui->pushButton_temp->setHidden(false);
            ui->pushButton_temp->setText("Включить вентилятор");
        }
    }
    else
    {
        timer_temp->stop();

        ui->frame->setStyleSheet("#frame {border: none;}");
        ui->temp_icon->setPixmap(QPixmap(""));
        ui->pushButton_temp->setHidden(true);

        ui->label_5->setHidden(true);
        ui->label_6->setHidden(false);
    }

    if ((hum < hum_min) || (hum > hum_max))
    {
        timer_hum->start(350);

        if (hum < hum_min)
        {
            ui->hum_icon->setPixmap(QPixmap(":/resources/details/icons8-ветер-64.png"));
            ui->pushButton_hum->setHidden(false);
            ui->pushButton_hum->setText("Включить увлажнитель");
        }

        if (hum > hum_max)
        {
            ui->hum_icon->setPixmap(QPixmap(":/resources/details/icons8-гигрометр-64.png"));
            ui->pushButton_hum->setHidden(false);
            ui->pushButton_hum->setText("Включить горячитель");
        }
    }
    else
    {
        timer_hum->stop();
        ui->frame_2->setStyleSheet("#frame_2 {border: none;}");
        ui->hum_icon->setPixmap(QPixmap(""));
        ui->pushButton_hum->setHidden(true);

        ui->label_3->setHidden(true);
        ui->label_4->setHidden(false);


    }

    this->temp = temp;
    this->hum = hum;
}

void details::timer_temp_alarm()
{
    static bool state = false;

    if (state)
    {
        if (temp < temp_min)
        {
            ui->frame->setStyleSheet("#frame {border: 2px solid navy; background-color: rgba(0, 0, 128, 0.2);}");
        }

        if (temp > temp_max)
        {
            ui->frame->setStyleSheet("#frame {border: 2px solid maroon; background-color: rgba(128, 0, 0, 0.2);}}");
        }
    }
    else
    {
        ui->frame->setStyleSheet("#frame {border: none; background-color: none;}");
    }

    state = !state;
}

void details::timer_hum_alarm()
{
    static bool state = false;

    if (state)
    {
        if (hum < hum_min)
        {
            ui->frame_2->setStyleSheet("#frame_2 {border: 2px solid darkgoldenrod; background-color: rgba(184, 134, 11, 0.2);}");
        }

        if (hum > hum_max)
        {
            ui->frame_2->setStyleSheet("#frame_2 {border: 2px solid darkcyan; background-color: rgba(0, 139, 139, 0.2);}");
        }
    }
    else
    {
        ui->frame_2->setStyleSheet("#frame {border: none; background-color: none;}");
    }

    state = !state;
}

void details::lang_changed_slot()
{
    switch (lang)
    {
        case MainWindow::RU_LANG:
            this->setWindowTitle("Детализация");
            ui->label->setText("Датчик температуры");
            ui->label_2->setText("Датчик влажности");
            break;
        case MainWindow::EN_LANG:
            this->setWindowTitle("Details");
            ui->label->setText("Temperature sensor");
            ui->label_2->setText("Humidity sensor");
            break;
    }
}

void details::temp_unit_changed_slot()
{
    switch (temp_unit)
    {
        case MainWindow::C_TEMP_UNIT:
            ui->label_temp->setText(QString::number(temp) + " °C");
            break;
        case MainWindow::F_TEMP_UNIT:
            ui->label_temp->setText(QString::number((int)std::round(temp * (9.0 / 5.0) + 32)) + " °F");
            break;
        case MainWindow::K_TEMP_UNIT:
            ui->label_temp->setText(QString::number((int)std::round(temp + 273.15)) + " K");
            break;
    }
}

void details::limits_changed_slot()
{
    return;
}

void details::on_pushButton_temp_clicked()
{
    ui->label_5->setHidden(false);
    ui->label_6->setHidden(true);

    if (ui->pushButton_temp->text() == "Включить вентилятор")
    {
        emit ven_changed_signal();
    }
    else
    {
        emit hot_changed_signal();
    }
}


void details::on_pushButton_hum_clicked()
{
    ui->label_3->setHidden(false);
    ui->label_4->setHidden(true);

    if (ui->pushButton_hum->text() == "Включить увлажнитель")
    {
        emit hum_changed_signal();
    }
    else
    {
        emit steam_changed_signal();
    }
}

