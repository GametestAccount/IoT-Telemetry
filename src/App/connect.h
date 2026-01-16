#ifndef CONNECT_H
#define CONNECT_H

#include <QDialog>

namespace Ui {
    class connect_dialog;
}

class connect_dialog : public QDialog
{
    Q_OBJECT

public:
    explicit connect_dialog(QWidget *parent = nullptr);
    ~connect_dialog();

private slots:
    void on_pushButton_ok_clicked();

private:
    Ui::connect_dialog *ui;

signals:
    void address_changed_signal();
};

#endif // CONNECT_H
