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

    /* Constitution de la BDD des stations hydrométriques et de leurs caractéristiques */
    stations_hydro = StationHydro::Liste_stations_hydro();

    /* Enregistrement des données Vigicrues */
    connect(ui->pushButton_telechargement, SIGNAL(clicked()), this, SLOT(Telechargement_Vigicrues()));

    /* Connexion des radioButton 'bassin versant' pour la visualisation des données */
    grp_radioButton_bassin = new QButtonGroup(ui->groupBox_bassin_versant);
    connect(grp_radioButton_bassin, SIGNAL(buttonClicked(QAbstractButton*)), this, SLOT(Selection_bassin_versant(QAbstractButton *)));

    /* Ouverture des fenêtres 'Enjeux' et 'Cartographie' sur base de la station et du niveau de crue choisis par l'utilisateur  */
    grp_pushButton_station = new QButtonGroup();
    connect(grp_pushButton_station, SIGNAL(buttonClicked(QAbstractButton*)), this, SLOT(Affichage_fenetres_annexes(QAbstractButton *)));

}

FenetrePrincipale::~FenetrePrincipale()
{
    delete ui;
}

QMap<QDateTime, double> Typage_releve(QVector<QVector<QString>> hh_str)
{
    QMap<QDateTime, double> hauteurs_horaires;

    for (int i(0); i < hh_str.size(); ++i){
        QDateTime dt;
        double h(0.0);

        for (int j(0); j < hh_str[i].size(); ++j)
            if (j == 0)
                dt = QDateTime(QDateTime::fromString(hh_str[i][j], Qt::ISODate));
            else if (j == 1)
                h = hh_str[i][j].toDouble();

        hauteurs_horaires[dt] = h;
    }

    return  hauteurs_horaires;
}

/** Enregistrement des données Vigicrues
    ==================================== */
void FenetrePrincipale::Telechargement_Vigicrues(void)
{
    for (int i(0); i < stations_hydro.size(); ++i){

        /* Téléchargement des données (historiques des hauteurs d'eau par station) sur le site Vigicrues, au format XML */
        Reseau *reseau = new Reseau();
        reseau->Enregistrer_page_web(QUrl("https://www.vigicrues.gouv.fr/services/observations.xml/?CdStationHydro=" + stations_hydro[i]->Identifiant().first));

        /* Conversion du tableau d'octets (QByteArray) en un tableau 2D indicé 'temps-hauteur' */
        QList<QString> liste_noeud_xml; liste_noeud_xml << "DtObsHydro" << "ResObsHydro";
        QVector<QVector<QString>> hauteurs_horaires_str = FichierXml::Lecture_valeur(reseau->Donnee_page_web(), liste_noeud_xml);

        delete reseau;

        /* Typage des chaînes de caractères (QString) du tableau 2D : conversion des 2 champs en 'QDateTime' et 'double' */
        stations_hydro[i]->Hauteurs_horaires(Typage_releve(hauteurs_horaires_str));

        /* Affichage du résultat (dernier relevé de hauteur) */
        if (!stations_hydro[i]->Hauteurs_horaires().isEmpty())
            ui->textBrowser_vigicrues->append(QString("<TABLE BORDER WIDTH=220 align=center CELLSPACING=1>"
                                                      "<TR><TD WIDTH=120 BGCOLOR=#%1 align=left>%2</TD>"
                                                      "<TD WIDTH=60 align=center>%3</TD>"
                                                      "<TD WIDTH=50 align=center>%L4 mm</TD></TR></TABLE>")
                                                        .arg(stations_hydro[i]->Bv_couleur())
                                                        .arg(stations_hydro[i]->Identifiant().second)
                                                        .arg(stations_hydro[i]->Hauteurs_horaires().lastKey().toString(Qt::ISODate))
                                                        .arg(stations_hydro[i]->Hauteurs_horaires().last()));
        else
            ui->textBrowser_vigicrues->append(QString("<TABLE BORDER WIDTH=220 align=center CELLSPACING=1>"
                                                      "<TR><TD WIDTH=120 align=left><font color=darkRed>%1</font></TD>"
                                                      "<TD WIDTH=110 align=center><font color=darkRed>Donnée absente !</font></TD></TR></TABLE>")
                                                        .arg(stations_hydro[i]->Identifiant().second));
    }
    /* Instanciation des radioButtons des bassins versants du territoire */
    Affichage_radioButton_bassin();
}

/** Instanciation des radioButtons 'bassin versant'
    =============================================== */
