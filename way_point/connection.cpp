#include "connection.h"
#include <QSqlDatabase>
#include <QDebug>
#include <QSettings>

connection::connection(){

    //database setting
    QSettings settings("config/config.ini", QSettings::IniFormat);
    settings.beginGroup("DATABASE");
    const QStringList childKeys = settings.childKeys();
    QStringList values;
    foreach (const QString &childKey, childKeys){
        values << settings.value(childKey).toString();
    }
    settings.endGroup();

    //connect to database postgres
    QSqlDatabase db = QSqlDatabase::addDatabase(values[0]);
    db.setDatabaseName(values[1]);
    db.setHostName(values[2]);
    db.setPassword(values[3]);
    db.setPort(values[4].toInt());
    db.setUserName(values[5]);

    bool ok = db.open();
    if(ok != true){
        qDebug() << "Connection Database Failed!";
        exit(0);
    }
    else{
        qDebug() << "Connection Database OK!";
    }
}
