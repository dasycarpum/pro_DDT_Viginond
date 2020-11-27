#ifndef DIALOG_PROXY_H
#define DIALOG_PROXY_H

#include <QDialog>
#include <QDir>
#include <QFile>
#include <QTextStream>
#include <QMessageBox>


namespace Ui {
class DialogProxy;
}

class DialogProxy : public QDialog
{
    Q_OBJECT
    Ui::DialogProxy *ui;

public:
    explicit DialogProxy(QWidget *parent = nullptr);
    ~DialogProxy();

public slots:
    void Enregistrement_parametres(void);

};

#endif // DIALOG_PROXY_H
