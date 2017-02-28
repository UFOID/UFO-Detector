#include "planechecker.h"
#include <QNetworkReply>
#include <QJsonDocument>
#include <QJsonArray>
#include <QJsonObject>
#define API_URL "http://boller.site:8080/flights/api"

PlaneChecker::PlaneChecker(QObject *parent, QString info) : QObject(parent)
{

    QStringList list = info.split("\n");
    QString par1 = list.at(0);
    QString par2 = list.at(1);
    QString par3 = list.at(2);
    url = QString(API_URL) + "?pos1="+par1+"&pos2="+par2+"&pos3="+par3;

    manager = new QNetworkAccessManager(this);
    connect(manager, SIGNAL(finished(QNetworkReply*)), this, SLOT(readReply(QNetworkReply*)) );
}

void PlaneChecker::callApi()
{

    QNetworkRequest req;
    req.setUrl(QUrl(url));
    manager->get(req);

}

void PlaneChecker::readReply(QNetworkReply *reply)
{
    if(reply->error() == QNetworkReply::NoError)
    {

        QString strReply = (QString)reply->readAll();
        QJsonDocument jsonResponse = QJsonDocument::fromJson(strReply.toUtf8());
        QJsonArray array = jsonResponse.array();
        if (array.isEmpty()){
        }
        else {
            qDebug() << "amount of planes " + QString(array.size());
        }

        emit foundNumberOfPlanes(array.size());
    }
    else qWarning() <<  "error calling plane check API" ;
}


