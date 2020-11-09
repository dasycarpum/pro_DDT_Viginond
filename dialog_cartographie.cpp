#include "dialog_cartographie.h"
#include "ui_dialog_cartographie.h"

DialogCarto::DialogCarto(const StationHydro * s,  double h, QWidget *parent) :
    QDialog(parent), ui(new Ui::DialogCarto), station_hydro(s), hauteur_crue(h)
{
    ui->setupUi(this);

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