void FenetrePrincipale::Affichage_radioButton_bassin(void)
{
    int k(0), maxColonne(2);

    for (int i(0); i < stations_hydro.size(); ++i){
        if (i == 0 || stations_hydro[i]->Bassin_versant() != stations_hydro[i-1]->Bassin_versant()){
            QRadioButton *radioButton = new QRadioButton(stations_hydro[i]->Bassin_versant());
            radioButton->setIcon(QIcon(QCoreApplication::applicationDirPath() + "/databank/pictogramme/" + radioButton->text() + ".png"));
            radioButton->setCursor(Qt::PointingHandCursor);

            grp_radioButton_bassin->addButton(radioButton);

            int ligne = k / maxColonne;
            int colonne = k % maxColonne;
            ui->gridLayout_bassin_versant->addWidget(radioButton, ligne, colonne);
            ++k;
        }
    }

    /* Empêcher un 2e téléchargement */
    ui->pushButton_telechargement->setDisabled(true);
}

/** Sélection d'un bassin versant, pour visualisation sous format graphique et tableau des crues
    ============================================================================================ */
void FenetrePrincipale::Selection_bassin_versant(QAbstractButton * rb)
{
    /* Nettoyage préalable des 2 tabWidgets */
    ui->tabWidget_graphique->clear();
    ui->tabWidget_tableau->clear();

    /* Sélection des cours d'eau appartenant au même bassin versant */
    QMap<int, QString>cours_d_eau;
    for (int i(0); i < stations_hydro.size(); ++i)
        if (stations_hydro[i]->Bassin_versant() == rb->text())
            cours_d_eau[stations_hydro[i]->Sens_affluent()] = stations_hydro[i]->Entite_hydrologique().second;

    for (QMap<int, QString>::const_iterator it = cours_d_eau.cbegin(); it != cours_d_eau.cend(); ++it){

        /* Sélection des stations appartenant au même cours d'eau */
        QList<StationHydro *> stations_par_cours_d_eau;
        for (int i(0); i < stations_hydro.size(); ++i)
            if (stations_hydro[i]->Bassin_versant() == rb->text()
                    && stations_hydro[i]->Entite_hydrologique().second == it.value())
                stations_par_cours_d_eau.append(stations_hydro[i]);

        /* Affichage */
        Affichage_graphique(rb->text(), it.value(), stations_par_cours_d_eau);
        Affichage_tableau(it.value(), stations_par_cours_d_eau);
    }

    /* Synchronisation du tableau et graphique visualisant les données d'un cours d'eau */
    connect(ui->tabWidget_graphique, SIGNAL(currentChanged(int)), ui->tabWidget_tableau, SLOT(setCurrentIndex(int)));
    connect(ui->tabWidget_tableau, SIGNAL(currentChanged(int)), ui->tabWidget_graphique, SLOT(setCurrentIndex(int)));
}

void FenetrePrincipale::Affichage_graphique(QString const& bassin_versant, QString const& cours_d_eau, QList<StationHydro *> const& stations_par_cours_d_eau)
{
    /* Graphique : présentation */
    QwtPlot *graph_entite_hydro = new QwtPlot(this);
    graph_entite_hydro->setTitle("Bassin versant " + bassin_versant + " - cours d'eau " +  cours_d_eau);
    graph_entite_hydro->setAxisTitle(QwtPlot::yLeft, "Hauteur eau (mm)");
    graph_entite_hydro->setCanvasBackground(QBrush(QColor("#e5e6e2")));

    /* Gestion du zoom et de l'étiquette mobile de données */
    Zoom* zoom = new Zoom( graph_entite_hydro->canvas() );
    zoom->setMousePattern( QwtEventPattern::MouseSelect2,
        Qt::RightButton, Qt::ControlModifier );
    zoom->setMousePattern( QwtEventPattern::MouseSelect3,
        Qt::RightButton );

    /* Format de l'échelle */
    EchelleHoraire * x = new EchelleHoraire();
    x->setLabelRotation(-45);
    graph_entite_hydro->setAxisScaleDraw(QwtPlot::xBottom, x);

    /* Grille */
    QwtPlotGrid *grille = new QwtPlotGrid();
    grille->setPen(QPen(Qt::darkGray, 0 , Qt::DotLine));
    grille->attach( graph_entite_hydro );

    /* Courbes */
    for (int i(0); i < stations_par_cours_d_eau.size(); ++i){
        QwtPlotCurve *curve_station = new QwtPlotCurve( stations_par_cours_d_eau[i]->Identifiant().second );
        curve_station->setSamples( stations_par_cours_d_eau[i]->Hauteurs_horaires_courbe() );
        curve_station->setPen(QColor(QColor::fromRgb(QRandomGenerator::global()->generate())), 2);
        curve_station->setLegendAttribute( QwtPlotCurve::LegendShowLine );
        curve_station->attach( graph_entite_hydro );
    }

    /* Légende */
    QwtLegend *legend = new QwtLegend();
    graph_entite_hydro->insertLegend(legend, QwtPlot::RightLegend);

    /* Insertion du graphique */
    ui->tabWidget_graphique->addTab(graph_entite_hydro, cours_d_eau);
    graph_entite_hydro->replot();
}

