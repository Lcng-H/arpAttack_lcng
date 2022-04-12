#include "mainwindow.h"
#include "ui_mainwindow.h"

MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent)
    , ui(new Ui::MainWindow)
{
    ui->setupUi(this);
//    Dev = "ens33";

    ui->lineEdit->setText("ens33");
    ui->tableWidget->setColumnCount(2);
    ui->tableWidget->setHorizontalHeaderLabels(QStringList() <<"IP地址"<<"MAC地址");
    ui->tableWidget->setSelectionBehavior(QAbstractItemView::SelectRows);//整行选中的方式
    ui->tableWidget->setEditTriggers(QAbstractItemView::NoEditTriggers);//禁止修改
    ui->tableWidget->setSelectionMode(QAbstractItemView::SingleSelection);//可以选中单个
    ui->tableWidget->setStyleSheet("selection-background-color:lightblue;");
    ui->tableWidget->horizontalHeader()->setSectionResizeMode(QHeaderView::ResizeToContents);

    sc.moveToThread(&scan_Thread);
    sr.moveToThread(&send_Thread);

    connect(this, &MainWindow::startRequest, &sr, &SendRequest::send);
    connect(this, &MainWindow::startGetReply, &sc, &Scanner::getArpReply);
    connect(&sc, &Scanner::arpReplyinfo, this, [=](char *sip, char *smac, bool flag){
        if(flag){
            ui->tableWidget->clearContents();
            ui->tableWidget->setRowCount(0);
        }
        int RowCont = ui->tableWidget->rowCount();
        ui->tableWidget->insertRow(RowCont);//增加一行

        ui->tableWidget->setItem(RowCont, 0, new QTableWidgetItem(QString(sip)));
        ui->tableWidget->setItem(RowCont, 1, new QTableWidgetItem(QString(smac)));
    },Qt::DirectConnection);
}

MainWindow::~MainWindow(){
    delete ui;
}


void MainWindow::on_action_getDev_triggered(){
    gdInfo->show();
}

void MainWindow::on_pBtn_startScan_clicked(){
    ui->pBtn_startScan->setDisabled(true);
    if(scan_Thread.isRunning()){
        QMessageBox::warning(this, "警告", "正在扫描中。。。如需重新扫描，请点击结束");
        return;
    }
    if(ui->lineEdit->text() == ""){
        QMessageBox::warning(this, "警告", "请输入选择的网卡名");
        return;
    }

    scan_Thread.start();   //启动线程
    send_Thread.start();
    sc.setFlag(true);
    sr.setflag(true);

    QString str=ui->lineEdit->text();
    Dev = str.toLocal8Bit().data();
    emit startRequest(str);     //发送信号，启动扫描
    emit startGetReply();
}

void MainWindow::on_pBtn_endScan_clicked(){
    ui->pBtn_startScan->setEnabled(true);
    sc.setFlag(false);
    sr.setflag(false);
    scan_Thread.terminate();
    send_Thread.terminate();


}


void MainWindow::on_actionARP_triggered(){
//    aa->setDev(QString(Dev));
//    qDebug() << QString(Dev);
//    qDebug() << "!!!";
    aa->show();
}

