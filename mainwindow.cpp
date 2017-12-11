#include "mainwindow.h"
#include "ui_mainwindow.h"
#include <QNetworkAccessManager>
#include <QNetworkRequest>
#include <QNetworkReply>
#include <QFile>
#include <QFileDialog>
#include <QJsonArray>
#include <QJsonDocument>
#include <QJsonObject>
#include <QMessageBox>
#include <QTimer>

MainWindow::MainWindow(QWidget *parent) :
    QMainWindow(parent),
    ui(new Ui::MainWindow)
{
    ui->setupUi(this);

    tabulatorWidth = 20;

    ui->tabWidget->setCurrentIndex(0);
    ui->radioRocketID->click();
    ui->radioCapsuleID->click();
    ui->radioLaunchpadID->click();
    ui->radioLaunchesFlightNo->click();
    ui->textCompany->setOpenExternalLinks(true);
    ui->textLaunches->setOpenExternalLinks(true);
    ui->textLaunchpad->setOpenExternalLinks(true);
    ui->textRocket->setOpenExternalLinks(true);
    ui->textCapsule->setOpenExternalLinks(true);
    ui->textCompany->setTabStopWidth(tabulatorWidth);
    ui->textRocket->setTabStopWidth(tabulatorWidth);
    ui->textCapsule->setTabStopWidth(tabulatorWidth);
    ui->textLaunchpad->setTabStopWidth(tabulatorWidth);
    ui->textLaunches->setTabStopWidth(tabulatorWidth);

    url = QUrl("https://api.spacexdata.com/v2/");

    connect(&manager, SIGNAL(finished(QNetworkReply*)), this, SLOT(downloadFinished(QNetworkReply*)));
    connect(ui->buttonHelp, SIGNAL(clicked(bool)), this, SLOT(onButtonHelp()));
    connect(ui->buttonRefreshCompany, SIGNAL(clicked(bool)), this, SLOT(onButtonRefreshCompany()));
    connect(ui->buttonRefreshRocket, SIGNAL(clicked(bool)), this, SLOT(onButtonRefreshRocket()));
    connect(ui->buttonRefreshCapsule, SIGNAL(clicked(bool)), this, SLOT(onButtonRefreshCapsule()));
    connect(ui->buttonRefreshLaunchpad, SIGNAL(clicked(bool)), this, SLOT(onButtonRefreshLaunchpad()));
    connect(ui->buttonRefreshLaunches, SIGNAL(clicked(bool)), this, SLOT(onButtonRefreshLaunches()));
}

MainWindow::~MainWindow()
{
    delete ui;
}

void MainWindow::onButtonRefreshCompany()
{
    QNetworkRequest request(QUrl(url.toString() + "info"));
    QNetworkReply *reply = manager.get(request);
}

void MainWindow::onButtonRefreshCapsule()
{
    QString string;
    string.append("capsules");
    if(ui->radioCapsuleID->isChecked() && !ui->lineSearchCapsule->text().isEmpty())
    {
        string.append("/" + ui->lineSearchCapsule->text());
    }
    QNetworkRequest request(QUrl(url.toString() + string));
    QNetworkReply *reply = manager.get(request);
}

void MainWindow::onButtonRefreshRocket()
{
    if(ui->lineSearchRocket->text() == "how not to land")
    {
        ui->textRocket->setHtml("<a href=\"https://www.youtube.com/watch?v=bvim4rsNHkQ\">How not to land an orbital rocket booster</a>");
        return;
    }
    if(ui->lineSearchRocket->text() == "Musk vs Bezos")
    {
        ui->textRocket->setHtml("<a href=\"https://www.youtube.com/watch?v=40uks4vjdvI\">Elon Musk - When Bezos Tried To Compete against SpaceX</a>");
        return;
    }
    if(ui->lineSearchRocket->text() == "Kill SpaceX!")
    {
        ui->textRocket->setHtml("<a href=\"https://www.youtube.com/watch?v=nULPR9MjKNw\">Elon Musk - When \"They\" Tried To Kill SpaceX | MUST WATCH</a>");
        return;
    }
    QString string;
    string.append("rockets");
    if(ui->radioRocketID->isChecked() && !ui->lineSearchRocket->text().isEmpty())
    {
        string.append("/" + ui->lineSearchRocket->text());
    }
    QNetworkRequest request(QUrl(url.toString() + string));
    QNetworkReply *reply = manager.get(request);
}

void MainWindow::onButtonRefreshLaunchpad()
{
    QString string;
    string.append("launchpads");
    if(ui->radioLaunchpadID->isChecked() && !ui->lineSearchLaunchpad->text().isEmpty())
    {
        string.append("/" + ui->lineSearchLaunchpad->text());
    }
    QNetworkRequest request(QUrl(url.toString() + string));
    QNetworkReply *reply = manager.get(request);
}

void MainWindow::onButtonRefreshLaunches()
{
    QString string;
    string.append("launches");
    bool latest = false;
    if(ui->radioLaunchesLatest->isChecked())
    {
        latest = true;
        string.append("/latest");
    }
    if(ui->checkLaunchesUpcoming->isChecked() && !latest)
    {
        string.append("/upcoming");
    }
    if(ui->radioLaunchesFlightNo->isChecked())
    {
        string.append("?flight_number=" + ui->lineSearchLaunches->text());
    }
    if(ui->radioLaunchesYear->isChecked())
    {
        string.append("?launch_year=" + ui->lineSearchLaunches->text());
    }
    QNetworkRequest request(QUrl(url.toString() + string));
    QNetworkReply *reply = manager.get(request);
}

