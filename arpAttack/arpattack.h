#ifndef ARPATTACK_H
#define ARPATTACK_H

#include <QDialog>
#include "attackfunc.h"
#include <QThread>

namespace Ui {
    class arpAttack;
}

class arpAttack : public QDialog
{
    Q_OBJECT

public:
    explicit arpAttack(QWidget *parent = nullptr);
    ~arpAttack();
//    void setDev(QString dev);

signals:
    void startAttack(QString source_ip, QString target_ip, QString source_mac, QString target_mac, QString Dev);

private slots:
    void on_pBtn_attack_clicked();
    void on_pBtn_end_clicked();


private:
    Ui::arpAttack *ui;
    QString Dev;
    QThread attack_Thread;
    attackFunc af;
};

#endif // ARPATTACK_H
