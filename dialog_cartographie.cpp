#include "dialog_cartographie.h"
#include "ui_dialog_cartographie.h"

DialogCarto::DialogCarto(const StationHydro * s,  double h, QWidget *parent) :
    QDialog(parent), ui(new Ui::DialogCarto), station_hydro(s), hauteur_crue(h)
{
    ui->setupUi(this);

    qDebug() << station_hydro->Identifiant().second << hauteur_crue;
}

DialogCarto::~DialogCarto()
{
    delete ui;
}