void MainWindow::onButtonHelp()
{
    QString helpString;
    switch(ui->tabWidget->currentIndex())
    {
    case 0:
        helpString += "Refresh: Refreshes info";
        break;
    case 1:
        helpString += "Refresh: If search line is empty, refreshes info, else applies filter and refreshes info.\n";
        helpString += "ID: Filter by ID.\n";
        helpString += "Name: Filter by rocket name.";
        break;
    case 2:
        helpString += "Refresh: If search line is empty, refreshes info, else applies filter and refreshes info.\n";
        helpString += "ID: Filter by ID.\n";
        helpString += "Name: Filter by capsule name.";
        break;
    case 3:
        helpString += "Refresh: If search line is empty, refreshes info, else applies filter and refreshes info.\n";
        helpString += "ID: Filter by ID.\n";
        helpString += "Name: Filter by launchpad name.";
        break;
    case 4:
        helpString += "Refresh: If search line is empty, refreshes info, else applies filter and refreshes info.\n";
        helpString += "Flight number: Filter by flight number.\n";
        helpString += "Launch year: Filter by year of launch.\n";
        helpString += "Latest: Display only latest launches. Has priority over Upcoming.\n";
        helpString += "Upcoming: Display only upcoming launches.";
        break;
    default:
        helpString += "Unknow tabwidget index.";
        break;
    }
    QMessageBox::information(this, "Help", helpString);
}

void MainWindow::downloadFinished(QNetworkReply *reply)
{
    if(!reply->error())
    {
        QJsonDocument json = QJsonDocument::fromJson(reply->readAll());
        if(reply->url().toString().contains("info"))
        {
            refreshCompany(json);
        }
        if(reply->url().toString().contains("rockets"))
        {
            refreshRocket(json);
        }
        if(reply->url().toString().contains("capsules"))
        {
            refreshCapsule(json);
        }
        if(reply->url().toString().contains("launchpads"))
        {
            refreshLaunchpad(json);
        }
        if(reply->url().toString().contains("launches"))
        {
            refreshLaunches(json);
        }
    }
    else
    {
        QMessageBox::warning(this, "ERROR", "Download failed\n" + reply->errorString());
    }
}

void MainWindow::refreshCompany(QJsonDocument doc)
{
    QJsonObject company = doc.object();
    if(company.contains("name") && company["name"].isString())
    {
        ui->textCompany->setText("Name: " + company["name"].toString());
    }
    if(company.contains("founder") && company["founder"].isString())
    {
        ui->textCompany->append("Founder: " + company["founder"].toString());
    }
    if(company.contains("founded") && company["founded"].isDouble())
    {
        ui->textCompany->append("Founded: " + QString::number(company["founded"].toInt()));
    }
    if(company.contains("employees") && company["employees"].isDouble())
    {
        ui->textCompany->append("Employees: " + QString::number(company["employees"].toInt()));
    }
    if(company.contains("rockets") && company["rockets"].isDouble())
    {
        ui->textCompany->append("rockets: " + QString::number(company["rockets"].toInt()));
    }
    if(company.contains("launch_sites") && company["launch_sites"].isDouble())
    {
        ui->textCompany->append("Launch sites: " + QString::number(company["launch_sites"].toInt()));
    }
    if(company.contains("test_sites") && company["test_sites"].isDouble())
    {
        ui->textCompany->append("Test sites: " + QString::number(company["test_sites"].toInt()));
    }
    if(company.contains("ceo") && company["ceo"].isString())
    {
        ui->textCompany->append("CEO: " + company["ceo"].toString());
    }
    if(company.contains("cto") && company["cto"].isString())
    {
        ui->textCompany->append("CTO: " + company["cto"].toString());
    }
    if(company.contains("coo") && company["coo"].isString())
    {
        ui->textCompany->append("COO: " + company["coo"].toString());
    }
    if(company.contains("cto_propulsion") && company["cto_propulsion"].isString())
    {
        ui->textCompany->append("CTO of propulsion: " + company["cto_propulsion"].toString());
    }
    if(company.contains("valuation") && company["valuation"].isDouble())
    {
        ui->textCompany->append("Value: " + QString::number(company["valuation"].toDouble(), 'f', 0));
    }
    if(company.contains("headquarters") && company["headquarters"].isObject())
    {
        QString address;
        address.append("Headquarters: ");
        QJsonObject headquarters = company["headquarters"].toObject();
        if(headquarters.contains("address") && headquarters["address"].isString())
        {
            address.append(headquarters["address"].toString() + " - ");
        }
        if(headquarters.contains("city") && headquarters["city"].isString())
        {
            address.append(headquarters["city"].toString() + " - ");
        }
        if(headquarters.contains("state") && headquarters["state"].isString())
        {
            address.append(headquarters["state"].toString() + " - ");
        }
        address.append("USA");
        ui->textCompany->append(address);
    }
}

