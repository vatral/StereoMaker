#ifndef FINDCAMERASDIALOG_H
#define FINDCAMERASDIALOG_H

#include <QDialog>

namespace Ui {
class FindCamerasDialog;
}

class FindCamerasDialog : public QDialog
{
    Q_OBJECT

public:
    explicit FindCamerasDialog(QWidget *parent = nullptr);
    ~FindCamerasDialog();

private:
    Ui::FindCamerasDialog *ui;
};

#endif // FINDCAMERASDIALOG_H
