#include "dialog_proxy.h"
#include "ui_dialog_proxy.h"

DialogProxy::DialogProxy(QWidget *parent) :
    QDialog(parent),
    ui(new Ui::DialogProxy)
{
    ui->setupUi(this);

    /* Gestion des boutons de la boîte de dialogue */
    connect(ui->buttonBox, SIGNAL(accepted()), this, SLOT(Enregistrement_parametres()));
    connect(ui->buttonBox, SIGNAL(rejected()), this, SLOT(reject()));
}

DialogProxy::~DialogProxy()
{
    delete ui;
}

/** Enregistrement des paramètres du proxy dans le fichier proxy.csv
    ================================================================ */
void DialogProxy::Enregistrement_parametres(void)
{
    /* Récupération des paramètres depuis la console */
    QString champs("Site client;Protocole;Adresse IP;Port;Utilisateur;Mot de passe");
    QStringList parametres;
    parametres << "Configuration" << ui->comboBox_protocol->currentText().at(1) << ui->lineEdit_host_name->text() << ui->lineEdit_port->text()
               << ui->lineEdit_user->text() << ui->lineEdit_password->text();

    /* Vérification de l'existence d'un répertoire temporaire */
    QDir repertoire("C:\\temp");
    if (!repertoire.exists())
        repertoire.mkdir("C:\\temp");

    /* Enregistrement des paramètres dans un fichier csv */
    QString chemin = "C:/temp/proxy.csv";

    QFile fichier(chemin);
    if (!fichier.open(QIODevice::WriteOnly))
        QMessageBox::critical(nullptr, "Oups !", "Impossible d'ouvrir le fichier "  + chemin);

    QTextStream texte(&fichier);
    texte << champs << endl;
    texte << parametres.join(';') << endl ;

    fichier.close();
}
