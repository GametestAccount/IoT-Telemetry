#include "app.h"
#include "ui_app.h"
#include "mainwindow.h"

extern const int lang;

app::app(QWidget *parent) : QDialog(parent), ui(new Ui::app)
{
    ui->setupUi(this);

    this->setMaximumSize(this->width(), this->height());
    this->setMinimumSize(this->width(), this->height());
}

app::~app()
{
    delete ui;
}

void app::showEvent(QShowEvent *event)
{
    lang_changed_slot();
}

void app::on_pushButton_clicked()
{
    this->destroy();
}

void app::lang_changed_slot()
{
    switch (lang)
    {
        case MainWindow::RU_LANG:
            this->setWindowTitle("О программе");

            ui->label->setText("О программе");
            ui->label_4->setText("Версия: 1.2.0");
            ui->label_5->setText("Дата выхода: 01.03.2025");
            ui->label_6->setText("Тенологии: C++, Qt 5.0.2");

            ui->pushButton->setText("Закрыть");
            break;
        case MainWindow::EN_LANG:
            this->setWindowTitle("About app");

            ui->label->setText("About app");
            ui->label_4->setText("Version: 1.2.0");
            ui->label_5->setText("Released: 01.03.2025");
            ui->label_6->setText("Technologies: C++, Qt 5.0.2");

            ui->pushButton->setText("Close");
            break;
    }
}
