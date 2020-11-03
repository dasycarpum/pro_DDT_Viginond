#ifndef STATIONHYDRO_H
#define STATIONHYDRO_H

#include <QPair>
#include <QString>
#include <tuple>
#include <QList>
#include <QDateTime>

#include "fichier.h"

class StationHydro
{
    QPair<QString, QString> identifiant;            // code, dénomination
    QList<double> coord_geo;                        // x, y, z
    QString troncon;                                // code
    QPair<QString, QString> entite_hydrologique;    // code, denomination
    int sens_affluent;                              // sur 1 bassin versant, position de l'amont vers l'aval de l'affluent (0 = confluent, 1 = 1er affluent, ...)
    QString bassin_versant;                         // dénomination
    QString bv_couleur;                             // couleur attribuée au bassin versant
    int amont_aval;                                 // sur 1 masse d'eau, position de l'amont vers l'aval (1 = station la + en amont)
    QString nature;                                 // "Prévision", "Observation", "Vigilance"
    QList<int> niveaux_crue;                        // en mm
    double QRef;                                    // cote de référence de la crue centennale
    QMap<QDateTime, double> hauteurs_horaires;      // série temporelle des hauteurs d'eau, téléchargée sur Vigicrues par l'utilisateur

public:
    StationHydro();
    ~StationHydro() {}

    /* Accesseurs */
    QPair<QString, QString> Identifiant(void) const {return identifiant;}
    QList<double> Coord_geo(void) const {return coord_geo;}
    QString Troncon(void) const {return troncon;}
    QPair<QString, QString> Entite_hydrologique(void) const {return entite_hydrologique;}
    int Sens_affluent(void) const {return sens_affluent;}
    QString Bassin_versant(void) const {return bassin_versant;}
    QString Bv_couleur(void) const {return bv_couleur;}
    int Amont_aval(void) const {return amont_aval ;}
    QString Nature(void) const {return nature;}
    QList<int> Niveaux_crue(void) const {return niveaux_crue;}
    double QREF(void) const {return QRef;}
    QMap<QDateTime, double> Hauteurs_horaires(void) const {return hauteurs_horaires;}

    /* Mutateur */
    void Hauteurs_horaires(QMap<QDateTime, double> hh) {hauteurs_horaires = hh;}

    /* Méthodes */
    static QList<StationHydro *> Liste_stations_hydro(void);
    QVector<QPointF> Hauteurs_horaires_courbe(void);
};

#endif // STATIONHYDRO_H