void MainWindow::refreshRocket(QJsonDocument doc)
{
    QJsonArray rocketArray;
    QJsonObject rocket;
    bool filterFlag = false;
    bool found = false;
    if(!ui->lineSearchRocket->text().isEmpty() && ui->radioRocketName->isChecked())
    {
        filterFlag = true;
    }
    ui->textRocket->clear();
    if(ui->radioRocketID->isChecked() && !ui->lineSearchRocket->text().isEmpty())
    {
        rocket = doc.object();
        if(rocket.contains("error") && rocket["error"].isString())
        {
            ui->textRocket->setText("ERROR: " + rocket["error"].toString());
            return;
        }
        rocketArray.append(rocket);
    }
    else
    {
        rocketArray = doc.array();
    }
    for(int i = 0; i < rocketArray.count(); ++i)
    {
        rocket = rocketArray.at(i).toObject();
        if(!filterFlag || (rocket.contains("name") && rocket["name"].isString() && rocket["name"].toString() == ui->lineSearchRocket->text()))
        {
            parseRocket(rocket);
            found = true;
            ui->textRocket->append("");
        }
    }
    if(!found)
    {
        ui->textRocket->setText("ERROR: No match found!");
    }
}

void MainWindow::refreshCapsule(QJsonDocument doc)
{
    QJsonArray capsuleArray;
    QJsonObject capsule;
    bool filterFlag = false;
    bool found = false;
    if(!ui->lineSearchCapsule->text().isEmpty() && ui->radioCapsuleName->isChecked())
    {
        filterFlag = true;
    }
    ui->textCapsule->clear();
    if(ui->radioCapsuleID->isChecked() && !ui->lineSearchCapsule->text().isEmpty())
    {
        capsule = doc.object();
        if(capsule.contains("error") && capsule["error"].isString())
        {
            ui->textCapsule->setText("ERROR: " + capsule["error"].toString());
            return;
        }
        capsuleArray.append(capsule);
    }
    else qDebug() << "Pokud dám qDebug mezi tento bod";
    {
        capsuleArray = doc.array();
    }
    for(int i = 0; i < capsuleArray.count(); ++i)
    {
        capsule = capsuleArray.at(i).toObject();
        if(!filterFlag || (capsule.contains("name") && capsule["name"].isString() && capsule["name"].toString() == ui->lineSearchCapsule->text()))
        {
            qDebug() << " a tento bod, tak to z ničeho nic funguje, jinak to padá. WTF!";
            parseCapsule(capsule);
            found = true;
            ui->textCapsule->append("");
        }
    }
    if(!found)
    {
        ui->textCapsule->setText("ERROR: No match found!");
    }
}

void MainWindow::refreshLaunchpad(QJsonDocument doc)
{
    QJsonArray launchpadArray;
    QJsonObject launchpad;
    bool filterFlag = false;
    bool found = false;
    if(!ui->lineSearchLaunchpad->text().isEmpty() && ui->radioLaunchpadName->isChecked())
    {
        filterFlag = true;
    }
    ui->textLaunchpad->clear();
    if(ui->radioLaunchpadID->isChecked() && !ui->lineSearchLaunchpad->text().isEmpty())
    {
        launchpad = doc.object();
        if(launchpad.contains("error") && launchpad["error"].isString())
        {
            ui->textLaunchpad->setText("ERROR: " + launchpad["error"].toString());
            return;
        }
        launchpadArray.append(launchpad);
    }
    else
    {
        launchpadArray = doc.array();
    }
    for(int i = 0; i < launchpadArray.count(); ++i)
    {
        launchpad = launchpadArray.at(i).toObject();
        if(!filterFlag || (launchpad.contains("full_name") && launchpad["full_name"].isString() && launchpad["full_name"].toString().contains(ui->lineSearchLaunchpad->text())))
        {
            parseLaunchpad(launchpad);
            found = true;
            ui->textLaunchpad->append("");
        }
    }
    if(!found)
    {
        ui->textLaunchpad->setText("ERROR: No match found!");
    }
}

void MainWindow::refreshLaunches(QJsonDocument doc)
{
    QJsonArray launchArray;
    QJsonObject launch;
    ui->textLaunches->clear();
    if(ui->radioLaunchesLatest->isChecked())
    {
        parseLaunch(doc.object());
        return;
    }
    launchArray = doc.array();
    for(int i = 0; i < launchArray.count(); ++i)
    {
        launch = launchArray.at(i).toObject();
        parseLaunch(launch);
        ui->textLaunches->append("");
    }
}

QString MainWindow::boolToString(bool value)
{
    if(value)
    {
        return QString("true");
    }
    else
    {
        return QString("false");
    }
}

