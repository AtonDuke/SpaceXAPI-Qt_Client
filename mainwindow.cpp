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
    ui->radioVehicleID->click();
    ui->radioLaunchpadID->click();
    ui->radioLaunchesFlightNo->click();
    ui->textCompany->setOpenExternalLinks(true);
    ui->textLaunches->setOpenExternalLinks(true);
    ui->textLaunchpad->setOpenExternalLinks(true);
    ui->textVehicle->setOpenExternalLinks(true);

    url = QUrl("https://api.spacexdata.com/v2/");

    connect(&manager, SIGNAL(finished(QNetworkReply*)), this, SLOT(downloadFinished(QNetworkReply*)));
    connect(ui->buttonHelp, SIGNAL(clicked(bool)), this, SLOT(onButtonHelp()));
    connect(ui->buttonRefreshCompany, SIGNAL(clicked(bool)), this, SLOT(onButtonRefreshCompany()));
    connect(ui->buttonRefreshVehicle, SIGNAL(clicked(bool)), this, SLOT(onButtonRefreshVehicle()));
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

void MainWindow::onButtonRefreshVehicle()
{
    if(ui->lineSearchVehicle->text() == "how not to land")
    {
        ui->textVehicle->setHtml("<a href=\"https://www.youtube.com/watch?v=bvim4rsNHkQ\">How not to land an orbital rocket booster</a>");
        return;
    }
    if(ui->lineSearchVehicle->text() == "Musk vs Bezos")
    {
        ui->textVehicle->setHtml("<a href=\"https://www.youtube.com/watch?v=40uks4vjdvI\">Elon Musk - When Bezos Tried To Compete against SpaceX</a>");
        return;
    }
    if(ui->lineSearchVehicle->text() == "Kill SpaceX!")
    {
        ui->textVehicle->setHtml("<a href=\"https://www.youtube.com/watch?v=nULPR9MjKNw\">Elon Musk - When \"They\" Tried To Kill SpaceX | MUST WATCH</a>");
        return;
    }
    QString string;
    string.append("vehicles");
    if(ui->radioVehicleID->isChecked() && !ui->lineSearchVehicle->text().isEmpty())
    {
        string.append("/" + ui->lineSearchVehicle->text());
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
        helpString += "Name: Filter by vehicle name.";
        break;
    case 2:
        helpString += "Refresh: If search line is empty, refreshes info, else applies filter and refreshes info.\n";
        helpString += "ID: Filter by ID.\n";
        helpString += "Name: Filter by launchpad name.";
        break;
    case 3:
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
        if(reply->url().toString().contains("vehicles"))
        {
            refreshVehicle(json);
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
    if(company.contains("vehicles") && company["vehicles"].isDouble())
    {
        ui->textCompany->append("Vehicles: " + QString::number(company["vehicles"].toInt()));
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

void MainWindow::refreshVehicle(QJsonDocument doc)
{
    QJsonArray vehicleArray;
    QJsonObject vehicle;
    bool filterFlag = false;
    bool found = false;
    if(!ui->lineSearchVehicle->text().isEmpty() && ui->radioVehicleName->isChecked())
    {
        filterFlag = true;
    }
    ui->textVehicle->clear();
    if(ui->radioVehicleID->isChecked() && !ui->lineSearchVehicle->text().isEmpty())
    {
        vehicle = doc.object();
        if(vehicle.contains("error") && vehicle["error"].isString())
        {
            ui->textVehicle->setText("ERROR: " + vehicle["error"].toString());
            return;
        }
        vehicleArray.append(vehicle);
    }
    else
    {
        vehicleArray = doc.array();
    }
    for(int i = 0; i < vehicleArray.count(); ++i)
    {
        vehicle = vehicleArray.at(i).toObject();
        if(!filterFlag || (vehicle.contains("name") && vehicle["name"].isString() && vehicle["name"].toString() == ui->lineSearchVehicle->text()))
        {
            parseVehicle(vehicle);
            found = true;
            ui->textVehicle->append("");
        }
    }
    if(!found)
    {
        ui->textVehicle->setText("ERROR: No match found!");
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

void MainWindow::parseVehicle(QJsonObject vehicle)
{
    if(vehicle.contains("id") && vehicle["id"].isString())
    {
        ui->textVehicle->append("ID: " + vehicle["id"].toString());
    }
    if(vehicle.contains("name") && vehicle["name"].isString())
    {
        ui->textVehicle->append("Name: " + vehicle["name"].toString());
    }
    if(vehicle.contains("type") && vehicle["type"].isString())
    {
        ui->textVehicle->append("Type: " + vehicle["type"].toString());
    }
    if(vehicle.contains("active") && vehicle["active"].isBool())
    {
        ui->textVehicle->append("Active: " + boolToString(vehicle["active"].toBool()));
    }
    if(vehicle.contains("stages") && vehicle["stages"].isDouble())
    {
        ui->textVehicle->append("Stages: " + QString::number(vehicle["stages"].toInt()));
    }
    if(vehicle.contains("cost_per_launch") && vehicle["cost_per_launch"].isDouble())
    {
        ui->textVehicle->append("Cost per launch: " + QString::number(vehicle["cost_per_launch"].toInt()));
    }
    if(vehicle.contains("success_rate_pct") && vehicle["success_rate_pct"].isDouble())
    {
        ui->textVehicle->append("Success rate: " + QString::number(vehicle["success_rate_pct"].toInt()) + "%");
    }
    if(vehicle.contains("first_flight") && vehicle["first_flight"].isString())
    {
        ui->textVehicle->append("First flight: " + vehicle["first_flight"].toString());
    }
    if(vehicle.contains("country") && vehicle["country"].isString())
    {
        ui->textVehicle->append("Country: " + vehicle["country"].toString());
    }
    if(vehicle.contains("height") && vehicle["height"].isObject())
    {
        QJsonObject height = vehicle["height"].toObject();
        if(height.contains("meters") && height["meters"].isDouble())
        {
            ui->textVehicle->append("Height: " + QString::number(height["meters"].toInt()) + " m");
        }
    }
    if(vehicle.contains("height_w_trunk") && vehicle["height_w_trunk"].isObject())
    {
        QJsonObject height = vehicle["height_w_trunk"].toObject();
        if(height.contains("meters") && height["meters"].isDouble())
        {
            ui->textVehicle->append("Height with trunk: " + QString::number(height["meters"].toInt()) + " m");
        }
    }
    if(vehicle.contains("diameter") && vehicle["diameter"].isObject())
    {
        QJsonObject diameter = vehicle["diameter"].toObject();
        if(diameter.contains("meters") && diameter["meters"].isDouble())
        {
            ui->textVehicle->append("Diameter: " + QString::number(diameter["meters"].toInt()) + " m");
        }
    }
    if(vehicle.contains("mass") && vehicle["mass"].isObject())
    {
        QJsonObject mass = vehicle["mass"].toObject();
        if(mass.contains("mass") && mass["meters"].isDouble())
        {
            ui->textVehicle->append("Mass: " + QString::number(mass["meters"].toInt()) + " m");
        }
    }
    if(vehicle.contains("payload_weights") && vehicle["payload_weights"].isArray())
    {
        QString string;
        string.append("Payload weights:");
        QJsonArray array = vehicle["payload_weights"].toArray();
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
        ui->textVehicle->append(string);
    }
    if(vehicle.contains("first_stage") && vehicle["first_stage"].isObject())
    {
        ui->textVehicle->append("First stage:");
        parseVehicleStage(vehicle["first_stage"].toObject());
    }
    if(vehicle.contains("second_stage") && vehicle["second_stage"].isObject())
    {
        ui->textVehicle->append("Second stage:");
        parseVehicleStage(vehicle["second_stage"].toObject());
    }
    if(vehicle.contains("engines") && vehicle["engines"].isObject())
    {
        parseEngines(vehicle["engines"].toObject(),"");
    }
    if(vehicle.contains("landing_legs") && vehicle["landing_legs"].isObject())
    {
        ui->textVehicle->append("Landing legs:");
        QJsonObject legs = vehicle["landing_legs"].toObject();
        if(legs.contains("number") && legs["number"].isDouble())
        {
            ui->textVehicle->append("\tNumber: " + QString::number(legs["number"].toInt()));
        }
        if(legs.contains("material") && legs["material"].isString())
        {
            ui->textVehicle->append("\tMaterial: " + legs["material"].toString());
        }
    }
    if(vehicle.contains("sidewall_angle_deg") && vehicle["sidewall_angle_deg"].isDouble())
    {
        ui->textVehicle->append("Sidewall angle: " + QString::number(vehicle["sidewall_angle_deg"].toInt()) + " degrees");
    }
    if(vehicle.contains("orbit_duration_yr") && vehicle["orbit_duration_yr"].isDouble())
    {
        ui->textVehicle->append("Orbit duration: " + QString::number(vehicle["orbit_duration_yr"].toInt()) + " years");
    }
    if(vehicle.contains("variations") && vehicle["variations"].isObject())
    {
        ui->textVehicle->append("Variations:");
        QJsonObject variations = vehicle["variations"].toObject();
        if(variations.contains("cargo") && variations["cargo"].isObject())
        {
            QString string;
            string.append("\tCargo: ");
            QJsonObject cargo = variations["cargo"].toObject();
            if(cargo.contains("description") && cargo["description"].isString())
            {
                string.append(cargo["description"].toString());
            }
            ui->textVehicle->append(string);
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
            ui->textVehicle->append(string);
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
            ui->textVehicle->append(string);
        }
    }
    if(vehicle.contains("heat_shield") && vehicle["heatshield"].isObject())
    {
        ui->textVehicle->append("Heat shield:");
        QJsonObject heatshield = vehicle["heatshield"].toObject();
        if(heatshield.contains("material") && heatshield["material"].isString())
        {
            ui->textVehicle->append("\tMaterial: " + heatshield["material"].toString());
        }
        if(heatshield.contains("size_meters") && heatshield["size_meters"].isDouble())
        {
            ui->textVehicle->append("\tSize: " + QString::number(heatshield["size_meters"].toInt()) + " m");
        }
        if(heatshield.contains("temp_degrees") && heatshield["temp_degrees"].isDouble())
        {
            ui->textVehicle->append("\tTemperature: " + QString::number(heatshield["temp_degrees"].toInt()) + " degrees C");
        }
        if(heatshield.contains("dev_partner") && heatshield["dev_partner"].isString())
        {
            ui->textVehicle->append("\tDevelopment partner: " + heatshield["dev_partner"].toString());
        }
    }
    if(vehicle.contains("thrusters") && vehicle["thrusters"].isObject())
    {
        ui->textVehicle->append("Thrusters:");
        QJsonObject thrusters = vehicle["thrusters"].toObject();
        if(thrusters.contains("type") && thrusters["type"].isString())
        {
            ui->textVehicle->append("\tType: " + thrusters["type"].toString());
        }
        if(thrusters.contains("amount") && thrusters["amount"].isDouble())
        {
            ui->textVehicle->append("\tAmount: " + QString::number(thrusters["amount"].toInt()));
        }
        if(thrusters.contains("pods") && thrusters["pods"].isDouble())
        {
            ui->textVehicle->append("\tPods: " + QString::number(thrusters["pods"].toInt()));
        }
        if(thrusters.contains("fuel_1") && thrusters["fuel_1"].isString())
        {
            ui->textVehicle->append("\tFuel 1: " + thrusters["fuel_1"].toString());
        }
        if(thrusters.contains("fuel_2") && thrusters["fuel_2"].isString())
        {
            ui->textVehicle->append("\tFuel 2: " + thrusters["fuel_2"].toString());
        }
        if(thrusters.contains("thrust") && thrusters["thrust"].isObject())
        {
            QJsonObject thrust = thrusters["thrust"].toObject();
            if(thrust.contains("kN") && thrust["kN"].isDouble())
            {
                ui->textVehicle->append("\tThrust: " + QString::number(thrust["kN"].toInt()) + " kN");
            }
        }
    }
    if(vehicle.contains("launch_payload_mass") && vehicle["launch_payload_mass"].isObject())
    {
        QJsonObject lpm = vehicle["launch_payload_mass"].toObject();
        if(lpm.contains("kg") && lpm["kg"].isDouble())
        {
            ui->textVehicle->append("Launch payload mass: " + QString::number(lpm["kg"].toInt()) + " kg");
        }
    }
    if(vehicle.contains("launch_payload_vol") && vehicle["launch_payload_vol"].isObject())
    {
        QJsonObject lpv = vehicle["launch_payload_vol"].toObject();
        if(lpv.contains("cubic_meters") && lpv["cubic_meters"].isDouble())
        {
            ui->textVehicle->append("Launch payload volume: " + QString::number(lpv["cubic_meters"].toInt()) + " m3");
        }
    }
    if(vehicle.contains("return_payload_mass") && vehicle["return_payload_mass"].isObject())
    {
        QJsonObject rpm = vehicle["return_payload_mass"].toObject();
        if(rpm.contains("kg") && rpm["kg"].isDouble())
        {
            ui->textVehicle->append("Return payload mass: " + QString::number(rpm["return_payload_mass"].toInt()) + " kg");
        }
    }
    if(vehicle.contains("return_payload_vol") && vehicle["return_payload_vol"].isObject())
    {
        QJsonObject rpv = vehicle["return_payload_vol"].toObject();
        if(rpv.contains("cubic_meteres") && rpv["cubic_meters"].isDouble())
        {
            ui->textVehicle->append("Return payload volume: " + QString::number(rpv["return_payload_vol"].toInt()) + " m3");
        }
    }
    if(vehicle.contains("pressurized_capsule") && vehicle["pressurized_capsule"].isObject())
    {
        QJsonObject capsule = vehicle["pressurized_capsule"].toObject();
        if(capsule.contains("payload_volume") && capsule["payload_volume"].isObject())
        {
            QJsonObject pv = capsule["payload_volume"].toObject();
            if(pv.contains("cubic_meters") && pv["cubic_meters"].isDouble())
            {
                ui->textVehicle->append("Pressurized capsule payload volume: " + QString::number(pv["cubic_meters"].toInt()) + " m3");
            }
        }
    }
    if(vehicle.contains("trunk") && vehicle["trunk"].isObject())
    {
        ui->textVehicle->append("Trunk:");
        QJsonObject trunk = vehicle["trunk"].toObject();
        if(trunk.contains("trunk_volume") && trunk["trunk_volume"].isObject())
        {
            QJsonObject vol = trunk["trunk_volume"].toObject();
            if(vol.contains("cubic_meters") && vol["cubic_meters"].isDouble())
            {
                ui->textVehicle->append("\tTrunk volume: " + QString::number(vol["cubic_meters"].toInt()) + " m3");
            }
        }
        if(trunk.contains("cargo") && trunk["cargo"].isObject())
        {
            ui->textVehicle->append("\tCargo:");
            QJsonObject cargo = trunk["cargo"].toObject();
            if(cargo.contains("solar_array") && cargo["solar_array"].isDouble())
            {
                ui->textVehicle->append("\t\tSolar array: " + QString::number(cargo["solar_array"].toInt()));
            }
            if(cargo.contains("unpressurized_cargo") && cargo["unpressurized_cargo"].isBool())
            {
                ui->textVehicle->append("\t\tUnpressurized cargo: " + boolToString(cargo["unpressurized_cargo"].toBool()));
            }
        }
    }
}

void MainWindow::parseVehicleStage(QJsonObject stage)
{
    if(stage.contains("reusable") && stage["reusable"].isBool())
    {
        ui->textVehicle->append("\tReusable: " + boolToString(stage["reusable"].toBool()));
    }
    if(stage.contains("engines") && stage["engines"].isDouble())
    {
        ui->textVehicle->append("\tEngines: " + QString::number(stage["engines"].toInt()));
    }
    if(stage.contains("engines") && stage["engines"].isObject())
    {
        parseEngines(stage["engines"].toObject(),"\t");
    }
    if(stage.contains("fuel_amount_tons") && stage["fuel_amount_tons"].isDouble())
    {
        ui->textVehicle->append("\tFuel amount: " + QString::number(stage["fuel_amount_tons"].toInt()) + " tons");
    }
    if(stage.contains("burn_time_sec") && stage["burn_time_sec"].isDouble())
    {
        ui->textVehicle->append("\tBurn time: " + QString::number(stage["burn_time_sec"].toInt()) + " seconds");
    }
    if(stage.contains("payloads") && stage["payloads"].isObject())
    {
        ui->textVehicle->append("\tPayloads:");
        QJsonObject payload = stage["payloads"].toObject();
        int count = 0;
        while(payload.contains("option_" + QString::number(count)))
        {
            if(payload["option_" + QString::number(count)].isString())
            {
                ui->textVehicle->append("\t\tOption " + QString::number(count) + ": " + payload["option_" + QString::number(count)].toString());
            }
        }
        if(payload.contains("composite_fairing") && payload["composite_fairing"].isObject())
        {
            ui->textVehicle->append("\t\tComposite fairing:");
            QJsonObject fairing = payload["composite_fairing"].toObject();
            if(fairing.contains("height") && fairing["height"].isObject())
            {
                QJsonObject height = fairing["height"].toObject();
                if(height.contains("meters") && height["meters"].isDouble())
                {
                    ui->textVehicle->append("\t\t\tHeight: " + QString::number(height["meters"].toInt()) + " m");
                }
            }
            if(fairing.contains("diameter") && fairing["diameter"].isObject())
            {
                QJsonObject diameter = fairing["diameter"].toObject();
                if(diameter.contains("meters") && diameter["meters"].isDouble())
                {
                    ui->textVehicle->append("\t\t\tDiameter: " + QString::number(diameter["meters"].toInt()) + " m");
                }
            }
        }
    }
}

void MainWindow::parseEngines(QJsonObject engine, QString tabulator)
{
    ui->textVehicle->append(tabulator + "Engines:");
    if(engine.contains("number") && engine["number"].isDouble())
    {
        ui->textVehicle->append(tabulator + "\tNumber: " + QString::number(engine["number"].toInt()));
    }
    if(engine.contains("type") && engine["type"].isString())
    {
        ui->textVehicle->append(tabulator + "\tType: " + engine["type"].toString());
    }
    if(engine.contains("version") && engine["version"].isString())
    {
        ui->textVehicle->append(tabulator + "\tVersion: " + engine["version"].toString());
    }
    if(engine.contains("layout") && engine["layout"].isString())
    {
        ui->textVehicle->append(tabulator + "\tLayout: " + engine["layout"].toString());
    }
    if(engine.contains("engine_loss_max") && engine["engine_los_max"].isDouble())
    {
        ui->textVehicle->append(tabulator + "\tEngine loss max: " + QString::number(engine["engine_loss_max"].toInt()));
    }
    if(engine.contains("propellant_1") && engine["propellant_1"].isString())
    {
        ui->textVehicle->append(tabulator + "\tPropellant 1: " + engine["propellant_1"].toString());
    }
    if(engine.contains("propellant_2") && engine["propellant_2"].isString())
    {
        ui->textVehicle->append(tabulator + "\tPropellant 2: " + engine["propellant_2"].toString());
    }
    if(engine.contains("thrust_sea_level") && engine["thrust_sea_level"].isObject())
    {
        QJsonObject thrust = engine["thrust_sea_level"].toObject();
        if(thrust.contains("kN") && thrust["kN"].isDouble())
        {
            ui->textVehicle->append(tabulator + "\tThrust at sea level: " + QString::number(thrust["kN"].toInt()) + " kN");
        }
    }
    if(engine.contains("thrust_vacuum") && engine["thrust_vacuum"].isObject())
    {
        QJsonObject thrust = engine["thrust_vacuum"].toObject();
        if(thrust.contains("kN") && thrust["kN"].isDouble())
        {
            ui->textVehicle->append(tabulator + "\tThrust in vacuum: " + QString::number(thrust["kN"].toInt()) + " kN");
        }
    }
    if(engine.contains("thrust_to_weight") && engine["thrust_to_weight"].isDouble())
    {
        ui->textVehicle->append(tabulator + "\tThrust to weight ratio: " + QString::number(engine["thrust_to_weight"].toInt()));
    }
}
