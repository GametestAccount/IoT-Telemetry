#include <QApplication>
#include <QSplashScreen>
#include <QFile>
#include <QJsonDocument>
#include <QJsonObject>
#include "mainwindow.h"

int theme = MainWindow::THEME_BLUISH;
int temp_unit = MainWindow::C_TEMP_UNIT;
int lang = MainWindow::RU_LANG;

QString address = "127.0.0.1";
uint16_t port = 40001;

int temp_min = 10;
int temp_max = 35;

int hum_min = 10;
int hum_max = 70;

static bool loadSettings()
{
    QFile file("settings.json");

    if (file.open(QIODevice::ReadOnly | QIODevice::Text))
    {
        QString data = file.readAll();

        file.close();

        if (data.length() > 0)
        {
            QJsonDocument document = QJsonDocument::fromJson(data.toUtf8());

            QJsonObject settings = document.object();

            theme = settings.value("theme").toInt();
            temp_unit = settings.value("temp_unit").toInt();
            lang = settings.value("lang").toInt();

            address = settings.value("connection").toObject().value("address").toString();
            port = settings.value("connection").toObject().value("port").toInt();

            temp_min = settings.value("temp").toObject().value("min").toInt();
            temp_max = settings.value("temp").toObject().value("max").toInt();

            hum_min = settings.value("hum").toObject().value("min").toInt();
            hum_max = settings.value("hum").toObject().value("max").toInt();

            return true;
        }
    }

    return false;
}

int main(int argc, char *argv[])
{
    QApplication a(argc, argv);

    loadSettings();

    QString themeName = "Dark";

    switch (theme)
    {
        case MainWindow::THEME_DARK:
            themeName = "Dark";
            break;
        case MainWindow::THEME_BLUISH:
            themeName = "Bluish";
            break;
        case MainWindow::THEME_CLOUDY:
            themeName = "Cloudy";
            break;
        case MainWindow::THEME_BURGUNDY:
            themeName = "Burgundy";
            break;
        case MainWindow::THEME_SUNRISE:
            themeName = "Sunrise";
            break;
        case MainWindow::THEME_ROSY:
            themeName = "Rosy";
            break;
    }

    QPixmap pix(":/resources/themes/" + themeName + "/splash_screen");
    QSplashScreen screen(pix);
    screen.show();
    a.processEvents();
    MainWindow w;
    w.show();
    screen.finish(&w);

    return a.exec();
}