/* Colorisation des cellules du tableau des résultats selon la hauteur d'eau observée et la couleur de vigilance associée
   ---------------------------------------------------------------------------------------------------------------------- */
QColor Couleur(QString couleur)
{
    QColor color(QColor(255,255,255));

    if (couleur.compare("Vert", Qt::CaseInsensitive) == 0)
        color.setRgb(186,255,117);
    else if (couleur.compare("Jaune", Qt::CaseInsensitive) == 0)
        color.setRgb(255,255,117);
    else if (couleur.compare("Orange", Qt::CaseInsensitive) == 0)
        color.setRgb(255,186,117);
    else if (couleur.compare("Rouge", Qt::CaseInsensitive) == 0)
        color.setRgb(255,117,117);
    else if (couleur.compare("Gris", Qt::CaseInsensitive) == 0)
        color.setRgb(230,230,230);

    return color;
}

/** Tableau présentant les indicateurs de crue par station (1 onglet = 1 cours d'eau)
    ================================================================================= */
void FenetrePrincipale::Affichage_tableau(QString const& cours_d_eau, QList<StationHydro *> const& stations_par_cours_d_eau)
{
    QStringList titre_ligne;
    titre_ligne << "Hauteur actuelle" << "Projection +4 heures" << "Prochain seuil historique" << "Niveau de crue actuel" << "Niveau de crue à venir" << "Choix du niveau" << "Enjeux et Cartographie";
    QStringList titre_colonne;

    QTableWidget *tableau = new QTableWidget(titre_ligne.count(), stations_par_cours_d_eau.size());

    for (int i(0); i < stations_par_cours_d_eau.size(); ++i){

        titre_colonne.push_back( stations_par_cours_d_eau[i]->Identifiant().second );

        if (!stations_par_cours_d_eau.at(i)->Hauteurs_horaires().empty())
            for (int j(0); j < titre_ligne.size(); ++j){

                QTableWidgetItem *cellule = new QTableWidgetItem();
                QString textCellule("NA");
                QString colorCellule("NA");

                switch (j){
                case 0:{ // dernier relevé horaire et hauteur actuelle
                    textCellule = QString::number(stations_par_cours_d_eau.at(i)->Hauteurs_horaires().last() / 1000) + " m à " + stations_par_cours_d_eau.at(i)->Hauteurs_horaires().lastKey().toString("hh:mm");
                    colorCellule = stations_par_cours_d_eau.at(i)->Vigilance(stations_par_cours_d_eau.at(i)->Hauteurs_horaires().last());
                    break;
                    }
                case 1:{ //projection hauteur d'eau à +4h
                    QPair<double, QDateTime> projection = stations_par_cours_d_eau.at(i)->Projection_niveau_4h();
                    textCellule = QString::number(projection.first / 1000, 'f', 2) + " m à " + projection.second.toString("hh:mm");
                    colorCellule = stations_par_cours_d_eau.at(i)->Vigilance(projection.first);
                    break;
                    }
                case 2:{ // prochain seuil historique (rappel de la hauteur atteinte et de la date)
                    textCellule = stations_par_cours_d_eau.at(i)->Seuil_historique().first == 0.0 ?
                                "" : QString::number(stations_par_cours_d_eau.at(i)->Seuil_historique().first / 1000) + " m le " + stations_par_cours_d_eau.at(i)->Seuil_historique().second.toString("d/M/yyyy") ;
                    colorCellule = stations_par_cours_d_eau.at(i)->Vigilance(stations_par_cours_d_eau.at(i)->Seuil_historique().first);
                    break;
                    }
                case 3:{ // valeur du niveau de crue modélisé pour la hauteur actuelle
                    textCellule = stations_par_cours_d_eau.at(i)->Niveau_crue_actuel() == -1.0 ?
                                    "Non déterminé" : stations_par_cours_d_eau.at(i)->Niveau_crue_actuel() == 0.0 ?
                                        "Pas de crue" : QString::number(stations_par_cours_d_eau.at(i)->Niveau_crue_actuel() / 1000) + " m";
                    colorCellule = stations_par_cours_d_eau.at(i)->Vigilance(stations_par_cours_d_eau.at(i)->Niveau_crue_actuel());
                    break;
                    }
                case 4:{ // valeur du 1er (ou suivant) niveau de crue modélisé
                    textCellule = stations_par_cours_d_eau.at(i)->Niveau_crue_a_venir() == -1.0 ?
                                    "Non déterminé" : QString::number(stations_par_cours_d_eau.at(i)->Niveau_crue_a_venir() / 1000) + " m";
                    colorCellule = stations_par_cours_d_eau.at(i)->Vigilance(stations_par_cours_d_eau.at(i)->Niveau_crue_a_venir());
                    break;
                    }
                case 5:{ // liste des niveaux de crue, affichée dans un menu déroulant
                    QComboBox *comboBox = new QComboBox();
                    comboBox->setLayoutDirection(Qt::RightToLeft);
                    comboBox->addItems(stations_par_cours_d_eau.at(i)->Liste_niveaux_crue());
                    comboBox->setCursor(Qt::PointingHandCursor);
                    comboBox->setWhatsThis(QString::number(i));
                    comboBox->setMaximumHeight(20);
                    tableau->setCellWidget(j, i, comboBox);
                    break;
                    }
                case 6:{ // pushButton pour que l'utilisateur puisse ouvrir les fenêtres 'Enjeux' et 'Cartographie' de la station, sur base du niveau de crue
                    QPushButton *pushButton = new QPushButton("OK");
                    pushButton->setMaximumHeight(18);
                    pushButton->setToolTip(stations_par_cours_d_eau.at(i)->Identifiant().first); // code de la station hydro
                    pushButton->setWhatsThis(QString::number(i)); // colonne du tableWidget
                    pushButton->setCursor(Qt::PointingHandCursor);
                    grp_pushButton_station->addButton(pushButton);
                    tableau->setCellWidget(j, i, pushButton);
                    break;
                    }
                default:
                    break;
                }

                if (j < 5){
                    cellule->setText(textCellule);
                    cellule->setBackgroundColor(Couleur(colorCellule));
                    cellule->setTextAlignment(Qt::AlignCenter);
                    tableau->setItem(j, i, cellule);
                }

            }
    }

    tableau->setVerticalHeaderLabels(titre_ligne);
    tableau->setHorizontalHeaderLabels(titre_colonne);
    tableau->verticalHeader()->setSectionResizeMode(QHeaderView::Stretch);
    tableau->horizontalHeader()->setSectionResizeMode(QHeaderView::Stretch);
    tableau->resizeColumnsToContents();

    ui->tabWidget_tableau->addTab(tableau, cours_d_eau);
}