void MainWindow::parseRocket(QJsonObject rocket)
{
    if(rocket.contains("id") && rocket["id"].isString())
    {
        ui->textRocket->append("ID: " + rocket["id"].toString());
    }
    if(rocket.contains("name") && rocket["name"].isString())
    {
        ui->textRocket->append("Name: " + rocket["name"].toString());
    }
    if(rocket.contains("type") && rocket["type"].isString())
    {
        ui->textRocket->append("Type: " + rocket["type"].toString());
    }
    if(rocket.contains("active") && rocket["active"].isBool())
    {
        ui->textRocket->append("Active: " + boolToString(rocket["active"].toBool()));
    }
    if(rocket.contains("stages") && rocket["stages"].isDouble())
    {
        ui->textRocket->append("Stages: " + QString::number(rocket["stages"].toInt()));
    }
    if(rocket.contains("cost_per_launch") && rocket["cost_per_launch"].isDouble())
    {
        ui->textRocket->append("Cost per launch: " + QString::number(rocket["cost_per_launch"].toInt()));
    }
    if(rocket.contains("success_rate_pct") && rocket["success_rate_pct"].isDouble())
    {
        ui->textRocket->append("Success rate: " + QString::number(rocket["success_rate_pct"].toInt()) + "%");
    }
    if(rocket.contains("first_flight") && rocket["first_flight"].isString())
    {
        ui->textRocket->append("First flight: " + rocket["first_flight"].toString());
    }
    if(rocket.contains("country") && rocket["country"].isString())
    {
        ui->textRocket->append("Country: " + rocket["country"].toString());
    }
    if(rocket.contains("height") && rocket["height"].isObject())
    {
        QJsonObject height = rocket["height"].toObject();
        if(height.contains("meters") && height["meters"].isDouble())
        {
            ui->textRocket->append("Height: " + QString::number(height["meters"].toInt()) + " m");
        }
    }
    if(rocket.contains("diameter") && rocket["diameter"].isObject())
    {
        QJsonObject diameter = rocket["diameter"].toObject();
        if(diameter.contains("meters") && diameter["meters"].isDouble())
        {
            ui->textRocket->append("Diameter: " + QString::number(diameter["meters"].toInt()) + " m");
        }
    }
    if(rocket.contains("mass") && rocket["mass"].isObject())
    {
        QJsonObject mass = rocket["mass"].toObject();
        if(mass.contains("mass") && mass["meters"].isDouble())
        {
            ui->textRocket->append("Mass: " + QString::number(mass["meters"].toInt()) + " m");
        }
    }
    if(rocket.contains("payload_weights") && rocket["payload_weights"].isArray())
    {
        QString string;
        string.append("Payload weights:");
        QJsonArray array = rocket["payload_weights"].toArray();
        for(int j = 0; j < array.count(); ++j)
        {
            string.append("\n");
            QJsonObject obj = array.at(j).toObject();
            if(obj.contains("name") && obj["name"].isString())
            {
                string.append("\t" + obj["name"].toString() + ": ");
            }
            if(obj.contains("kg") && obj["kg"].isDouble())
            {
                string.append(QString::number(obj["kg"].toInt()) + " kg");
            }
        }
        ui->textRocket->append(string);
    }
    if(rocket.contains("first_stage") && rocket["first_stage"].isObject())
    {
        ui->textRocket->append("First stage:");
        parseRocketStage(rocket["first_stage"].toObject());
    }
    if(rocket.contains("second_stage") && rocket["second_stage"].isObject())
    {
        ui->textRocket->append("Second stage:");
        parseRocketStage(rocket["second_stage"].toObject());
    }
    if(rocket.contains("engines") && rocket["engines"].isObject())
    {
        parseEngines(rocket["engines"].toObject(),"");
    }
    if(rocket.contains("landing_legs") && rocket["landing_legs"].isObject())
    {
        ui->textRocket->append("Landing legs:");
        QJsonObject legs = rocket["landing_legs"].toObject();
        if(legs.contains("number") && legs["number"].isDouble())
        {
            ui->textRocket->append("\tNumber: " + QString::number(legs["number"].toInt()));
        }
        if(legs.contains("material") && legs["material"].isString())
        {
            ui->textRocket->append("\tMaterial: " + legs["material"].toString());
        }
    }
}

void MainWindow::parseRocketStage(QJsonObject stage)
{
    if(stage.contains("reusable") && stage["reusable"].isBool())
    {
        ui->textRocket->append("\tReusable: " + boolToString(stage["reusable"].toBool()));
    }
    if(stage.contains("engines") && stage["engines"].isDouble())
    {
        ui->textRocket->append("\tEngines: " + QString::number(stage["engines"].toInt()));
    }
    if(stage.contains("engines") && stage["engines"].isObject())
    {
        parseEngines(stage["engines"].toObject(),"\t");
    }
    if(stage.contains("fuel_amount_tons") && stage["fuel_amount_tons"].isDouble())
    {
        ui->textRocket->append("\tFuel amount: " + QString::number(stage["fuel_amount_tons"].toInt()) + " tons");
    }
    if(stage.contains("burn_time_sec") && stage["burn_time_sec"].isDouble())
    {
        ui->textRocket->append("\tBurn time: " + QString::number(stage["burn_time_sec"].toInt()) + " seconds");
    }
    if(stage.contains("payloads") && stage["payloads"].isObject())
    {
        ui->textRocket->append("\tPayloads:");
        QJsonObject payload = stage["payloads"].toObject();
        int count = 0;
        while(payload.contains("option_" + QString::number(count)))
        {
            if(payload["option_" + QString::number(count)].isString())
            {
                ui->textRocket->append("\t\tOption " + QString::number(count) + ": " + payload["option_" + QString::number(count)].toString());
            }
        }
        if(payload.contains("composite_fairing") && payload["composite_fairing"].isObject())
        {
            ui->textRocket->append("\t\tComposite fairing:");
            QJsonObject fairing = payload["composite_fairing"].toObject();
            if(fairing.contains("height") && fairing["height"].isObject())
            {
                QJsonObject height = fairing["height"].toObject();
                if(height.contains("meters") && height["meters"].isDouble())
                {
                    ui->textRocket->append("\t\t\tHeight: " + QString::number(height["meters"].toInt()) + " m");
                }
            }
            if(fairing.contains("diameter") && fairing["diameter"].isObject())
            {
                QJsonObject diameter = fairing["diameter"].toObject();
                if(diameter.contains("meters") && diameter["meters"].isDouble())
                {
                    ui->textRocket->append("\t\t\tDiameter: " + QString::number(diameter["meters"].toInt()) + " m");
                }
            }
        }
    }
}

