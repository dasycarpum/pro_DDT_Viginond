#ifndef RESEAU_H
#define RESEAU_H

#include <QObject>
#include <QHostInfo>

class Reseau : public QObject
{
    Q_OBJECT

public:
    explicit Reseau(QObject *parent = nullptr);

    bool Test_connexion(QString const& = "google.com");

signals:

public slots:
};

#endif // RESEAU_H
