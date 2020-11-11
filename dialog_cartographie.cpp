#include "dialog_cartographie.h"
#include "ui_dialog_cartographie.h"

DialogCarto::DialogCarto(const StationHydro * s,  double h, QWidget *parent) :
    QDialog(parent), ui(new Ui::DialogCarto), station_hydro(s), hauteur_crue(h)
{
    ui->setupUi(this);

    /* Test d'un accès au réseau internet et aux serveurs cartographiques */
   Affichage_infos_WMS();

    /* Gestion des boutons de la boîte de dialogue */
    connect(ui->buttonBox, SIGNAL(accepted()), this, SLOT(Affichage_QGiS()));
    connect(ui->buttonBox, SIGNAL(rejected()), this, SLOT(reject()));
}

DialogCarto::~DialogCarto()
{
    delete ui;
}

void DialogCarto::Affichage_infos_WMS(void)
{
    /* Collecte des informations relatives aux serveurs et couches WMS */
    FichierCsv *fichier = new FichierCsv("/databank/cartographie/referentiel/WMS");
    fichier->Lire();

    for(int i(1); i < fichier->matrix.size(); ++i ){
        Couche_wms couche;
        Serveur_wms serveur(fichier->matrix[i][0], fichier->matrix[i][1]);
        couche.denomination = fichier->matrix[i][2];
        couche.adresse = fichier->matrix[i][3];
        couche.format_image = fichier->matrix[i][4];

        infos_Wms[serveur].push_back(couche);
    }

    qDebug() << infos_Wms.size();
    for (QMap<Serveur_wms, QList<Couche_wms>>::const_iterator it = infos_Wms.cbegin(); it != infos_Wms.cend(); ++it){
        qDebug() << it.key().denomination << it.value().size();
    }
}

void Qgis_projet_custom(const StationHydro * station)
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
    texte << "iface.messageBar().pushMessage(\"DDT 57\", \"Bienvenue sur le projet d'analyse cartographique de crue !\")" << endl;
    /* Ouverture du projet */
    texte << "qgs = QgsApplication([], False)" << endl;
    texte << "qgs.initQgis()" << endl;
    texte << "project = QgsProject.instance()" << endl;
    texte << "project.read(\"" + QCoreApplication::applicationDirPath() + "/databank/cartographie/projet_qgis.qgz\")" << endl;
    /* Vue carto localisée sur le secteur géographique de la station hydro */
    texte << "qgis.utils.iface.mapCanvas().setExtent(QgsRectangle(" << station->Emprise_communale().second[0] << ","
                                                                    << station->Emprise_communale().second[1] << ","
                                                                    << station->Emprise_communale().second[2] << ","
                                                                    << station->Emprise_communale().second[3] << "))" << endl;
    /* Projection = Lambert 93 Bornes Europe = EPSG 2154 */
    texte << "lambert93 = QgsCoordinateReferenceSystem(2154, QgsCoordinateReferenceSystem.PostgisCrsId)\n"
             "qgis.utils.iface.mapCanvas().setDestinationCrs(lambert93)" << endl;
    /* Référentiel WMS */
    texte << "uri = 'crs=EPSG:2154&featureCount=10&format=image/png&layers=default&styles=&url=https://osm.geograndest.fr/mapcache/'" << endl;
    texte << "qgis.utils.iface.addRasterLayer(uri, 'OSM GéoGrandEst', 'wms')" << endl;
    /* Finalisation */
    texte << "qgis.utils.iface.mapCanvas().refresh()";

    fichier.close();
}

void DialogCarto::Affichage_QGiS(void)
{
    /* Customisation du projet QGiS */
    Qgis_projet_custom(station_hydro);

    /* Exécution de QGiS */
    QProcess *process = new QProcess(this);
    QString chemin = QDir::toNativeSeparators(QCoreApplication::applicationDirPath())+ "//QGiS_3.4.5//usbgis//apps//qgis//bin//";
    process->setWorkingDirectory(chemin);
    process->startDetached(chemin + "qgis-ltr.bat", QStringList());
    process->waitForFinished();
}