void MainWindow::parseEngines(QJsonObject engine, QString tabulator)
{
    ui->textRocket->append(tabulator + "Engines:");
    if(engine.contains("number") && engine["number"].isDouble())
    {
        ui->textRocket->append(tabulator + "\tNumber: " + QString::number(engine["number"].toInt()));
    }
    if(engine.contains("type") && engine["type"].isString())
    {
        ui->textRocket->append(tabulator + "\tType: " + engine["type"].toString());
    }
    if(engine.contains("version") && engine["version"].isString())
    {
        ui->textRocket->append(tabulator + "\tVersion: " + engine["version"].toString());
    }
    if(engine.contains("layout") && engine["layout"].isString())
    {
        ui->textRocket->append(tabulator + "\tLayout: " + engine["layout"].toString());
    }
    if(engine.contains("engine_loss_max") && engine["engine_los_max"].isDouble())
    {
        ui->textRocket->append(tabulator + "\tEngine loss max: " + QString::number(engine["engine_loss_max"].toInt()));
    }
    if(engine.contains("propellant_1") && engine["propellant_1"].isString())
    {
        ui->textRocket->append(tabulator + "\tPropellant 1: " + engine["propellant_1"].toString());
    }
    if(engine.contains("propellant_2") && engine["propellant_2"].isString())
    {
        ui->textRocket->append(tabulator + "\tPropellant 2: " + engine["propellant_2"].toString());
    }
    if(engine.contains("thrust_sea_level") && engine["thrust_sea_level"].isObject())
    {
        QJsonObject thrust = engine["thrust_sea_level"].toObject();
        if(thrust.contains("kN") && thrust["kN"].isDouble())
        {
            ui->textRocket->append(tabulator + "\tThrust at sea level: " + QString::number(thrust["kN"].toInt()) + " kN");
        }
    }
    if(engine.contains("thrust_vacuum") && engine["thrust_vacuum"].isObject())
    {
        QJsonObject thrust = engine["thrust_vacuum"].toObject();
        if(thrust.contains("kN") && thrust["kN"].isDouble())
        {
            ui->textRocket->append(tabulator + "\tThrust in vacuum: " + QString::number(thrust["kN"].toInt()) + " kN");
        }
    }
    if(engine.contains("thrust_to_weight") && engine["thrust_to_weight"].isDouble())
    {
        ui->textRocket->append(tabulator + "\tThrust to weight ratio: " + QString::number(engine["thrust_to_weight"].toInt()));
    }
}

