#ifndef FENETREPRINCIPALE_H
#define FENETREPRINCIPALE_H

#include <QMainWindow>

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

public:
    FenetrePrincipale(QWidget *parent = nullptr);
    ~FenetrePrincipale();

public slots:
    void Telechargement_Vigicrues(void);

};
#endif // FENETREPRINCIPALE_H
