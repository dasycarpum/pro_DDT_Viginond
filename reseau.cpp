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
