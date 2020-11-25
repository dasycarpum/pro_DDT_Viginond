#include "dialog_proxy.h"
#include "ui_dialog_proxy.h"

DialogProxy::DialogProxy(QWidget *parent) :
    QDialog(parent),
    ui(new Ui::DialogProxy)
{
    ui->setupUi(this);
}

DialogProxy::~DialogProxy()
{
    delete ui;
}
