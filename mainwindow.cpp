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

MainWindow::MainWindow(QWidget *parent) :
    QMainWindow(parent),
    ui(new Ui::MainWindow)
{
    ui->setupUi(this);

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
//    QNetworkRequest request();
//    QNetworkReply *reply = manager.get(request);
}

void MainWindow::onButtonRefreshLaunches()
{
//    QNetworkRequest request();
//    QNetworkReply *reply = manager.get(request);
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
        helpString += "Latest: Display only latest launches.\n";
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

}

void MainWindow::refreshLaunches(QJsonDocument doc)
{

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
