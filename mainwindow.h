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
    void onButtonRefreshRocket();
    void onButtonRefreshCapsule();
    void onButtonRefreshLaunchpad();
    void onButtonRefreshLaunches();
    void onButtonHelp();

private:
    int tabulatorWidth;
    void refreshCompany(QJsonDocument doc);
    void refreshRocket(QJsonDocument doc);
    void refreshCapsule(QJsonDocument doc);
    void refreshLaunchpad(QJsonDocument doc);
    void refreshLaunches(QJsonDocument doc);
    Ui::MainWindow *ui;
    QNetworkAccessManager manager;
    QUrl url;
    QString boolToString(bool value);
    void parseRocket(QJsonObject rocket);
    void parseRocketStage(QJsonObject stage);
    void parseEngines(QJsonObject engine, QString tabulator);
    void parseCapsule(QJsonObject capsule);
    void parseLaunchpad(QJsonObject launchpad);
    void parseLaunch(QJsonObject launch);
private slots:
    void downloadFinished(QNetworkReply *reply);

};

#endif // MAINWINDOW_H
