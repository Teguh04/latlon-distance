#include "mainwindow.h"
#include "ui_mainwindow.h"
#include "connection.h"
#include <QString>
#include <QtMath>
#include <QSqlQuery>

MainWindow::MainWindow(QWidget *parent) :
    QMainWindow(parent),
    ui(new Ui::MainWindow)
{
    ui->setupUi(this);

    //connection to database
    connection();
}

MainWindow::~MainWindow()
{
    delete ui;
}

void MainWindow::on_pushButton_clicked()
{
    //get latitude and longitude from ui
    latitude = ui->latitude->toPlainText().toDouble();
    longitude = ui->longitude->toPlainText().toDouble();

    //query to get latitude, longitude and template from way_point table in database
    QSqlQuery query1;
    query1.exec("SELECT w, latitude, longitude, template FROM way_point ORDER BY template ASC;");

    //get smallest distance from ui latitude longitude and latitude longitude in database
    double start = 1000000;
    double tmp;
    int template_way, index;
    while (query1.next()){
        tmp = distance_two_point(latitude,longitude,query1.value(1).toDouble(),query1.value(2).toDouble());
        if(start > tmp){
            start = tmp;
            template_way = query1.value(3).toInt();
            index = query1.value(0).toInt();
        }
    }

    //query to get value from two point in database
    QList<double> result;
    QSqlQuery query2;
    query2.prepare(("SELECT r FROM line WHERE template = ? ORDER BY w1 ASC;"));
    query2.addBindValue(template_way);
    query2.exec();
    while (query2.next()) {
        result.append(query2.value(0).toDouble());
    }

    //if template not found in database
    if(result.isEmpty()){
        ui->textEdit->setText("Template not Found");
    } else{

        //get distance from ui latitude longitude to destination point in database
        double sum = start;
        for(int i=index-2;i >= 0;i--){
            sum = sum + result[i];
        }

        //show distance value in KM
        ui->textEdit->setText(QString::number(sum) + " KM to template " + QString::number(template_way));
    }
}

void MainWindow::on_pushButton_2_clicked()
{
    bool ok = false;
    double r;
    int tmp = 1;

    //query to truncate line table in database
    QSqlQuery query3;
    query3.exec("TRUNCATE line;");

    //query to get latitude, longitude and tamplate from way_point table
    QSqlQuery query4;
    query4.exec("SELECT latitude, longitude, template FROM way_point ORDER BY template ASC;");
    while (query4.next()){
        latitude_way_point.append(query4.value(0).toString());
        longitude_way_point.append(query4.value(1).toString());
        template_way_point.append(query4.value(2).toString());
    }

    //function to get distance value from two point
    for(int i=1; i < template_way_point.size(); i++){
        if(template_way_point[i-1] == template_way_point[i]){
            r = distance_two_point(latitude_way_point[i-1].toDouble(&ok),longitude_way_point[i-1].toDouble(&ok),latitude_way_point[i].toDouble(&ok),longitude_way_point[i].toDouble(&ok));

            //save value in database
            QSqlQuery query5;
            query5.prepare("INSERT INTO line(w1, w2, r, template) VALUES(?, ?, ?, ?);");
            query5.addBindValue(tmp);
            query5.addBindValue(tmp+1);
            query5.addBindValue(r);
            query5.addBindValue(template_way_point[i]);
            query5.exec();
            tmp++;
        } else{
            tmp = 1;
        }
    }
    ui->textEdit->setText("The Distance Between Points on the Database has been Calculated");
}

//function convert latitude and longitude to radian
double MainWindow::toRadians(const double degree){
    long double one_deg = (M_PI)/180;
    return (one_deg * degree);
}

//function to calculate distance from two point
double MainWindow::distance_two_point(double lat1, double long1, double lat2, double long2){
    lat1 = toRadians(lat1);
    long1 = toRadians(long1);
    lat2 = toRadians(lat2);
    long2 = toRadians(long2);

    //haversine formula
    double dlong = long2 - long1;
    double dlat = lat2 - lat1;
    double ans = pow(qSin(dlat/2), 2) + qCos(lat1) * qCos(lat2) * pow(qSin(dlong/2), 2);
    ans = 2 * qAtan2(sqrt(ans), sqrt(1-ans));
    double R = 6371;

    //calculate the result
    ans = ans * R;
    return ans;
}
