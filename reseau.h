#ifndef RESEAU_H
#define RESEAU_H

#include <QObject>
#include <QHostInfo>
#include <QNetworkReply>
#include <QEventLoop>

class Reseau : public QObject
{
    Q_OBJECT

    QByteArray donnee_page_web;

public:
    explicit Reseau(QObject *parent = nullptr);

    QByteArray Donnee_page_web(void) const {return donnee_page_web;}

    bool Test_connexion(QString const& = "google.com");
    void Enregistrer_page_web(QUrl const&);  // type XML ou HTML

signals:

private slots:
    void Reponse_terminee(QNetworkReply *);
};

#endif // RESEAU_H
