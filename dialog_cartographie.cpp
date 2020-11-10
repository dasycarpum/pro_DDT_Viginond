#include "dialog_cartographie.h"
#include "ui_dialog_cartographie.h"

DialogCarto::DialogCarto(const StationHydro * s,  double h, QWidget *parent) :
    QDialog(parent), ui(new Ui::DialogCarto), station_hydro(s), hauteur_crue(h)
{
    ui->setupUi(this);

    /* Test d'un accès au réseau internet et aux serveurs cartographiques */
    Reseau *reseau = new Reseau();
    if (reseau->Test_connexion())
        ui->textBrowser_information->append("<font color=darkGreen><b>Accès internet validé</b></font> <font color=darkGrey>(www.google.com)</font>");
    else
        ui->textBrowser_information->append("<font color=darkRed><b>Pas de connexion internet !</b></font>");

    QString serveur_carto("osm.geograndest.fr");
    if (reseau->Test_connexion(serveur_carto))
        ui->textBrowser_information->append(QString("<font color=darkGreen><b>Accès serveur validé</b></font> <font color=darkGrey>(%1)</font>")
                                            .arg(serveur_carto));
    else
        ui->textBrowser_information->append("<font color=darkRed><b>Pas de connexion serveur !</b></font>");

    delete reseau;

    /* Gestion des boutons de la boîte de dialogue */
    connect(ui->buttonBox, SIGNAL(accepted()), this, SLOT(Affichage_QGiS()));
    connect(ui->buttonBox, SIGNAL(rejected()), this, SLOT(reject()));
}

DialogCarto::~DialogCarto()
{
    delete ui;
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
