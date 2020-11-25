#ifndef FENETREPRINCIPALE_H
#define FENETREPRINCIPALE_H

#pragma GCC diagnostic ignored "-Wfloat-equal" // évite le "warning: comparing floating point with == or != is unsafe"
                                               // à l'instruction "stations_par_cours_d_eau.at(i)->Niveau_crue_actuel() == -1.0 ?"
#include <QMainWindow>
#include <QButtonGroup>
#include <QRadioButton>
#include <QRandomGenerator>
#include <QTableWidget>
#include <QHeaderView>
#include <QComboBox>
#include <QDesktopServices>

#include "reseau.h"
#include "fichier.h"
#include "station_hydro.h"
#include "graphique.h"
#include "dialog_cartographie.h"
#include "dialog_proxy.h"

QT_BEGIN_NAMESPACE
namespace Ui { class FenetrePrincipale; }
QT_END_NAMESPACE

class FenetrePrincipale : public QMainWindow
{
    Q_OBJECT
    Ui::FenetrePrincipale *ui;

    bool acces_internet;                    // accès au réseau internet disponible (avec ou sans proxy)
    QList<StationHydro *> stations_hydro;   // liste de toutes les stations hydrométriques
    QButtonGroup *grp_radioButton_bassin;   // tous les bassins versants (sous forme de QRadioButton) - dans 1 QGroupBox
    QButtonGroup *grp_pushButton_station;   // toutes les stations d'un cours d'eau (sous forme de QPushButton) - dans 1 QTabWidget

public:
    FenetrePrincipale(QWidget *parent = nullptr);
    ~FenetrePrincipale();

    QString Selection_proxy(void);
    void Affichage_radioButton_bassin(void);
    void Affichage_graphique(QString const&, QString const&, QList<StationHydro *> const&);
    void Affichage_tableau(QString const&, QList<StationHydro *> const&);
    void Affichage_menu_sites_web(void);
    void Affichage_menu_crues_historiques(void);

public slots:
    void Affichage_dialog_proxy(void);
    void Telechargement_Vigicrues(void);
    void Selection_bassin_versant(QAbstractButton *);
    void Affichage_fenetres_annexes(QAbstractButton *);

    void Menu_arretes_prefectoraux(void);
    void Menu_sites_web(QAction * action);
    void Menu_crues_historiques(QAction * action);
    void Menu_aide(void);

};
#endif // FENETREPRINCIPALE_H
