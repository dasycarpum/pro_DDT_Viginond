#ifndef RESEAU_H
#define RESEAU_H

#include <QObject>
#include <QHostInfo>
#include <QNetworkReply>
#include <QEventLoop>
#include <QTextBrowser>
#include <QNetworkProxy>

#include "fichier.h"

class Proxy : public QNetworkProxy
{
    QMap<QString, QList<QString>> proxies;      // liste des proxies préenregistrés, par site client (DDT, Préfecture, ...)

public:
    Proxy(QString const&);
    ~Proxy(){}

    QMap<QString, QList<QString>> Proxies(void) const {return proxies;}
};


class Reseau : public QObject
{
    Q_OBJECT

    QByteArray donnee_page_web;
    const Proxy *proxy;

public:
    explicit Reseau(QObject *parent = nullptr) : QObject(parent) {proxy = nullptr;}
    explicit Reseau(const Proxy * p, QObject *parent = nullptr) : QObject(parent), proxy(p) {}

    QByteArray Donnee_page_web(void) const {return donnee_page_web;}

    bool Test_connexion(QString const& = "google.com");
    bool Test_connexion(QTextBrowser *, QString const& = "Google", QString const& = "google.com");
    void Enregistrer_page_web(QUrl const&);  // type XML ou HTML

signals:

private slots:
    void Reponse_terminee(QNetworkReply *);
};



#endif // RESEAU_H
