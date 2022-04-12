#include "arpattack.h"
#include "ui_arpattack.h"
#include <QMessageBox>

arpAttack::arpAttack(QWidget *parent) :
    QDialog(parent),
    ui(new Ui::arpAttack)
{
    ui->setupUi(this);
    ui->lineEdit_dev->setText("ens33");
    Dev = ui->lineEdit_dev->text();
    Dev = "ens33";
    af.moveToThread(&attack_Thread);
    connect(this, &arpAttack::startAttack, &af, &attackFunc::attack);
}

arpAttack::~arpAttack()
{
    delete ui;
}

void arpAttack::on_pBtn_attack_clicked(){
    ui->pBtn_attack->setDisabled(true);
    if(attack_Thread.isRunning()){
        QMessageBox::warning(this, "警告", "正在攻击中。。。如需重新输入信息，请点击停止");
        return;
    }

    if(ui->lineEdit_sourceIP->text()=="" || ui->lineEdit_targetIP->text()=="" || ui->lineEdit_targetMac->text()=="" || ui->lineEdit_sourceMAC->text()==""){
        QMessageBox::warning(this, "警告", "请输入必要信息后再尝试");
        return;
    }

//    char *source_ip = ui->lineEdit_sourceIP->text().toLocal8Bit().data();       //ARP回复的源IP     target_ip_getway
//    char *target_ip = ui->lineEdit_targetIP->text().toLocal8Bit().data();       //ARP回复的目标IP    sender_ip_getway
//    char *source_mac = ui->lineEdit_sourceMAC->text().toLocal8Bit().data();
//    char *target_mac = ui->lineEdit_targetMac->text().toLocal8Bit().data();

    attack_Thread.start();
    af.setFlag(true);
    emit startAttack(ui->lineEdit_sourceIP->text(), ui->lineEdit_targetIP->text(), ui->lineEdit_sourceMAC->text(), ui->lineEdit_targetMac->text(), Dev);
    QMessageBox::information(this, "提示", "正在攻击中");
}


void arpAttack::on_pBtn_end_clicked(){
    ui->pBtn_attack->setEnabled(true);
    af.setFlag(false);
    attack_Thread.quit();
}

//void arpAttack::setDev(QString dev){
//    Dev = dev.toLocal8Bit().data();
//    qDebug() << Dev;
//}