/** Instanciation de la fenêtre d'analyse cartographique, pour le secteur de la station retenue par l'utilisateur
    ============================================================================================================= */
void FenetrePrincipale::Affichage_fenetres_annexes(QAbstractButton * button)
{
    /* Station hydrométrique retenue par l'utilisateur par le biais du tableau d'indicateurs */
    StationHydro * station_choisie = new StationHydro();

    for (int i(0); i < stations_hydro.size(); ++i){
        if (stations_hydro[i]->Identifiant().first == button->toolTip())
            station_choisie = stations_hydro[i];
    }

    /* Hauteur de crue retenue par l'utilisateur dans la QComboBox du tableau (ligne 5) */
    QTableWidget *tableau = qobject_cast<QTableWidget *>(ui->tabWidget_tableau->currentWidget());
    QComboBox *comboBox = qobject_cast<QComboBox *>(tableau->cellWidget(5, button->whatsThis().toInt()));
    const double hauteur_crue(comboBox->currentText().split(' ').first().toDouble() * 1000);

    /* Codes INSEE des communes aux alentours de la station (+- 5 km) ET coordonnées XY du rectangle d'emprise */
    station_choisie->Evaluation_emprise();

    /* Instanciation de la boîte de dialogue Cartographie */
    DialogCarto *dialog_carto = new DialogCarto(station_choisie, hauteur_crue);
    dialog_carto->show();
}
