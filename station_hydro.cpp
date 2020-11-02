#include "station_hydro.h"

StationHydro::StationHydro()
{
    troncon = bassin_versant = nature = "NA"; sens_affluent = amont_aval = 0; QRef = 0.0;
}

QList<StationHydro *> StationHydro::Liste_stations_hydro(void)
{
    QList<StationHydro *> stations_hydro;

    FichierCsv *fichier = new FichierCsv("/databank/hydrometeorologie/station_hydrometrique");
    fichier->Lire();

    for (int i(1); i < fichier->matrix.size(); ++i){
        StationHydro * sh = new StationHydro();

        QPair<QString, QString> id;
        id.first = fichier->matrix[i][0];
        id.second = fichier->matrix[i][1];
        sh->identifiant = id;

        QList<double>cg;
        cg.append(fichier->matrix[i][2].toDouble());
        cg.append(fichier->matrix[i][3].toDouble());
        cg.append(fichier->matrix[i][4].toDouble());
        sh->coord_geo = cg;

        sh->troncon = fichier->matrix[i][5];

        QPair<QString, QString> eh;
        eh.first = fichier->matrix[i][6];
        eh.second = fichier->matrix[i][7];
        sh->entite_hydrologique = eh;

        sh->sens_affluent = fichier->matrix[i][8].toInt();

        sh->bassin_versant = fichier->matrix[i][9];
        sh->bv_couleur = fichier->matrix[i][10];

        sh->amont_aval = fichier->matrix[i][11].toInt();

        sh->nature = fichier->matrix[i][12];

        QStringList nc = fichier->matrix[i][13].split('_');
        foreach (QString n, nc)
            sh->niveaux_crue.append(n.toInt());

        sh->QRef = fichier->matrix[i][14].toDouble();

        stations_hydro.append(sh);
    }

    return stations_hydro;
}
