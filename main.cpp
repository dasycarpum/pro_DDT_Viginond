#include "fenetre_principale.h"

#include <QApplication>
#include <QStyleFactory>
#include <QProxyStyle>

int main(int argc, char *argv[])
{
    /* Style général */
    QApplication a(argc, argv);
    QStyle *style = new QProxyStyle(QStyleFactory::create("fusion"));
    a.setStyle(style);

    /* Couleur de fond */
    QPalette palette;
    palette = qApp->palette();
    palette.setColor(QPalette::Window, QColor(159, 195, 233));
    qApp->setPalette(palette);

    FenetrePrincipale w;
    w.show();
    return a.exec();
}