void MainWindow::parseCapsule(QJsonObject capsule)
{
    if(capsule.contains("id") && capsule["id"].isString())
    {
        ui->textCapsule->append("ID: " + capsule["id"].toString());
    }
    if(capsule.contains("name") && capsule["name"].isString())
    {
        ui->textCapsule->append("Name: " + capsule["name"].toString());
    }
    if(capsule.contains("type") && capsule["type"].isString())
    {
        ui->textCapsule->append("Type: " + capsule["type"].toString());
    }
    if(capsule.contains("active") && capsule["active"].isBool())
    {
        ui->textCapsule->append("Active: " + boolToString(capsule["active"].toBool()));
    }
    if(capsule.contains("height_w_trunk") && capsule["height_w_trunk"].isObject())
    {
        QJsonObject height = capsule["height_w_trunk"].toObject();
        if(height.contains("meters") && height["meters"].isDouble())
        {
            ui->textCapsule->append("Height with trunk: " + QString::number(height["meters"].toInt()) + " m");
        }
    }
    if(capsule.contains("diameter") && capsule["diameter"].isObject())
    {
        QJsonObject diameter = capsule["diameter"].toObject();
        if(diameter.contains("meters") && diameter["meters"].isDouble())
        {
            ui->textCapsule->append("Diameter: " + QString::number(diameter["meters"].toInt()) + " m");
        }
    }
    if(capsule.contains("sidewall_angle_deg") && capsule["sidewall_angle_deg"].isDouble())
    {
        ui->textCapsule->append("Sidewall angle: " + QString::number(capsule["sidewall_angle_deg"].toInt()) + " degrees");
    }
    if(capsule.contains("orbit_duration_yr") && capsule["orbit_duration_yr"].isDouble())
    {
        ui->textCapsule->append("Orbit duration: " + QString::number(capsule["orbit_duration_yr"].toInt()) + " years");
    }
    if(capsule.contains("variations") && capsule["variations"].isObject())
    {
        ui->textCapsule->append("Variations:");
        QJsonObject variations = capsule["variations"].toObject();
        if(variations.contains("cargo") && variations["cargo"].isObject())
        {
            QString string;
            string.append("\tCargo: ");
            QJsonObject cargo = variations["cargo"].toObject();
            if(cargo.contains("description") && cargo["description"].isString())
            {
                string.append(cargo["description"].toString());
            }
            ui->textCapsule->append(string);
        }
        if(variations.contains("crew") && variations["crew"].isObject())
        {
            QString string;
            string.append("\tCrew: ");
            QJsonObject crew = variations["crew"].toObject();
            if(crew.contains("description") && crew["description"].isString())
            {
                string.append(crew["description"].toString());
            }
            ui->textCapsule->append(string);
        }
        if(variations.contains("dragonlab") && variations["dragonlab"].isObject())
        {
            QString string;
            string.append("\tDragonlab: ");
            QJsonObject dragonlab = variations["dragonlab"].toObject();
            if(dragonlab.contains("description") && dragonlab["description"].isString())
            {
                string.append(dragonlab["description"].toString());
            }
            ui->textCapsule->append(string);
        }
    }
    if(capsule.contains("heat_shield") && capsule["heatshield"].isObject())
    {
        ui->textCapsule->append("Heat shield:");
        QJsonObject heatshield = capsule["heatshield"].toObject();
        if(heatshield.contains("material") && heatshield["material"].isString())
        {
            ui->textCapsule->append("\tMaterial: " + heatshield["material"].toString());
        }
        if(heatshield.contains("size_meters") && heatshield["size_meters"].isDouble())
        {
            ui->textCapsule->append("\tSize: " + QString::number(heatshield["size_meters"].toInt()) + " m");
        }
        if(heatshield.contains("temp_degrees") && heatshield["temp_degrees"].isDouble())
        {
            ui->textCapsule->append("\tTemperature: " + QString::number(heatshield["temp_degrees"].toInt()) + " degrees C");
        }
        if(heatshield.contains("dev_partner") && heatshield["dev_partner"].isString())
        {
            ui->textCapsule->append("\tDevelopment partner: " + heatshield["dev_partner"].toString());
        }
    }
    if(capsule.contains("thrusters") && capsule["thrusters"].isObject())
    {
        ui->textCapsule->append("Thrusters:");
        QJsonObject thrusters = capsule["thrusters"].toObject();
        if(thrusters.contains("type") && thrusters["type"].isString())
        {
            ui->textCapsule->append("\tType: " + thrusters["type"].toString());
        }
        if(thrusters.contains("amount") && thrusters["amount"].isDouble())
        {
            ui->textCapsule->append("\tAmount: " + QString::number(thrusters["amount"].toInt()));
        }
        if(thrusters.contains("pods") && thrusters["pods"].isDouble())
        {
            ui->textCapsule->append("\tPods: " + QString::number(thrusters["pods"].toInt()));
        }
        if(thrusters.contains("fuel_1") && thrusters["fuel_1"].isString())
        {
            ui->textCapsule->append("\tFuel 1: " + thrusters["fuel_1"].toString());
        }
        if(thrusters.contains("fuel_2") && thrusters["fuel_2"].isString())
        {
            ui->textCapsule->append("\tFuel 2: " + thrusters["fuel_2"].toString());
        }
        if(thrusters.contains("thrust") && thrusters["thrust"].isObject())
        {
            QJsonObject thrust = thrusters["thrust"].toObject();
            if(thrust.contains("kN") && thrust["kN"].isDouble())
            {
                ui->textCapsule->append("\tThrust: " + QString::number(thrust["kN"].toInt()) + " kN");
            }
        }
    }
    if(capsule.contains("launch_payload_mass") && capsule["launch_payload_mass"].isObject())
    {
        QJsonObject lpm = capsule["launch_payload_mass"].toObject();
        if(lpm.contains("kg") && lpm["kg"].isDouble())
        {
            ui->textCapsule->append("Launch payload mass: " + QString::number(lpm["kg"].toInt()) + " kg");
        }
    }
    if(capsule.contains("launch_payload_vol") && capsule["launch_payload_vol"].isObject())
    {
        QJsonObject lpv = capsule["launch_payload_vol"].toObject();
        if(lpv.contains("cubic_meters") && lpv["cubic_meters"].isDouble())
        {
            ui->textCapsule->append("Launch payload volume: " + QString::number(lpv["cubic_meters"].toInt()) + " m3");
        }
    }
    if(capsule.contains("return_payload_mass") && capsule["return_payload_mass"].isObject())
    {
        QJsonObject rpm = capsule["return_payload_mass"].toObject();
        if(rpm.contains("kg") && rpm["kg"].isDouble())
        {
            ui->textCapsule->append("Return payload mass: " + QString::number(rpm["return_payload_mass"].toInt()) + " kg");
        }
    }
    if(capsule.contains("return_payload_vol") && capsule["return_payload_vol"].isObject())
    {
        QJsonObject rpv = capsule["return_payload_vol"].toObject();
        if(rpv.contains("cubic_meteres") && rpv["cubic_meters"].isDouble())
        {
            ui->textCapsule->append("Return payload volume: " + QString::number(rpv["return_payload_vol"].toInt()) + " m3");
        }
    }
    if(capsule.contains("pressurized_capsule") && capsule["pressurized_capsule"].isObject())
    {
        QJsonObject capsule = capsule["pressurized_capsule"].toObject();
        if(capsule.contains("payload_volume") && capsule["payload_volume"].isObject())
        {
            QJsonObject pv = capsule["payload_volume"].toObject();
            if(pv.contains("cubic_meters") && pv["cubic_meters"].isDouble())
            {
                ui->textCapsule->append("Pressurized capsule payload volume: " + QString::number(pv["cubic_meters"].toInt()) + " m3");
            }
        }
    }
    if(capsule.contains("trunk") && capsule["trunk"].isObject())
    {
        ui->textCapsule->append("Trunk:");
        QJsonObject trunk = capsule["trunk"].toObject();
        if(trunk.contains("trunk_volume") && trunk["trunk_volume"].isObject())
        {
            QJsonObject vol = trunk["trunk_volume"].toObject();
            if(vol.contains("cubic_meters") && vol["cubic_meters"].isDouble())
            {
                ui->textCapsule->append("\tTrunk volume: " + QString::number(vol["cubic_meters"].toInt()) + " m3");
            }
        }
        if(trunk.contains("cargo") && trunk["cargo"].isObject())
        {
            ui->textCapsule->append("\tCargo:");
            QJsonObject cargo = trunk["cargo"].toObject();
            if(cargo.contains("solar_array") && cargo["solar_array"].isDouble())
            {
                ui->textCapsule->append("\t\tSolar array: " + QString::number(cargo["solar_array"].toInt()));
            }
            if(cargo.contains("unpressurized_cargo") && cargo["unpressurized_cargo"].isBool())
            {
                ui->textCapsule->append("\t\tUnpressurized cargo: " + boolToString(cargo["unpressurized_cargo"].toBool()));
            }
        }
    }
}

