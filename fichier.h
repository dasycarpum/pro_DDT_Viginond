#ifndef FICHIER_H
#define FICHIER_H

#include <QFile>
#include <QDir>
#include <QCoreApplication>
#include <QMessageBox>
#include <QXmlStreamReader>

class FichierCsv : public QFile
{
    const QString extension;
    const QString separateur;           // séparateur de champ, par défaut un ';'

public:
    QVector<QVector<QString>> matrix;   // matrice de stockage des attributs, indexée [ligne][colonne]

    FichierCsv(QString const& nom, QString const& sep = ";") :
        extension(".csv"), separateur(sep)
        {setFileName(QCoreApplication::applicationDirPath() + nom + extension);}
    ~FichierCsv(){}

    void Lire(void);
    static QStringList Liste_sous_repertoires(QString const&);
};

/*_______________________________________________________________________________________________________*/

class FichierXml
{
public:
    FichierXml(){}
    ~FichierXml(){}

    static QVector<QVector<QString>> Lecture_valeur(const QByteArray &, QList<QString> const&);
};


#endif // FICHIER_H