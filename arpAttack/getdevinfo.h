#ifndef GETDEVINFO_H
#define GETDEVINFO_H

#include <QDialog>
#include <head.h>

namespace Ui {
class getDevInfo;
}

class getDevInfo : public QDialog
{
    Q_OBJECT

public:
    explicit getDevInfo(QWidget *parent = nullptr);
    ~getDevInfo();

private:
    Ui::getDevInfo *ui;
};

#endif // GETDEVINFO_H