void MainWindow::parseLaunchpad(QJsonObject launchpad)
{
    if(launchpad.contains("id") && launchpad["id"].isString())
    {
        ui->textLaunchpad->append("ID: " + launchpad["id"].toString());
    }
    if(launchpad.contains("full_name") && launchpad["full_name"].isString())
    {
        ui->textLaunchpad->append("Full name: " + launchpad["full_name"].toString());
    }
    if(launchpad.contains("status") && launchpad["status"].isString())
    {
        ui->textLaunchpad->append("Status: " + launchpad["status"].toString());
    }
    if(launchpad.contains("location") && launchpad["location"].isObject())
    {
        ui->textLaunchpad->append("Location:");
        QJsonObject location = launchpad["location"].toObject();
        if(location.contains("name") && location["name"].isString())
        {
            ui->textLaunchpad->append("\tName: " + location["name"].toString());
        }
        if(location.contains("region") && location["region"].isString())
        {
            ui->textLaunchpad->append("\tRegion: " + location["region"].toString());
        }
        if(location.contains("latitude") && location["latitude"].isDouble())
        {
            ui->textLaunchpad->append("\tLatitude: " + QString::number(location["latitude"].toDouble(), 'f', 10));
        }
        if(location.contains("longitude") && location["longitude"].isDouble())
        {
            ui->textLaunchpad->append("\tLongitude: " + QString::number(location["longitude"].toDouble(), 'f', 10));
        }
    }
    if(launchpad.contains("vehicles_launched") && launchpad["vehicles_launched"].isString())
    {
        ui->textLaunchpad->append("Vehicles launched: " + launchpad["vehicles_launched"].toString());
    }
}

