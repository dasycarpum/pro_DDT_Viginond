#include "dialog_cartographie.h"
#include "ui_dialog_cartographie.h"

DialogCarto::DialogCarto(const StationHydro * s,  double h, QWidget *parent) :
    QDialog(parent), ui(new Ui::DialogCarto), station_hydro(s), hauteur_crue(h)
{
    ui->setupUi(this);

    /* Test d'un accès au réseau internet et aux serveurs cartographiques */
   Affichage_infos_WMS();
   /* Liste des couches SIG disponibles pour l'analyse de crue  (selon station et hauteur de crue) */
   Affichage_infos_SIG();

    /* Gestion des boutons de la boîte de dialogue */
    connect(ui->buttonBox, SIGNAL(accepted()), this, SLOT(Affichage_QGiS()));
    connect(ui->buttonBox, SIGNAL(rejected()), this, SLOT(reject()));
}

DialogCarto::~DialogCarto()
{
    delete ui;
}

/** Affichage des données WMS disponibles : vérification de l'accès internet aux serveurs
 *  (dans QTextBrowser) et couches répertoriées pour chacun d'eux (dans QTreeWidget)
    ===================================================================================== */
void DialogCarto::Affichage_infos_WMS(void)
{
    /* Collecte des informations relatives aux serveurs et couches WMS */
    FichierCsv *fichier = new FichierCsv("/databank/cartographie/referentiel/WMS");
    fichier->Lire();

    for(int i(1); i < fichier->matrix.size(); ++i ){
        Serveur_wms serveur(fichier->matrix[i][0], fichier->matrix[i][1]);
        Couche_wms couche;
        couche.denomination = fichier->matrix[i][2];
        couche.adresse = fichier->matrix[i][3];
        couche.format_image = fichier->matrix[i][4];

        infos_Wms[serveur].push_back(couche);
    }

    /* Affichage des tests d'accès à internet et aux serveurs (QTextBrowser) */
    Reseau *reseau = new Reseau();
    reseau->Test_connexion(ui->textBrowser_information);    // test de connexion à internet (Google)

    QMap<Serveur_wms, QList<Couche_wms>> infos_Wms_modif;
    for (QMap<Serveur_wms, QList<Couche_wms>>::const_iterator it = infos_Wms.cbegin(); it != infos_Wms.cend(); ++it)
        if (reseau->Test_connexion(ui->textBrowser_information, it.key().denomination, it.key().adresse.split('/').first())){
            Serveur_wms serveur(it.key().denomination, it.key().adresse);
            serveur.acces = true;                           // validation de l'accès internet pour ce serveur
            infos_Wms_modif[serveur] = it.value();
        }
    infos_Wms.swap(infos_Wms_modif);
    delete reseau;

    /* Affichage des serveurs accessibles, avec leurs couches respectives (QTreeWidget) */
    QTreeWidgetItem *item_0 = nullptr;
    QTreeWidgetItem *item_1 = nullptr;

    for (QMap<Serveur_wms, QList<Couche_wms>>::const_iterator it = infos_Wms.cbegin(); it != infos_Wms.cend(); ++it){

            if (it.key().acces){                                // nom des serveurs accessibles
                item_0 = new QTreeWidgetItem();
                item_0->setCheckState(0, Qt::Unchecked);
                item_0->setText(0, it.key().denomination);
            }
            ui->treeWidget_fond_carto->addTopLevelItem(item_0);

            for (int i(0); i < it.value().size(); ++i){         // nom des couches (pour chaque serveur)
                item_1 = new QTreeWidgetItem();
                item_1->setCheckState(0, Qt::Unchecked);
                item_1->setText(0, it.value()[i].denomination);
                item_0->addChild(item_1);
            }
    }
    ui->treeWidget_fond_carto->expandAll();

    connect(ui->treeWidget_fond_carto, SIGNAL(itemChanged(QTreeWidgetItem *, int) ), this, SLOT(Gestion_check(QTreeWidgetItem*,int)));
}

/** Validation des couches WMS retenues par l'utilisateur
    ===================================================== */
void DialogCarto::Choix_referentiel_WMS(void)
{
    QTreeWidgetItemIterator it_tree(ui->treeWidget_fond_carto);
    while (*it_tree){
        if ((*it_tree)->checkState(0) == 2 && (*it_tree)->childCount() == 0)  // = 0 : unchecked, = 1 : partially, = 2 : checked  && thématique sans enfant (donc finale)
            for (QMap<Serveur_wms, QList<Couche_wms>>::iterator it_map = infos_Wms.begin(); it_map != infos_Wms.end(); ++it_map)
                for (int i(0); i < it_map.value().size(); ++i)
                    if (it_map.value()[i].denomination == (*it_tree)->text(0))
                       it_map.value()[i].choix_utilisateur = true;
        ++it_tree;
    }
}

