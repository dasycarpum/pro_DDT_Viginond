#ifndef DIALOG_PROXY_H
#define DIALOG_PROXY_H

#include <QDialog>

namespace Ui {
class DialogProxy;
}

class DialogProxy : public QDialog
{
    Q_OBJECT

public:
    explicit DialogProxy(QWidget *parent = nullptr);
    ~DialogProxy();

private:
    Ui::DialogProxy *ui;
};

#endif // DIALOG_PROXY_H
