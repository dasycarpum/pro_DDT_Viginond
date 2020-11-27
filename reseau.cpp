#include "reseau.h"

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

bool Reseau::Test_connexion(QTextBrowser * textBrowser, const QString & nom, const QString & adresse)
{
    QHostInfo info =QHostInfo::fromName(adresse);

    if (info.error() != QHostInfo::NoError){
        textBrowser->append(QString("<font color=darkRed><b>Pas de connexion %1 %2 !</b></font>")
                            .arg(nom == "Google" ? "à internet" : "au serveur")
                            .arg(nom == "Google" ? "" : nom));
        return false;
    }
    else{
        textBrowser->append(QString("<font color=%1><b>Accès %2 %3 validé</b></font> <font color=darkGrey>(https://%4)</font>")
                            .arg(nom == "Google" ? "darkGreen" : "darkBlue")
                            .arg(nom == "Google" ? "internet" : "serveur")
                            .arg(nom == "Google" ? "" : nom)
                            .arg(adresse));
        return true;
    }
}

/** Enregistrement d'une page web  pour récupérer son contenu
    =========================================================*/
void Reseau::Enregistrer_page_web(const QUrl & url)
{
    QNetworkAccessManager *manager = new QNetworkAccessManager(this);
    manager->setProxy(*proxy);

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

/** Constructeur
    ============ */
Proxy::Proxy(QString const& nom_fichier)
{
    /* Ouverture et lecture du fichier contenant la liste des proxies préenregistrés */
    FichierCsv *fichier = new FichierCsv(nom_fichier);
    fichier->Lire();

    for (int i(1); i < fichier->matrix.size(); ++i){
        for (int j(1); j < fichier->matrix[i].size(); ++j)
            proxies[fichier->matrix[i][0]].append(fichier->matrix[i][j]);
    }

    delete fichier;
}
