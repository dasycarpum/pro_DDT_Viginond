#include "reseau.h"

Reseau::Reseau(QObject *parent) : QObject(parent)
{

}

/** Test de la connexion de l'ordinateur au réseau internet
    ======================================================= */
bool Reseau::Test_connexion(const QString & nom_site)
{
    QHostInfo info =QHostInfo::fromName(nom_site);

    if (info.error() != QHostInfo::NoError)
        return false;
    else
        return true;
}

/** Enregistrement d'une page web  pour récupérer son contenu
    =========================================================*/
void Reseau::Enregistrer_page_web(const QUrl & url)
{
    QNetworkAccessManager *manager = new QNetworkAccessManager(this);

    const QNetworkRequest requete(url);

    connect(manager, SIGNAL(finished(QNetworkReply *)), this, SLOT(Reponse_terminee(QNetworkReply *)));

    QNetworkReply *reponse = manager->get(requete);

    /* Temporisation : attente de la fin de l'enregistrement */
    QEventLoop loop;
    connect(reponse, SIGNAL(finished()), &loop, SLOT(quit()));
    loop.exec();
}

/** SLOT en lien avec l'enregistrement d'une page Web
    ================================================= */
void Reseau::Reponse_terminee(QNetworkReply * reponse)
{
    /*Indique le code d'état HTTP reçu du serveur HTTP (comme 200, 304, 404, 401, etc.)*/
    QVariant code_etat = reponse->attribute(QNetworkRequest::HttpStatusCodeAttribute);

    /*Si la requête est traitée avec succès, on lit toutes les données, renvoyées sous forme de tableau d'octets */
    if (code_etat == 200)
        donnee_page_web = reponse->readAll();
    else
        qDebug() << code_etat;
}
