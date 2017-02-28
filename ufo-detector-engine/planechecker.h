#ifndef PLANECHECKER_H
#define PLANECHECKER_H

#include <QObject>
#include <QNetworkAccessManager>

class PlaneChecker : public QObject
{
    Q_OBJECT
public:
    explicit PlaneChecker(QObject *parent = 0, QString info = "");


private:
    QString url;
    QNetworkAccessManager *manager;


signals:
    void foundNumberOfPlanes(int amount);

public slots:
    void callApi();
private slots:
    void readReply(QNetworkReply* reply);
};

#endif // PLANECHECKER_H