/** Affichage des couches SIG vecteur caractérisant la crue (enjeux et ZIP-ZICH) dans 2 QTreeWidget
    =============================================================================================== */
void DialogCarto::Affichage_infos_SIG(void)
{
    /* Collecte des informations relatives aux couches SIG disponibles */
    QStringList sig_enjeux = FichierCsv::Liste_fichiers("/databank/cartographie/bassin_versant/" + station_hydro->Bassin_versant(), "" , "TAB");
    QStringList sig_zip = FichierCsv::Liste_fichiers("/databank/cartographie/bassin_versant/" + station_hydro->Bassin_versant() + "/ZIP/" + station_hydro->Identifiant().first + "/", QString::number(hauteur_crue), "SHP");

    foreach (QString const& couche, sig_enjeux){
        QPair<QString, bool> info;
        info.first = couche;
        info.second = false;
        infos_sig_enjeux.append(info);
    }

    foreach (QString const& couche, sig_zip){
        QPair<QString, bool> info;
        info.first = couche;
        info.second = false;
        infos_sig_zip.append(info);
    }

    /* Affichage des couches disponibles (2 QTreeWidget) */
    QTreeWidgetItem *item_0 = nullptr;
    QTreeWidgetItem *item_1 = nullptr;

    item_0 = new QTreeWidgetItem();
    item_0->setCheckState(0, Qt::Checked);
    item_0->setText(0, "Enjeux");

    ui->treeWidget_carto_enjeu->addTopLevelItem(item_0);

    foreach (QString const& couche, sig_enjeux){
        item_1 = new QTreeWidgetItem();
        item_1->setCheckState(0, Qt::Checked);
        item_1->setText(0, couche.split('/').last());
        item_0->addChild(item_1);
    }
    ui->treeWidget_carto_enjeu->expandAll();

    item_0 = new QTreeWidgetItem();
    item_0->setCheckState(0, Qt::Checked);
    item_0->setText(0, sig_zip.isEmpty() ? "Pas de ZIP-ZICH" : "ZIP-ZICH");

    ui->treeWidget_carto_zip->addTopLevelItem(item_0);

    foreach (QString const& couche, sig_zip){
        item_1 = new QTreeWidgetItem();
        item_1->setCheckState(0, Qt::Checked);
        item_1->setText(0, couche.split('/').last());
        item_0->addChild(item_1);
    }
    ui->treeWidget_carto_zip->expandAll();

    connect(ui->treeWidget_carto_enjeu, SIGNAL(itemChanged(QTreeWidgetItem *, int) ), this, SLOT(Gestion_check(QTreeWidgetItem*,int)));
    connect(ui->treeWidget_carto_zip, SIGNAL(itemChanged(QTreeWidgetItem *, int) ), this, SLOT(Gestion_check(QTreeWidgetItem*,int)));
}

/** Validation des couches SIG vecteur retenues par l'utilisateur
    ============================================================= */
void DialogCarto::Choix_couche_SIG(void)
{
    QTreeWidgetItemIterator it_tree_1(ui->treeWidget_carto_enjeu);
    while (*it_tree_1){
        if ((*it_tree_1)->checkState(0) == 2 && (*it_tree_1)->childCount() == 0)  // = 0 : unchecked, = 1 : partially, = 2 : checked  && thématique sans enfant (donc finale)
            for (int i(0); i < infos_sig_enjeux.size(); ++i)
                if (infos_sig_enjeux[i].first.contains((*it_tree_1)->text(0)))
                    infos_sig_enjeux[i].second = true;
        ++it_tree_1;
    }

    QTreeWidgetItemIterator it_tree_2(ui->treeWidget_carto_zip);
    while (*it_tree_2){
        if ((*it_tree_2)->checkState(0) == 2 && (*it_tree_2)->childCount() == 0)  // = 0 : unchecked, = 1 : partially, = 2 : checked  && thématique sans enfant (donc finale)
            for (int i(0); i < infos_sig_zip.size(); ++i)
                if (infos_sig_zip[i].first.contains((*it_tree_2)->text(0)))
                    infos_sig_zip[i].second = true;
        ++it_tree_2;
    }
}

/** Personnalisation du projet QGiS en fonction des choix utilisateurs
 * (localisation station, référentiels WMS, hauteur de crue, ...)
    ==================================================================*/
