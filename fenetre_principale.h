#ifndef FENETREPRINCIPALE_H
#define FENETREPRINCIPALE_H

#include <QMainWindow>
#include <QButtonGroup>
#include <QRadioButton>
#include <qwt_plot.h>
#include <qwt_plot_curve.h>
#include <qwt_legend.h>
#include <qwt_plot_grid.h>
#include <qwt_scale_draw.h>

#include "reseau.h"
#include "fichier.h"
#include "station_hydro.h"

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

public slots:
    void Telechargement_Vigicrues(void);
    void Selection_bassin_versant(QAbstractButton *);

};
#endif // FENETREPRINCIPALE_H
