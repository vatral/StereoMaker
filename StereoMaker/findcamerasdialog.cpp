#include "findcamerasdialog.h"
#include "ui_findcamerasdialog.h"

FindCamerasDialog::FindCamerasDialog(QWidget *parent)
    : QDialog(parent)
    , ui(new Ui::FindCamerasDialog)
{
    ui->setupUi(this);
}

FindCamerasDialog::~FindCamerasDialog()
{
    delete ui;
}
