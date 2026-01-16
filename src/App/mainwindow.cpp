#include <QActionGroup>
#include <QJsonDocument>
#include <QJsonObject>
#include <QFile>
#include <cmath>
#include "mainwindow.h"
#include "ui_mainwindow.h"

extern int theme;
extern int temp_unit;
extern int lang;

extern QString address;
extern uint16_t port;

extern int temp_min;
extern int temp_max;

extern int hum_min;
extern int hum_max;

static bool saveSettings()
{
    QFile file("settings.json");

    if (file.open(QIODevice::WriteOnly | QIODevice::Text))
    {
        QJsonObject settings = {
            {"theme", theme},
            {"temp_unit", temp_unit},
            {"lang", lang},
            {"connection", QJsonObject({{"address", address}, {"port", port}})},
            {"temp", QJsonObject({{"min", temp_min}, {"max", temp_max}})},
            {"hum", QJsonObject({{"min", hum_min}, {"max", hum_max}})}
        };

        QJsonDocument *document = new QJsonDocument(settings);

        if (document)
        {
            file.write(document->toJson(QJsonDocument::Compact));

            delete document;

            return true;
        }

        file.close();
    }

    return false;
}

MainWindow::MainWindow(QWidget *parent) : QMainWindow(parent), ui(new Ui::MainWindow)
{
    ui->setupUi(this);

    this->setMaximumSize(this->width(), this->height());
    this->setMinimumSize(this->width(), this->height());

    menu_theme_group = new QActionGroup(this);

    for (auto action : ui->menu_theme->actions())
    {
        menu_theme_group->addAction(action);
    }

    menu_units_group = new QActionGroup(this);

    for (auto action : ui->menu_units->actions())
    {
        menu_units_group->addAction(action);
    }

    menu_lang_group = new QActionGroup(this);

    for (auto action : ui->menu_lang->actions())
    {
        menu_lang_group->addAction(action);
    }

    address_changed_slot();

    watchdogTimer = new QTimer();
    connect(watchdogTimer, &QTimer::timeout, this, &MainWindow::timeout);

    connect(&ws, &QWebSocket::connected, this, &MainWindow::wsOnConnected);
    connect(&ws, &QWebSocket::disconnected, this, &MainWindow::wsOnDisconnected);
    connect(&ws, &QWebSocket::textMessageReceived, this, &MainWindow::wsOnTextMessageReceived);

    updateTheme();
    updateMenuLang();
    setValues();

    app_window = new app();
    app_window->setWindowFlag(Qt::WindowContextHelpButtonHint, false);

    connect_window = new connect_dialog();
    connect_window->setWindowFlag(Qt::WindowContextHelpButtonHint, false);

    connect(connect_window, &connect_dialog::address_changed_signal, this, &MainWindow::address_changed_slot);

    details_window = new details();
    details_window->setWindowFlag(Qt::WindowContextHelpButtonHint, false);

    connect(this, &MainWindow::values_changed_signal, details_window, &details::values_changed_slot);
    connect(this, &MainWindow::lang_changed_signal, details_window, &details::lang_changed_slot);
    connect(this, &MainWindow::lang_changed_signal, app_window, &app::lang_changed_slot);

    connect(this, &MainWindow::temp_unit_changed_signal, details_window, &details::temp_unit_changed_slot);
    connect(this, &MainWindow::limits_changed_signal, details_window, &details::limits_changed_slot);

    limits_window = new sensor_limits();
    limits_window->setWindowFlag(Qt::WindowContextHelpButtonHint, false);
    connect(limits_window, &sensor_limits::limits_changed_signal, this, &MainWindow::limits_changed_slot);
    connect(this, &MainWindow::lang_changed_signal, limits_window, &sensor_limits::lang_changed_slot);

    setWindowTitle(windowTitle() + " - No connection");

    connect(details_window, &details::ven_changed_signal, this, &MainWindow::ven_changed_slot);
    connect(details_window, &details::hot_changed_signal, this, &MainWindow::hot_changed_slot);
    connect(details_window, &details::hum_changed_signal, this, &MainWindow::hum_changed_slot);
    connect(details_window, &details::steam_changed_signal, this, &MainWindow::steam_changed_slot);
}

MainWindow::~MainWindow()
{
    delete menu_theme_group;
    delete menu_units_group;
    delete menu_lang_group;

    delete app_window;
    delete connect_window;
    delete limits_window;

    delete watchdogTimer;

    delete ui;
}

void MainWindow::closeEvent(QCloseEvent *event)
{
    saveSettings();

    QCoreApplication::quit();
}

void MainWindow::on_action_theme_dark_triggered()
{
    if (theme == THEME_DARK)
        return;

    theme = THEME_DARK;

    updateTheme();
}

void MainWindow::on_action_theme_bluish_triggered()
{
    if (theme == THEME_BLUISH)
        return;

    theme = THEME_BLUISH;

    updateTheme();
}

void MainWindow::on_action_theme_cloudy_triggered()
{
    if (theme == THEME_CLOUDY)
        return;

    theme = THEME_CLOUDY;

    updateTheme();
}

