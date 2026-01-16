#ifndef APP_H
#define APP_H

#include <QDialog>

namespace Ui {
    class app;
}

class app : public QDialog
{
    Q_OBJECT

public:
    explicit app(QWidget *parent = nullptr);
    ~app();

private slots:
    void showEvent(QShowEvent *event);

    void on_pushButton_clicked();

private:
    Ui::app *ui;

public slots:
    void lang_changed_slot();
};

#endif // APP_H
