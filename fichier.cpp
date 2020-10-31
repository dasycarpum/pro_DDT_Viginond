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
        foreach (QString const& attribut, champs) { // complétude du tableau
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

    for (int i(0); i < listRepertoire.size(); ++i)
    {
        QFileInfo fileInfos = listRepertoire.at(i);

        if (fileInfos.baseName().size() > 0)
            liste.append(fileInfos.baseName());
    }

    return liste;
}
