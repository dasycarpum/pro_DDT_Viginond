#ifndef FENETREPRINCIPALE_H
#define FENETREPRINCIPALE_H

#include <QMainWindow>

QT_BEGIN_NAMESPACE
namespace Ui { class FenetrePrincipale; }
QT_END_NAMESPACE

class FenetrePrincipale : public QMainWindow
{
    Q_OBJECT

public:
    FenetrePrincipale(QWidget *parent = nullptr);
    ~FenetrePrincipale();

private:
    Ui::FenetrePrincipale *ui;
};
#endif // FENETREPRINCIPALE_H
