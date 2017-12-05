#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include <QNetworkAccessManager>
#include <QNetworkRequest>
#include <QNetworkReply>

namespace Ui {
class MainWindow;
}

class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    explicit MainWindow(QWidget *parent = 0);
    ~MainWindow();
public slots:
    void onButtonRefreshCompany();
    void onButtonRefreshVehicle();
    void onButtonRefreshLaunchpad();
    void onButtonRefreshLaunches();
    void onButtonHelp();

private:
    void refreshCompany(QJsonDocument doc);
    void refreshVehicle(QJsonDocument doc);
    void refreshLaunchpad(QJsonDocument doc);
    void refreshLaunches(QJsonDocument doc);
    Ui::MainWindow *ui;
    QNetworkAccessManager manager;
    QUrl url;
    QString boolToString(bool value);
    void parseVehicle(QJsonObject vehicle);
    void parseVehicleStage(QJsonObject stage);
    void parseEngines(QJsonObject engine, QString tabulator);
private slots:
    void downloadFinished(QNetworkReply *reply);

};

#endif // MAINWINDOW_H
