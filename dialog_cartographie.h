#ifndef DIALOG_CARTOGRAPHIE_H
#define DIALOG_CARTOGRAPHIE_H

#include <QDialog>
#include <QProcess>
#include <QDir>

#include "station_hydro.h"
#include "reseau.h"

namespace Ui {
class DialogCarto;
}

class DialogCarto : public QDialog
{
    Q_OBJECT
    Ui::DialogCarto *ui;

    const StationHydro * station_hydro;
    const double hauteur_crue;


public:
    explicit DialogCarto(const StationHydro *, const double, QWidget *parent = nullptr);
    ~DialogCarto();

public slots:
    void Affichage_QGiS(void);

};

#endif // DIALOG_CARTOGRAPHIE_H
