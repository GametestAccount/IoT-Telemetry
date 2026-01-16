#ifndef SENSOR_LIMITS_H
#define SENSOR_LIMITS_H

#include <QDialog>

namespace Ui {
class sensor_limits;
}

class sensor_limits : public QDialog
{
    Q_OBJECT

public:
    explicit sensor_limits(QWidget *parent = nullptr);
    ~sensor_limits();

private slots:
    void on_pushButton_ok_clicked();

    void showEvent(QShowEvent *event);

private:
    Ui::sensor_limits *ui;

signals:
    void limits_changed_signal();

public slots:
    void lang_changed_slot();
};

#endif // SENSOR_LIMITS_H
