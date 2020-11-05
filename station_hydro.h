#ifndef STATIONHYDRO_H
#define STATIONHYDRO_H

#include <QPair>
#include <QString>
#include <tuple>
#include <QList>
#include <QDateTime>

#include "fichier.h"
#include "regression_polynomiale.h"

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
    QList<double> niveaux_crue;                     // liste des cotes présentant un risque d'inondation (en mm)
    double QRef;                                    // cote de référence de la crue centennale
    QMap<QDateTime, double> hauteurs_horaires;      // série temporelle des hauteurs d'eau, téléchargée sur Vigicrues par l'utilisateur
    QMap<double, QPair<QDate, QString>> historique_crue; // Hauteurs d'eau associées aux principales crues connues et leurs niveaux de vigilance

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
    QList<double> Niveaux_crue(void) const {return niveaux_crue;}
    double QREF(void) const {return QRef;}
    QMap<QDateTime, double> Hauteurs_horaires(void) const {return hauteurs_horaires;}
    QMap<double, QPair<QDate, QString>> Historique_crue(void) const {return historique_crue;}

    /* Mutateur */
    void Hauteurs_horaires(QMap<QDateTime, double> hh) {hauteurs_horaires = hh;}

    /* Méthodes */
    static QList<StationHydro *> Liste_stations_hydro(void);
    QVector<QPointF> Hauteurs_horaires_courbe(void);
    QPair<double, QDateTime> Projection_niveau_4h(void) const;
    void Historique_crue_donnees(void);
    QPair<double, QDate> Seuil_historique(void) const;
    double Niveau_crue_actuel(void) const;

};

#endif // STATIONHYDRO_H
