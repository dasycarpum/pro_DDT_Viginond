#include "fenetre_principale.h"
#include "ui_fenetre_principale.h"

FenetrePrincipale::FenetrePrincipale(QWidget *parent)
    : QMainWindow(parent)
    , ui(new Ui::FenetrePrincipale)
{
    ui->setupUi(this);

    /* Style */
    ui->groupBox_proxy->setObjectName("box");
    ui->groupBox_proxy->setStyleSheet("QGroupBox#box { font-weight:bold }");
    ui->groupBox_requete->setObjectName("box");
    ui->groupBox_requete->setStyleSheet("QGroupBox#box { font-weight:bold }");
    ui->groupBox_visualisation->setObjectName("box");
    ui->groupBox_visualisation->setStyleSheet("QGroupBox#box { font-weight:bold }");
    ui->groupBox_bassin_versant->setObjectName("box");
    ui->groupBox_bassin_versant->setStyleSheet("QGroupBox#box { font-weight:bold }");

    QPalette palette; palette.setColor(QPalette::Button, QColor(133, 163, 195));
    ui->pushButton_telechargement->setPalette(palette);

    QScreen *screen = QGuiApplication::primaryScreen();
    QRect  screenGeometry = screen->geometry();
    hauteur_ecran = screenGeometry.height();

    /* Test d'un accès au réseau internet */
    acces_internet = false;
    Reseau *reseau = new Reseau();
    if (reseau->Test_connexion(ui->textBrowser_internet))
        acces_internet = true;
    delete reseau;

    /* Paramétrage d'un proxy par l'utilisateur */
    connect(ui->radioButton_proxy_autre, SIGNAL(clicked()), this, SLOT(Affichage_dialog_proxy()));

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

    /* Menus */
    connect(ui->action_Meuse_Moselle, SIGNAL(triggered()), this, SLOT(Menu_arretes_prefectoraux()));
    connect(ui->action_Rhin_Sarre, SIGNAL(triggered()), this, SLOT(Menu_arretes_prefectoraux()));
    Affichage_menu_sites_web();
    connect(ui->menu_sites_web, SIGNAL(triggered(QAction *)), this, SLOT(Menu_sites_web(QAction *)));
    Affichage_menu_crues_historiques();
    connect(ui->menu_crues_historiques, SIGNAL(triggered(QAction *)), this, SLOT(Menu_crues_historiques(QAction *)));
    connect(ui->action_manuel_utilisation, SIGNAL(triggered()), this, SLOT(Menu_aide()));
    connect(ui->action_QGiS_3_4_5, SIGNAL(triggered()), this, SLOT(Menu_aide()));
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

/** Personnalisation du proxy par une interface de dialogue
    ======================================================= */
void FenetrePrincipale::Affichage_dialog_proxy(void)
{
    /* Instanciation de la boîte de dialogue */
    DialogProxy *dialog_proxy = new DialogProxy();
    dialog_proxy->show();
}

/** Enregistrement des données Vigicrues
    ==================================== */
void FenetrePrincipale::Telechargement_Vigicrues(void)
{
    if (acces_internet){
        /* Configuration du proxy pour connexion à un réseau protégé */
        QString site(Selection_proxy());
        QString emplacement;
        site == "Configuration" ? emplacement = "C:/temp/proxy" : emplacement = "/databank/reseau/proxy";

        Proxy * proxy = new Proxy(emplacement);
        if (site != "Aucun"){
            proxy->setType(QNetworkProxy::ProxyType(proxy->Proxies()[site][0].toInt()));
            proxy->setHostName(proxy->Proxies()[site][1]);
            proxy->setPort(quint16(proxy->Proxies()[site][2].toUInt()));
            proxy->setUser(proxy->Proxies()[site][3]);
            proxy->setPassword(proxy->Proxies()[site][4]);
        }

        /* Boucle sur les stations hydro */
        for (int i(0); i < stations_hydro.size(); ++i){

            /* Téléchargement des données (historiques des hauteurs d'eau par station) sur le site Vigicrues, au format XML */
            Reseau *reseau = new Reseau(proxy);
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
                                                            .arg(stations_hydro[i]->Hauteurs_horaires().lastKey().toString("dd MMM à hh:mm"))
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
    else{
        QMessageBox::StandardButton reply;
        reply = QMessageBox::warning(this, "Hors connexion", "Voulez-vous poursuivre en mode dégradé ?",
                                      QMessageBox::Yes|QMessageBox::No);
        if (reply == QMessageBox::Yes)
            Affichage_radioButton_bassin();
        else
            this->close();
    }
}

/** Identification du site de connexion et de son proxy
    =================================================== */
QString FenetrePrincipale::Selection_proxy(void)
{
    foreach(QAbstractButton *rb, ui->buttonGroup_proxy->buttons())
        if (rb->isChecked())
            return rb->text();

    return "NA";
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
        if (acces_internet)
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
    if (hauteur_ecran <= 800)
        graph_entite_hydro->setMaximumHeight(340);

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

/* Colorisation des cellules du tableau des paramètres de crue selon la hauteur d'eau observée et la couleur de vigilance associée
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
/* Mise en forme du menu déroulant dans le tableau des paramètres de crue
   ----------------------------------------------------------------------*/
void ComboBox(QComboBox * comboBox, const StationHydro * station, int i)
{
    comboBox->setLayoutDirection(Qt::RightToLeft);
    comboBox->addItems(station->Liste_niveaux_crue());
    comboBox->setCursor(Qt::PointingHandCursor);
    comboBox->setWhatsThis(QString::number(i));
    comboBox->setMaximumHeight(20);
}

/* Mise en forme du bouton dans le tableau des paramètres de crue
   --------------------------------------------------------------*/
void PushButton(QPushButton * pushButton, const StationHydro * station, int i)
{
    pushButton->setMaximumHeight(18);
    pushButton->setToolTip(station->Identifiant().first); // code de la station hydro
    pushButton->setWhatsThis(QString::number(i)); // colonne du tableWidget
    pushButton->setCursor(Qt::PointingHandCursor);
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

        if (!stations_par_cours_d_eau.at(i)->Hauteurs_horaires().empty() && acces_internet){
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
                    ComboBox(comboBox, stations_par_cours_d_eau.at(i), i);
                    tableau->setCellWidget(j, i, comboBox);
                    break;
                    }
                case 6:{ // pushButton pour que l'utilisateur puisse ouvrir les fenêtres 'Enjeux' et 'Cartographie' de la station, sur base du niveau de crue
                    QPushButton *pushButton = new QPushButton("OK");
                    PushButton(pushButton, stations_par_cours_d_eau.at(i), i);
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
        else if (!acces_internet){
            for (int j(5); j < titre_ligne.size(); ++j){
                switch(j){
                case 5:{ // liste des niveaux de crue, affichée dans un menu déroulant
                    QComboBox *comboBox = new QComboBox();
                    ComboBox(comboBox, stations_par_cours_d_eau.at(i), i);
                    tableau->setCellWidget(j, i, comboBox);
                    break;
                    }
                case 6:{ // pushButton pour que l'utilisateur puisse ouvrir les fenêtres 'Enjeux' et 'Cartographie' de la station, sur base du niveau de crue
                    QPushButton *pushButton = new QPushButton("OK");
                    PushButton(pushButton, stations_par_cours_d_eau.at(i), i);
                    grp_pushButton_station->addButton(pushButton);
                    tableau->setCellWidget(j, i, pushButton);
                    break;
                    }
                default:
                    break;
                }
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

void FenetrePrincipale::Menu_arretes_prefectoraux(void)
{
    QAction * act = qobject_cast<QAction *>(sender());
    QString arrete = act->text().replace(' ', '_');

    QUrl url("file:///databank/hydrometeorologie/arretes_prefectoraux/" + arrete + ".pdf");
    QUrl urlFichier = QUrl::fromLocalFile(QCoreApplication::applicationDirPath() + url.toLocalFile());
    QDesktopServices::openUrl(urlFichier);
}

void FenetrePrincipale::Affichage_menu_sites_web(void)
{
    /* Ouverture et lecture du fichier contenant la liste des sites Web*/
    FichierCsv *fichier = new FichierCsv("/databank/reseau/sites_web");
    fichier->Lire();

    /* Insertion des menus-actions 'sites Web' */
    QList<QAction *> sites;
    for (int i(1); i < fichier->matrix.size(); ++i){
            QAction * action = new QAction(fichier->matrix[i][0]); // nom générique du site
            action->setWhatsThis(fichier->matrix[i][1]);           // adresse internet du site
            QIcon icon(QCoreApplication::applicationDirPath() + "/databank/pictogramme/" + fichier->matrix[i][2]);
            action->setIcon(icon);
            sites.append(action);
        }
    ui->menu_sites_web->addActions(sites);
    delete fichier;
}

void FenetrePrincipale::Menu_sites_web(QAction * action)
{
    QDesktopServices::openUrl(QUrl(action->whatsThis()));
}


void FenetrePrincipale::Affichage_menu_crues_historiques(void)
{
    /* Liste des cours d'eau avec historiques de crues par bassin versant */
    QMap<QString, QSet<QString>> cours_d_eau_par_bv;

    for (int i(0); i < stations_hydro.size(); ++i)
        if (!stations_hydro[i]->Historique_crue().isEmpty())
            cours_d_eau_par_bv[stations_hydro[i]->Bassin_versant()].insert(stations_hydro[i]->Entite_hydrologique().second);

    /* Insertion des menus 'bassins versants' et de leurs actions 'cours d'eau' */
    for (QMap<QString, QSet<QString>>::const_iterator it_m = cours_d_eau_par_bv.cbegin(); it_m != cours_d_eau_par_bv.cend(); ++it_m){
        QList<QAction *> cours_d_eau;
        for(QSet<QString>::const_iterator it_s = it_m.value().cbegin(); it_s != it_m.value().cend(); ++it_s){
            QAction * action = new QAction(*it_s);  // nom du cours d'eau
            action->setWhatsThis(it_m.key());       // nom du bassin versant
            QIcon icon(QCoreApplication::applicationDirPath() + "/databank/pictogramme/" + it_m.key() + ".png");
            action->setIcon(icon);
            cours_d_eau.append(action);
        }
        QMenu *menu = new QMenu();
        menu->setTitle(it_m.key());
        QIcon icon(QCoreApplication::applicationDirPath() + "/databank/pictogramme/bv_"+ it_m.key() +".png");
        menu->setIcon(icon);
        menu->addActions(cours_d_eau);
        ui->menu_crues_historiques->addMenu(menu);
    }
}

void FenetrePrincipale::Menu_crues_historiques(QAction * action)
{
    QString textMessage("");
    static QMap<QString, QString> couleur; couleur.insert("Vert","lightGreen"); couleur.insert("Jaune","yellow");
                                           couleur.insert("Orange","orange"); couleur.insert("Rouge","red");

    for (int i(0); i < stations_hydro.size(); ++i)
        if (stations_hydro[i]->Bassin_versant() == action->whatsThis() &&
                stations_hydro[i]->Entite_hydrologique().second == action->text() &&
                !stations_hydro[i]->Historique_crue().isEmpty()){
            textMessage.append(QString("Cours d'eau : <b>%1</b> - Station <b>%2</b><br><br>")
                                            .arg(stations_hydro[i]->Entite_hydrologique().second)
                                            .arg(stations_hydro[i]->Identifiant().second));
            for (QMap<double, QPair<QDate, QString>>::const_iterator it = stations_hydro[i]->Historique_crue().cbegin(); it != stations_hydro[i]->Historique_crue().cend(); ++it)
                textMessage.append(QString("<span style='background-color:%1'><b>%2</b> - Hauteur d'eau : <b>%L3 m</b></span><br>")
                                                   .arg(couleur[it.value().second])
                                                   .arg(it.value().first.toString("MMMM yyyy"))
                                                   .arg(it.key() /1000, 0, 'f', 2));
            textMessage.append("<br>");
        }

    /* Affichage du texte dans la MessagBox */
    QMessageBox msgBox;
    msgBox.setIcon(QMessageBox::Information);
    msgBox.setWindowTitle(ui->menu_crues_historiques->title());
    msgBox.setText(textMessage);
    msgBox.exec();
}

void FenetrePrincipale::Menu_aide(void)
{
    QAction * action = qobject_cast<QAction *>(sender());

    if (action->text().contains("manuel", Qt::CaseInsensitive)){
        QUrl url("file:///databank/documentation/manuel_utilisation.pdf");
        QUrl urlFichier = QUrl::fromLocalFile(QCoreApplication::applicationDirPath() + url.toLocalFile());
        QDesktopServices::openUrl(urlFichier);
    }
    else
        QDesktopServices::openUrl(QUrl(action->whatsThis()));

}