void MainWindow::on_action_theme_burgundy_triggered()
{
    if (theme == THEME_BURGUNDY)
        return;

    theme = THEME_BURGUNDY;

    updateTheme();
}

void MainWindow::on_action_theme_sunrise_triggered()
{
    if (theme == THEME_SUNRISE)
        return;

    theme = THEME_SUNRISE;

    updateTheme();
}

void MainWindow::on_action_theme_rosy_triggered()
{
    if (theme == THEME_ROSY)
        return;

    theme = THEME_ROSY;

    updateTheme();
}

void MainWindow::on_action_units_c_triggered()
{
    if (temp_unit == C_TEMP_UNIT)
        return;

    temp_unit = C_TEMP_UNIT;

    updateTempUnit();
}

void MainWindow::on_action_units_f_triggered()
{
    if (temp_unit == F_TEMP_UNIT)
        return;

    temp_unit = F_TEMP_UNIT;

    updateTempUnit();
}

void MainWindow::on_action_units_k_triggered()
{
    if (temp_unit == K_TEMP_UNIT)
        return;

    temp_unit = K_TEMP_UNIT;

    updateTempUnit();
}

void MainWindow::on_action_lang_ru_triggered()
{
    lang = RU_LANG;

    updateTheme();
    updateMenuLang();
    setValues(false);

    emit lang_changed_signal();
}

void MainWindow::on_action_lang_en_triggered()
{
    lang = EN_LANG;

    updateTheme();
    updateMenuLang();
    setValues(false);

    emit lang_changed_signal();
}

void MainWindow::on_action_connect_triggered()
{
    connect_window->show();
}

void MainWindow::on_action_exit_triggered()
{
    this->close();
}

void MainWindow::on_action_info_app_triggered()
{
    app_window->show();
}

void MainWindow::updateTheme()
{
    ui->menubar->setStyleSheet("background-color: none;");

    QString themeName = "Dark";
    QString labelColor = "rgb(227, 234, 253)";
    QString backgroundColor = "rgb(7, 26, 69)";

    switch (theme)
    {
        case THEME_DARK:
            themeName = "Dark";
            labelColor = "rgb(227, 234, 253)";
            backgroundColor = "rgb(7, 26, 69)";
            ui->action_theme_dark->setChecked(true);
            break;
        case THEME_BLUISH:
            themeName = "Bluish";
            labelColor = "rgb(45, 60, 63)";
            backgroundColor = "rgb(223, 239, 240)";
            ui->action_theme_bluish->setChecked(true);
            break;
        case THEME_CLOUDY:
            themeName = "Cloudy";
            labelColor = "rgb(21, 38, 54)";
            backgroundColor = "rgb(198, 212, 228)";
            ui->action_theme_cloudy->setChecked(true);
            break;
        case THEME_BURGUNDY:
            themeName = "Burgundy";
            labelColor = "rgb(57, 6, 15)";
            backgroundColor = "rgb(229, 193, 203)";
            ui->action_theme_burgundy->setChecked(true);
            break;
        case THEME_SUNRISE:
            themeName = "Sunrise";
            labelColor = "rgb(42, 35, 17)";
            backgroundColor = "rgb(244, 229, 195)";
            ui->action_theme_sunrise->setChecked(true);
            break;
        case THEME_ROSY:
            themeName = "Rosy";
            labelColor = "rgb(77, 28, 21)";
            backgroundColor = "rgb(248, 235, 233)";
            ui->action_theme_rosy->setChecked(true);
            break;
    }

    QString langPrefix = "ru_RU";

    switch (lang)
    {
        case RU_LANG:
            langPrefix = "ru_RU";
            break;
        case EN_LANG:
            langPrefix = "en_US";
            break;
    }

    ui->background->setPixmap(QPixmap(":/resources/themes/" + themeName + "/ui_" + langPrefix));

    setLabelStyle("color: " + labelColor);

    this->setStyleSheet("background-color: " + backgroundColor);
}

void MainWindow::updateTempUnit()
{
    switch (temp_unit)
    {
        case C_TEMP_UNIT:
            ui->label_temp->setText(QString::number(temp) + " °C");
            ui->action_units_c->setChecked(true);
            break;
        case F_TEMP_UNIT:
            ui->label_temp->setText(QString::number((int)std::round(temp * (9.0 / 5.0) + 32)) + " °F");
            ui->action_units_f->setChecked(true);
            break;
        case K_TEMP_UNIT:
            ui->label_temp->setText(QString::number((int)std::round(temp + 273.15)) + " K");
            ui->action_units_k->setChecked(true);
            break;
    }

    emit temp_unit_changed_signal();
}

