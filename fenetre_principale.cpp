#include "fenetre_principale.h"
#include "ui_fenetre_principale.h"

FenetrePrincipale::FenetrePrincipale(QWidget *parent)
    : QMainWindow(parent)
    , ui(new Ui::FenetrePrincipale)
{
    ui->setupUi(this);

    /* Test d'un accès au réseau internet */
    Reseau *reseau = new Reseau();
    if (reseau->Test_connexion())
        ui->textBrowser_internet->append("<font color=darkGreen><b>Accès internet validé</b></font> <font color=darkGrey>(https://www.google.com/)</font>");
    else
        ui->textBrowser_internet->append("<font color=darkRed><b>Pas de connexion internet !</b></font>");
    delete reseau;

    /* Enregistrement des données Vigicrues */
    Telechargement_Vigicrues();
}

FenetrePrincipale::~FenetrePrincipale()
{
    delete ui;
}

/** Enregistrement des données Vigicrues
    ==================================== */
void FenetrePrincipale::Telechargement_Vigicrues()
{
    /* Téléchargement des données (historiques des hauteurs d'eau par station) sur le site Vigicrues, au format XML */
    Reseau *reseau = new Reseau();
    reseau->Enregistrer_page_web(QUrl("https://www.vigicrues.gouv.fr/services/observations.xml/?CdStationHydro=A743061001" ));

    /* Conversion du tableau d'octets (QByteArray) en un tableau 2D indicé 'temps-hauteur' */
    QList<QString> liste_noeud_xml; liste_noeud_xml << "DtObsHydro" << "ResObsHydro";
    QVector<QVector<QString>> hauteurs_horaires_str = FichierXml::Lecture_valeur(reseau->Donnee_page_web(), liste_noeud_xml);

    delete reseau;

    /* Typage des chaînes de caractères (QString) du tableau 2D : conversion des 2 champs en 'QDateTime' et 'double' */
    QMap<QDateTime, double> hauteurs_horaires;

    for (int i(0); i < hauteurs_horaires_str.size(); ++i){
        QDateTime dt;
        double h(0.0);

        for (int j(0); j < hauteurs_horaires_str[i].size(); ++j)
            if (j == 0)
                dt = QDateTime(QDateTime::fromString(hauteurs_horaires_str[i][j], Qt::ISODate));
            else if (j == 1)
                h = hauteurs_horaires_str[i][j].toDouble();

        hauteurs_horaires[dt] = h;
    }

    /* Affichage du résultat (dernier relevé de hauteur) */
    if (!hauteurs_horaires.isEmpty())
        ui->textBrowser_vigicrues->append(QString("<TABLE BORDER WIDTH=220 align=center CELLSPACING=1>"
                                                  "<TR><TD WIDTH=120 BGCOLOR=#%1 align=left>%2</TD>"
                                                  "<TD WIDTH=60 align=center>%3</TD>"
                                                  "<TD WIDTH=50 align=center>%L4 mm</TD></TR></TABLE>")
                                                    .arg("08CA79")
                                                    .arg("Station Metz")
                                                    .arg(hauteurs_horaires.lastKey().toString(Qt::ISODate))
                                                    .arg(hauteurs_horaires.last()));
    else
        ui->textBrowser_vigicrues->append(QString("<TABLE BORDER WIDTH=220 align=center CELLSPACING=1>"
                                                  "<TR><TD WIDTH=120 align=left><font color=darkRed>%1</font></TD>"
                                                  "<TD WIDTH=110 align=center><font color=darkRed>Donnée absente !</font></TD></TR></TABLE>")
                                                    .arg("Station ?"));



}
