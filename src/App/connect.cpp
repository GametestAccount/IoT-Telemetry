#include "connect.h"
#include "ui_connect.h"

extern QString address;
extern uint16_t port;

connect_dialog::connect_dialog(QWidget *parent) : QDialog(parent), ui(new Ui::connect_dialog)
{
    ui->setupUi(this);

    this->setMaximumSize(this->width(), this->height());
    this->setMinimumSize(this->width(), this->height());

    ui->lineEdit_address->setText(address);
    ui->lineEdit_port->setText(QString::number(port));
}

connect_dialog::~connect_dialog()
{
    delete ui;
}

void connect_dialog::on_pushButton_ok_clicked()
{
    QString newAddress = ui->lineEdit_address->text();
    uint16_t newPort = ui->lineEdit_port->text().toUShort();

    if ((newAddress != address) || (newPort != port))
    {
        address = newAddress;
        port = newPort;

        emit address_changed_signal();
    }

    this->destroy();
}