void MainWindow::updateMenuLang()
{
    switch (lang)
    {
        case RU_LANG:
            ui->settings->setTitle("Настройки");

            ui->menu_theme->setTitle("Тема...");
            ui->action_theme_dark->setText("Тема Dark");
            ui->action_theme_bluish->setText("Тема Bluish");
            ui->action_theme_cloudy->setText("Тема Cloudy");
            ui->action_theme_burgundy->setText("Тема Burgundy");
            ui->action_theme_sunrise->setText("Тема Sunrise");
            ui->action_theme_rosy->setText("Тема Rosy");

            ui->menu_units->setTitle("Единицы измерения...");
            ui->action_units_c->setText("Градус Цельсия - °C");
            ui->action_units_f->setText("Градус Фаренгейта - °F");
            ui->action_units_k->setText("Кельвины - К");

            ui->menu_lang->setTitle("Язык...");
            ui->action_lang_ru->setText("Русский язык");
            ui->action_lang_en->setText("Анлийский язык");

            ui->action_connect->setText("Подключение...");

            ui->action_exit->setText("Выход");

            ui->info->setTitle("Справка");
            ui->action_info_app->setText("О приложении...");
            ui->action_limits->setText("Пределы срабатывания...");

            ui->action_lang_ru->setChecked(true);

            break;
        case EN_LANG:
            ui->settings->setTitle("Settings");

            ui->menu_theme->setTitle("Theme...");
            ui->action_theme_dark->setText("Theme Dark");
            ui->action_theme_bluish->setText("Theme Bluish");
            ui->action_theme_cloudy->setText("Theme Cloudy");
            ui->action_theme_burgundy->setText("Theme Burgundy");
            ui->action_theme_sunrise->setText("Theme Sunrise");
            ui->action_theme_rosy->setText("Theme Rosy");

            ui->menu_units->setTitle("Units...");
            ui->action_units_c->setText("Degree Celsius - °C");
            ui->action_units_f->setText("Fahrenheit - °F");
            ui->action_units_k->setText("Kelvins - К");

            ui->menu_lang->setTitle("Language...");
            ui->action_lang_ru->setText("Russian language");
            ui->action_lang_en->setText("English language");

            ui->action_connect->setText("Connection...");

            ui->action_exit->setText("Exit");

            ui->info->setTitle("Info");
            ui->action_info_app->setText("About app...");

            ui->action_limits->setText("Sensor limits...");

            ui->action_lang_en->setChecked(true);

            break;
    }
}

void MainWindow::setValues(bool reset)
{
    if (reset)
    {
        gas = 0;
        photoresistor = 0;
        motion = false;
        temp = 0;
        humidity = 0;
    }

    ui->label_gas->setText(QString::number(gas) + " %");
    ui->label_photoresistor->setText(QString::number(photoresistor) + " %");
    ui->label_humidity->setText(QString::number(humidity) + " %");

    switch (lang)
    {
        case RU_LANG:
            ui->label_motion->setText(motion ? "Да" : "Нет");
            break;
        case EN_LANG:
            ui->label_motion->setText(motion ? "Yes" : "No");
            break;
    }

    emit values_changed_signal(temp, humidity);

    updateTempUnit();
}

void MainWindow::setLabelStyle(QString css)
{
    ui->label_gas->setStyleSheet(css);
    ui->label_photoresistor->setStyleSheet(css);
    ui->label_motion->setStyleSheet(css);
    ui->label_temp->setStyleSheet(css);
    ui->label_humidity->setStyleSheet(css);
}

void MainWindow::wsOnConnected()
{
    watchdogTimer->start(3000);
    //setWindowTitle("Connected...");

    setWindowTitle("Multistation");
}

void MainWindow::wsOnDisconnected()
{
    setWindowTitle("Multistation - No connection");

    watchdogTimer->stop();

    setValues();

    //setWindowTitle("Disconnect...");

    ws.open(serverAddress);
}

void MainWindow::wsOnTextMessageReceived(QString message)
{
    auto doc = QJsonDocument::fromJson(message.toUtf8());

    if (doc.isNull() || !doc.isObject())
    {
        return;
    }

    auto data = doc.object();

    if (data.isEmpty())
    {
        return;
    }

    gas = data.value("gas").toInt();
    photoresistor = data.value("photoresistor").toInt();
    motion = data.value("motion").toBool();
    temp = data.value("temp").toInt();
    humidity = data.value("humidity").toInt();

    setValues(false);

    lastPacketReceived = true;
}

void MainWindow::timeout()
{
    if (!lastPacketReceived)
    {
        setValues();
        ws.close();
    }
    else
    {
        lastPacketReceived = false;
    }
}

void MainWindow::on_details_pushButton_clicked()
{
    details_window->show();
}

void MainWindow::on_details_pushButton_2_clicked()
{
    details_window->show();
}

void MainWindow::address_changed_slot()
{
    serverAddress = "ws://" + address + ":" + QString::number(port);

    ws.open(serverAddress);
}

void MainWindow::on_action_limits_triggered()
{
    limits_window->show();
}

void MainWindow::limits_changed_slot()
{
    emit limits_changed_signal();
}

void MainWindow::ven_changed_slot()
{
    ws.sendTextMessage("{\"click\": 0}");
}

void MainWindow::hot_changed_slot()
{
    ws.sendTextMessage("{\"click\": 1}");
}

void MainWindow::hum_changed_slot()
{
    ws.sendTextMessage("{\"click\": 2}");
}

void MainWindow::steam_changed_slot()
{
    ws.sendTextMessage("{\"click\": 3}");
}
