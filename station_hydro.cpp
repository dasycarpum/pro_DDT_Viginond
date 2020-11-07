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
            sh->niveaux_crue.append(n.toDouble());

        sh->QRef = fichier->matrix[i][14].toDouble();

        if (sh->Nature() == "Prévision" || sh->Nature() == "Vigilance")
            sh->Historique_crue_donnees();

        stations_hydro.append(sh);
    }

    return stations_hydro;
}

/** Convertit les hauteurs horaires en tableau de points xy, et permettre de tracer la courbe du graphe
    =================================================================================================== */
QVector<QPointF> StationHydro::Hauteurs_horaires_courbe(void)
{
    QVector<QPointF> xy;

    for (QMap<QDateTime, double>::const_iterator it = hauteurs_horaires.cbegin(); it != hauteurs_horaires.cend(); ++it){
        QPointF point (it.key().toTime_t(), it.value());
        xy.push_back(point);
    }

    return xy;
}

/** Renvoie la hauteur d'eau estimée 4 heures après le dernier relevé
    ================================================================= */
QPair<double, QDateTime> StationHydro::Projection_niveau_4h(void) const
{
    /* Relevés sur les dernières 48h */
    std::vector<double> x; // QDateTime en msec
    std::vector<double> y; // hauteur d'eau en mm

    QDateTime releve_moins_1j = hauteurs_horaires.lastKey().addDays(-2);
    for(QMap<QDateTime, double>::const_iterator it = hauteurs_horaires.cbegin(); it != hauteurs_horaires.cend(); ++it)
        if (it.key() > releve_moins_1j){
            x.push_back(it.key().toTime_t());
            y.push_back(it.value());
        }

    /* Degrés de liberté */
    const int order(3);

    /* Régression polynomiale et calcul des coefficients de l'équation en retour :
     * y = a0 + a1 * x + a2 * x^2 + ... + an * x^n - voir outil.h */
    std::vector<double>coeffs = polyFit(x, y, order);

    /* Application des coefficients à l'équation pour projeter une hauteur d'eau 4 heures après le dernier relevé */
    double temps_plus_4h(hauteurs_horaires.lastKey().addSecs(3600 * 4).toTime_t());
    double projection_hauteur(0.0);
    for (uint i(0); i < coeffs.size(); ++i)
        projection_hauteur += coeffs.at(i) * pow(temps_plus_4h, i);

    /* Retour */
    QPair<double, QDateTime> projection(projection_hauteur, hauteurs_horaires.lastKey().addSecs(3600 * 4));

    return projection;
}

/** Date, hauteur et vigilance des crues historiques de la station hydrométrique (source RIC)
    ========================================================================================= */
void StationHydro::Historique_crue_donnees(void)
{
    /* Ouverture et lecture du fichier contenant la liste des historiques de crues à la station hydrométrique */
    FichierCsv *fichier = new FichierCsv("/databank/hydrometeorologie/historique_crue/" + identifiant.first);
    fichier->Lire();

    /* Lecture des valeurs du tableau */
    for (int i(1); i < fichier->matrix.size(); ++i)
            historique_crue[fichier->matrix[i][1].toDouble()] = qMakePair( QDate::fromString(fichier->matrix[i][0], "dd/MM/yyyy"),fichier->matrix[i][2]);
}

/** Renvoie le prochain seuil historique, immédiatement supérieur à la hauteur horaire actuelle
    ============================================================================================ */
QPair<double, QDate> StationHydro::Seuil_historique(void) const
{
    QPair<double, QDate> seuil(0.0, QDate(1900, 01, 01));

    for (QMap<double, QPair<QDate, QString>>::const_iterator it = historique_crue.cbegin(); it != historique_crue.cend(); ++it)
        if (it.key() > hauteurs_horaires.last()){
            seuil.first = it.key();
            seuil.second = it.value().first;
            return seuil;
        }

    return seuil;
}

/** Renvoie le niveau de crue modélisé pour la hauteur actuelle
    =========================================================== */
double StationHydro::Niveau_crue_actuel(void) const
{
    if (niveaux_crue.first() == 0.0)
        return -1.0;    // niveau non déterminé
    else if ( hauteurs_horaires.last() < niveaux_crue.first())
        return 0.0;     // pas de crue

    for (int i(0); i < niveaux_crue.size(); ++i)
        if (hauteurs_horaires.last() >= niveaux_crue[i])
            return niveaux_crue[i];

    return -1.0;
}

/** Renvoie le prochain niveau de crue modélisé en cas de hausse de la hauteur d'eau
    ================================================================================= */
double StationHydro::Niveau_crue_a_venir(void) const
{
    if (niveaux_crue.first() == 0.0)
        return -1.0;    // niveau non déterminé
    else if ( hauteurs_horaires.last() < niveaux_crue.first())
        return niveaux_crue.first();

    for (int i(0); i < niveaux_crue.size()-1; ++i)
        if (hauteurs_horaires.last() >= niveaux_crue[i])
            return niveaux_crue[i+1];

    return -1.0;
}

/** Renvoie la couleur de la vigilance en fonction de la hauteur d'eau (en mm) passée en argument
    ============================================================================================== */
QString StationHydro::Vigilance(double const& hauteur) const
{
    QString couleur("Vert");

    if (nature.compare("Observation", Qt::CaseInsensitive) == 0)
        couleur = "Gris";
    else if (!historique_crue.isEmpty()){
        QMap<double, QPair<QDate, QString>>::const_iterator it = historique_crue.lowerBound(hauteur);
        if (hauteur >= historique_crue.firstKey())
            couleur = it.value().second;
    }

    return couleur;
}
