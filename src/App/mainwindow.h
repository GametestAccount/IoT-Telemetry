#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QtWebSockets/QWebSocket>
#include <QMainWindow>
#include <QTimer>
#include <QActionGroup>
#include "connect.h"
#include "app.h"
#include "details.h"
#include "sensor_limits.h"
#include <QMovie>

/*
    Сетевая часть, , добавить сохранение этих параметров при закрытии окна через крестик,
    окна должны быть активны, даже если их повторно открывают

*/

QT_BEGIN_NAMESPACE
namespace Ui {
    class MainWindow;
}
QT_END_NAMESPACE

class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    enum {THEME_DARK, THEME_BLUISH, THEME_CLOUDY, THEME_BURGUNDY, THEME_SUNRISE, THEME_ROSY};
    enum {C_TEMP_UNIT, F_TEMP_UNIT, K_TEMP_UNIT};
    enum {RU_LANG, EN_LANG};

    MainWindow(QWidget *parent = nullptr);
    ~MainWindow();

private:
    Ui::MainWindow *ui;

    QActionGroup *menu_theme_group;
    QActionGroup *menu_units_group;
    QActionGroup *menu_lang_group;

    QString serverAddress;
    QWebSocket ws;

    int gas = 0;
    int photoresistor = 0;
    bool motion = false;
    int temp = 0;
    int humidity = 0;

    void updateTheme();

    void updateTempUnit();

    void updateMenuLang();

    void setValues(bool reset = true);

    void setLabelStyle(QString css);

    //clear...

    QTimer *watchdogTimer;

    bool lastPacketReceived = false;

    app *app_window;
    connect_dialog *connect_window;
    details *details_window;
    sensor_limits *limits_window;

    QMovie mo;

private slots:
    void closeEvent(QCloseEvent *event);

    void on_action_theme_dark_triggered();
    void on_action_theme_bluish_triggered();
    void on_action_theme_cloudy_triggered();
    void on_action_theme_burgundy_triggered();
    void on_action_theme_sunrise_triggered();
    void on_action_theme_rosy_triggered();

    void on_action_units_c_triggered();
    void on_action_units_f_triggered();
    void on_action_units_k_triggered();

    void on_action_lang_ru_triggered();
    void on_action_lang_en_triggered();

    void on_action_connect_triggered();

    void on_action_exit_triggered();

    void on_action_info_app_triggered();

    void wsOnConnected();
    void wsOnDisconnected();
    void wsOnTextMessageReceived(QString message);

    void timeout();

    void on_details_pushButton_clicked();
    void on_details_pushButton_2_clicked();

    void on_action_limits_triggered();

public slots:
    void address_changed_slot();

    void limits_changed_slot();

    void ven_changed_slot();
    void hot_changed_slot();
    void hum_changed_slot();
    void steam_changed_slot();

signals:
    void values_changed_signal(int temp, int hum);

    void lang_changed_signal();

    void temp_unit_changed_signal();

    void limits_changed_signal();
};

#endif // MAINWINDOW_H
