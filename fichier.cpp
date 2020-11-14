#include "fichier.h"

/** Lecture du fichier csv et stockage des données dans une matrice
    =============================================================== */
void FichierCsv::Lire(void)
{
    if (!open(QIODevice::ReadOnly | QIODevice::Text))
        QMessageBox::critical(nullptr, "Oups !", "Impossible d'ouvrir le fichier " + fileName());

    int l(0); // numéro de ligne dans le fichier
    while (!atEnd()) {
        QString enregistrement = readLine();    // lecture du fichier ligne par ligne
        enregistrement.remove(QRegExp("\\n"));  // suppression du caractère de fin de ligne
        matrix.resize(l+1);                     // redimensionnement du tableau d'une ligne supplémentaire

        QStringList champs = enregistrement.split(separateur);  // scission de la ligne en une table de champs
        matrix[l].resize(champs.size());        // redimensionnement du tableau avec le nombre de colonnes

        int c(0); // numéro de colonne dans l'enregistrement
        foreach (QString const& attribut, champs){ // complétude du tableau
            matrix[l][c] = attribut;
            ++c;
        }
        ++l;
    }
}

/** Retourne la liste des sous-dossiers d'un répertoire
    =================================================== */
QStringList FichierCsv::Liste_sous_repertoires(const QString & repertoire)
{
    QStringList liste;

    QDir dir;
    if (repertoire.mid(1,1) != ":")
        dir.setPath(QCoreApplication::applicationDirPath() + repertoire);
    else
        dir.setPath(repertoire);

    QFileInfoList listRepertoire = dir.entryInfoList(QDir::Dirs);

    for (int i(0); i < listRepertoire.size(); ++i){
        QFileInfo fileInfos = listRepertoire.at(i);

        if (fileInfos.baseName().size() > 0)
            liste.append(fileInfos.baseName());
    }

    return liste;
}

/** Retourne la liste des fichiers et leurs chemins contenus dans un répertoire,
 *  en précisant en option un sous-répertoire cible et l'extension des fichiers recherchés
    ======================================================================================= */
QStringList FichierCsv::Liste_fichiers(const QString & repertoire, const QString & sous_repertoire, const QString & extension)
{
    QStringList liste_fichiers;

    QDirIterator it(QCoreApplication::applicationDirPath() + repertoire, QStringList() << "*." + extension, QDir::Files, QDirIterator::Subdirectories);
    while (it.hasNext())
        if(it.next().contains(sous_repertoire))
            liste_fichiers.append(it.fileInfo().filePath());

    return liste_fichiers;
}

//____________________________________________________________________________________________________________________________________________

/* Conversion d'une liste séquentielle  en tableau 2D
   -------------------------------------------------- */
QVector<QVector<QString>> Conversion_liste_en_tableau2D(QList<QString> const& liste_valeur, QList<QString> const& liste_noeud)
{
    QVector<QVector<QString>> tableau_valeur;
    QVector<QString>enregistrement(liste_noeud.size()); // séquence

    for(int i(0); i< liste_valeur.size(); ++i){
        int modulo(i%liste_noeud.size());

        for (int j(0); j < enregistrement.size(); ++j)
            if (modulo == j)
                enregistrement[j] = liste_valeur[i];

        if (modulo == liste_noeud.size()-1)
            tableau_valeur.push_back(enregistrement);
    }
    return tableau_valeur;
}


/** Dans une page XML, extraction des valeurs à l'intérieur des noeuds
    ================================================================== */

QVector<QVector<QString>> FichierXml::Lecture_valeur(const QByteArray & donnee_page_xml, QList<QString> const& liste_noeud)
{
    /* Lecture séquentielle des valeurs contenues dans les noeuds identifiés */
    QList<QString> liste_valeur;

    QXmlStreamReader xml(donnee_page_xml);

    while (!xml.atEnd()){
        xml.readNext();

        if (xml.isStartElement()){
            for(QString noeud : liste_noeud)
                if (xml.name() == noeud)
                    liste_valeur.push_back(xml.readElementText());
        }
    }

    if (xml.hasError())
        QMessageBox::critical(nullptr, "Oups !", "Impossible de lire le fichier XML");

    /* Conversion de la liste en tableau 2D */
    QVector<QVector<QString>> tableau_valeur = Conversion_liste_en_tableau2D(liste_valeur, liste_noeud);

    return tableau_valeur;
}
