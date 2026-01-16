#ifndef DETAILS_H
#define DETAILS_H

#include <QDialog>
#include <QTimer>
#include <QMovie>

namespace Ui {
    class details;
}

class details : public QDialog
{
    Q_OBJECT

public:
    explicit details(QWidget *parent = nullptr);
    ~details();

private:
    Ui::details *ui;
    QTimer *timer_temp;
    QTimer *timer_hum;
    int temp = 0;
    int hum = 0;

    QMovie mo;

public slots:
    void values_changed_slot(int temp, int hum);

    void timer_temp_alarm();

    void timer_hum_alarm();

    void lang_changed_slot();

    void temp_unit_changed_slot();

    void limits_changed_slot();
private slots:
    void on_pushButton_temp_clicked();
    void on_pushButton_hum_clicked();

signals:
    void ven_changed_signal();
    void hot_changed_signal();
    void hum_changed_signal();
    void steam_changed_signal();
};

#endif // DETAILS_H
