#ifndef DIALOG_CARTOGRAPHIE_H
#define DIALOG_CARTOGRAPHIE_H

#include <QDialog>
#include <QProcess>
#include <QDir>
#include <QTreeWidgetItem>

#include "station_hydro.h"
#include "reseau.h"

namespace Ui {
class DialogCarto;
}

struct Serveur_wms
{
    const QString denomination;
    const QString adresse;
    bool acces;

    Serveur_wms () = default ;
    Serveur_wms (QString const& d, QString const& a) : denomination(d), adresse(a)  {acces = false;}
    ~Serveur_wms () {}

};

inline bool operator<(const Serveur_wms &sw1, const Serveur_wms &sw2)
{
    if (sw1.denomination != sw2.denomination)
        return sw1.denomination < sw2.denomination;

    return sw1.adresse < sw2.adresse;
}

struct Couche_wms
{
    QString denomination;
    QString adresse;
    QString format_image;
    bool choix_utilisateur;

    Couche_wms () {denomination = adresse = format_image = "NA"; choix_utilisateur = false;}
    ~Couche_wms () {}
};


class DialogCarto : public QDialog
{
    Q_OBJECT
    Ui::DialogCarto *ui;

    const StationHydro * station_hydro;                     // Station hydrométrique retenue par l'utilisateur
    const double hauteur_crue;                              // modèle de hauteur de crue choisie par l'utilisateur
    bool acces_internet;

    QMap<Serveur_wms, QList<Couche_wms>> infos_Wms;         // liste des couches WMS disponibles par serveurs
    QList<QPair<QString, bool>> infos_sig_enjeux;           // liste des couches SIG de type vecteur (enjeux)
    QList<QPair<QString, bool>> infos_sig_zip;              // liste des couches SIG de type vecteur (ZIP-ZICH)

public:
    explicit DialogCarto(const StationHydro *, const double, QWidget *parent = nullptr);
    ~DialogCarto();

    void Affichage_infos_WMS(void);
    void Choix_referentiel_WMS(void);
    void Affichage_infos_SIG(void);
    void Choix_couche_SIG(void);
    void Customisation_projet_QGiS(void);

public slots:
    void Affichage_QGiS(void);
    void Gestion_check(QTreeWidgetItem *,int);

};

#endif // DIALOG_CARTOGRAPHIE_H
