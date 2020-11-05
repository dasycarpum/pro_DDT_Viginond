#ifndef FENETREPRINCIPALE_H
#define FENETREPRINCIPALE_H

#include <QMainWindow>
#include <QButtonGroup>
#include <QRadioButton>
#include <QRandomGenerator>
#include <QTableWidget>
#include <QHeaderView>
#include <QComboBox>

#include "reseau.h"
#include "fichier.h"
#include "station_hydro.h"
#include "graphique.h"

QT_BEGIN_NAMESPACE
namespace Ui { class FenetrePrincipale; }
QT_END_NAMESPACE

class FenetrePrincipale : public QMainWindow
{
    Q_OBJECT
    Ui::FenetrePrincipale *ui;

    QList<StationHydro *> stations_hydro;
    QButtonGroup *grp_radioButton_bassin;

public:
    FenetrePrincipale(QWidget *parent = nullptr);
    ~FenetrePrincipale();

    void Affichage_radioButton_bassin(void);
    void Affichage_graphique(QString const&, QString const&, QList<StationHydro *> const&);
    void Affichage_tableau(QString const&, QList<StationHydro *> const&);

public slots:
    void Telechargement_Vigicrues(void);
    void Selection_bassin_versant(QAbstractButton *);

};
#endif // FENETREPRINCIPALE_H