void MainWindow::parseLaunch(QJsonObject launch)
{
    if(launch.contains("flight_number") && launch["flight_number"].isDouble())
    {
        ui->textLaunches->append("Flight number: " + QString::number(launch["flight_number"].toInt()));
    }
    if(launch.contains("launch_year") && launch["launch_year"].isString())
    {
        ui->textLaunches->append("Launch year: " + launch["launch_year"].toString());
    }
    if(launch.contains("launch_date_utc") && launch["launch_date_local"].isString())
    {
        ui->textLaunches->append("Launch date (UTC): " + launch["launch_date_utc"].toString());
    }
    if(launch.contains("launch_date_local") && launch["launch_date_local"].isString())
    {
        ui->textLaunches->append("Launch date (local): " + launch["launch_date_local"].toString());
    }
    if(launch.contains("rocket") && launch["rocket"].isObject())
    {
        QJsonObject rocket = launch["rocket"].toObject();
        ui->textLaunches->append("Rocket:");
        if(rocket.contains("rocket_id") && rocket["rocket_id"].isString())
        {
            ui->textLaunches->append("\tRocket ID: " + rocket["rocket_id"].toString());
        }
        if(rocket.contains("rocket_name") && rocket["rocket_name"].isString())
        {
            ui->textLaunches->append("\tRocket name: " + rocket["rocket_name"].toString());
        }
        if(rocket.contains("rocket_type") && rocket["rocket_type"].isString())
        {
            ui->textLaunches->append("\tRocket type: " + rocket["rocket_type"].toString());
        }
        if(rocket.contains("first_stage") && rocket["first_stage"].isObject())
        {
            QJsonObject firstStage = rocket["first_stage"].toObject();
            ui->textLaunches->append("\tFirst stage:");
            if(firstStage.contains("cores") && firstStage["cores"].isArray())
            {
                QJsonArray coreArray = firstStage["cores"].toArray();
                ui->textLaunches->append("\t\tCores:");
                for(int i = 0; i < coreArray.count(); ++i)
                {
                    ui->textLaunches->append("\t\t\t" + QString::number(i) + ":");
                    QJsonObject core = coreArray[i].toObject();
                    if(core.contains("core_serial") && core["core_serial"].isString())
                    {
                        ui->textLaunches->append("\t\t\t\tCore serial number: " + core["core_serial"].toString());
                    }
                    if(core.contains("reused") && core["reused"].isBool())
                    {
                        ui->textLaunches->append("\t\t\t\tReused: " + boolToString(core["reused"].toBool()));
                    }
                    if(core.contains("land_success") && core["land_success"].isBool())
                    {
                        ui->textLaunches->append("\t\t\t\tLand success: " + boolToString(core["land_success"].toBool()));
                    }
                    if(core.contains("landing_type") && core["landing_type"].isString())
                    {
                        ui->textLaunches->append("\t\t\t\tLanding type: " + core["landing_type"].toString());
                    }
                    if(core.contains("landing_vehicle") && core["landing_vehicle"].isString())
                    {
                        ui->textLaunches->append("\t\t\t\tLanding vehicle: " + core["landing_vehicle"].toString());
                    }
                }
            }
        }
        if(rocket.contains("second_stage") && rocket["second_stage"].isObject())
        {
            QJsonObject secondStage = rocket["second_stage"].toObject();
            ui->textLaunches->append("\tSecond stage:");
            if(secondStage.contains("payloads") && secondStage["payloads"].isArray())
            {
                QJsonArray payloadArray = secondStage["payloads"].toArray();
                ui->textLaunches->append("\t\tPayloads:");
                for(int i = 0; i < payloadArray.count(); ++i)
                {
                    ui->textLaunches->append("\t\t\t" + QString::number(i) + ":");
                    QJsonObject payload = payloadArray[i].toObject();
                    if(payload.contains("payload_id") && payload["payload_id"].isString())
                    {
                        ui->textLaunches->append("\t\t\t\tPayload ID: " + payload["payload_id"].toString());
                    }
                    if(payload.contains("reused") && payload["reused"].isBool())
                    {
                        ui->textLaunches->append("\t\t\t\tReused: " + boolToString(payload["reused"].toBool()));
                    }
                    if(payload.contains("customers") && payload["customers"].isArray())
                    {
                        QJsonArray customers = payload["customers"].toArray();
                        ui->textLaunches->append("\t\t\t\tCustomers:");
                        for(int j = 0; j < customers.count(); ++j)
                        {
                            if(customers[j].isString())
                            {
                                ui->textLaunches->append("\t\t\t\t\t" + QString::number(j) + ": " + customers[j].toString());
                            }
                        }
                    }
                    if(payload.contains("payload_type") && payload["payload_type"].isString())
                    {
                        ui->textLaunches->append("\t\t\t\tPayload type: " + payload["payload_type"].toString());
                    }
                    if(payload.contains("payload_mass_kg") && payload["payload_mass_kg"].isDouble())
                    {
                        ui->textLaunches->append("\t\t\t\tPayload mass: " + QString::number(payload["payload_mass_kg"].toInt()) + " kg");
                    }
                    if(payload.contains("orbit") && payload["orbit"].isString())
                    {
                        ui->textLaunches->append("\t\t\t\tOrbit: " + payload["orbit"].toString());
                    }
                }
            }
        }
    }
    if(launch.contains("reuse") && launch["reuse"].isObject())
    {
        QJsonObject reuse = launch["reuse"].toObject();
        ui->textLaunches->append("Reuse:");
        QStringList list;
        list << "core" << "side_core1" << "side_core2" << "fairings" << "capsule";
        for(int i = 0; i < list.count(); ++i)
        {
            if(reuse.contains(list.at(i)) && reuse[list.at(i)].isBool())
            {
                QString string = list.at(i);
                string.replace('_',' ');
                string.replace(0, 1, string.at(0).toUpper());
                ui->textLaunches->append("\t" + string + ": " + boolToString(reuse[list.at(i)].toBool()));
            }
        }
    }
    if(launch.contains("launch_site") && launch["launch_site"].isObject())
    {
        QJsonObject site = launch["launch_site"].toObject();
        ui->textLaunches->append("Launch site:");
        if(site.contains("site_id") && site["site_id"].isString())
        {
            ui->textLaunches->append("\tSite ID: " + site["site_id"].toString());
        }
        if(site.contains("site_name_long") && site["site_name_long"].isString())
        {
            ui->textLaunches->append("\tSite name: " + site["site_name_long"].toString());
        }
    }
    if(launch.contains("launch_success") && launch["launch_success"].isBool())
    {
        ui->textLaunches->append("Launch success: " + boolToString(launch["launch_success"].toBool()));
    }
    if(launch.contains("links") && launch["links"].isObject())
    {
        QJsonObject links = launch["links"].toObject();
        ui->textLaunches->append("Links:");
        QStringList list;
        list << "mission_patch" << "presskit" << "article_link" << "video_link";
        for(int i = 0; i < list.count(); ++i)
        {
            if(links.contains(list.at(i)) && links[list.at(i)].isString())
            {
                QString string = list.at(i);
                string.replace('_',' ');
                string.replace(0, 1, string.at(0).toUpper());
                ui->textLaunches->append("\t" + string + ": " + links[list.at(i)].toString());
            }
        }
    }
}