void DialogCarto::Customisation_projet_QGiS(void)
{
    QDir repertoire("C:\\temp");
    if (!repertoire.exists())
        repertoire.mkdir("C:\\temp");

    QString chemin = "C:/temp/qgis_projet_custom.py";

    QFile fichier(chemin);
    if (!fichier.open(QIODevice::WriteOnly))
        qDebug() << chemin;

    QTextStream texte(&fichier);

    /* Bibliothèques */
    texte << "from qgis.utils import iface" << endl;
    texte << "from qgis.core import *" << endl;

    /* Message d'accueil */
    texte << "iface.messageBar().pushMessage(\"DDT 57\", \"Bienvenue sur le projet d'analyse cartographique de crue...\")" << endl;
    /* Ouverture du projet */
    texte << "qgs = QgsApplication([], False)" << endl;
    texte << "qgs.initQgis()" << endl;
    texte << "project = QgsProject.instance()" << endl;
    texte << "project.read(\"" + QCoreApplication::applicationDirPath() + "/databank/cartographie/projet_qgis.qgz\")" << endl;
    /* Vue carto localisée sur le secteur géographique de la station hydro */
    texte << "qgis.utils.iface.mapCanvas().setExtent(QgsRectangle(" << station_hydro->Emprise_communale().second[0] << ","
                                                                    << station_hydro->Emprise_communale().second[1] << ","
                                                                    << station_hydro->Emprise_communale().second[2] << ","
                                                                    << station_hydro->Emprise_communale().second[3] << "))" << endl;
    /* Projection = Lambert 93 Bornes Europe = EPSG 2154 */
    texte << "lambert93 = QgsCoordinateReferenceSystem(2154, QgsCoordinateReferenceSystem.PostgisCrsId)\n"
             "qgis.utils.iface.mapCanvas().setDestinationCrs(lambert93)" << endl;
    /* Référentiels WMS */
    Choix_referentiel_WMS();
    for (QMap<Serveur_wms, QList<Couche_wms>>::const_iterator it = infos_Wms.cbegin(); it != infos_Wms.cend(); ++it)
        for (int i(0); i < it.value().size(); ++i)
            if (it.value()[i].choix_utilisateur){
                texte << QString("uri = 'crs=EPSG:2154&featureCount=10&format=%1&layers=%2&styles=&url=https://%3'")
                         .arg(it.value()[i].format_image)
                         .arg(it.value()[i].adresse)
                         .arg(it.key().adresse) << endl;
                texte << QString("qgis.utils.iface.addRasterLayer(uri, '%1', 'wms')")
                         .arg(it.value()[i].denomination).toUtf8() << endl;
            }
    /* Couches SIG de type vecteur (enjeux et ZIP-ZICH) */
    Choix_couche_SIG();
    QList<QList<QPair<QString, bool>>> infos_sig; infos_sig << infos_sig_enjeux << infos_sig_zip;
    for (int k(0); k < infos_sig.size(); ++k)
        for (int i(0); i < infos_sig[k].size(); ++i)
            if (infos_sig[k][i].second){
                texte << QString("couche%1 = iface.addVectorLayer(\"%2\", \"%3\",\"ogr\")")
                        .arg(QString::number(i))
                        .arg(infos_sig[k][i].first)
                        .arg("") << endl;
                texte << QString("if not couche%1 : print (\"Echec au chargement de la couche %2 !\")")
                         .arg(QString::number(i))
                         .arg(infos_sig[k][i].first) << endl;
            }
    /* Finalisation */
    texte << "qgis.utils.iface.mapCanvas().refresh()";

    fichier.close();
}

/** Exécution de QGiS pour l'analyse cartographique de la crue
    ========================================================== */
void DialogCarto::Affichage_QGiS(void)
{
    /* Customisation du projet QGiS */
    Customisation_projet_QGiS();

    /* Exécution de QGiS */
    QProcess *process = new QProcess(this);
    QString chemin = QDir::toNativeSeparators(QCoreApplication::applicationDirPath())+ "//QGiS_3.4.5//usbgis//apps//qgis//bin//";
    process->setWorkingDirectory(chemin);
    process->startDetached(chemin + "qgis-ltr.bat", QStringList());
    process->waitForFinished();
}

/** Gère les checks des cases à cocher dans le QTreeWidget (entreQTreeWidgetItem parent et enfants)
    =============================================================================================== */
void DialogCarto::Gestion_check(QTreeWidgetItem * item, int )
{
    if (item->checkState(0) == 2)
        for (int i(0); i < item->childCount(); ++i)
          item->child(i)->setCheckState(0, Qt::Checked);

    else if (item->checkState(0) == 0)
        for (int i(0); i < item->childCount(); ++i)
          item->child(i)->setCheckState(0, Qt::Unchecked);

    if (item->parent())
    {
        bool allChecked=true;
        bool allUncheked=true;
        for (int i=0; i < item->parent()->childCount(); i++)
        {
            if (item->parent()->child(i)->checkState(0) != Qt::Checked)
                allChecked = false;
            if (item->parent()->child(i)->checkState(0) != Qt::Unchecked)
                allUncheked = false;
        }
        if (allChecked)
            item->parent()->setCheckState(0, Qt::Checked);
        else if(allUncheked)
            item->parent()->setCheckState(0, Qt::Unchecked);
        else
            item->parent()->setCheckState(0, Qt::PartiallyChecked);
    }
}
