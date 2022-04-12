#include "getdevinfo.h"
#include "ui_getdevinfo.h"

getDevInfo::getDevInfo(QWidget *parent) :
    QDialog(parent),
    ui(new Ui::getDevInfo)
{
    ui->setupUi(this);

    ui->tableWidget->setColumnCount(2);
    ui->tableWidget->setHorizontalHeaderLabels(QStringList() <<"网卡名"<<"网卡IP");
    ui->tableWidget->setEditTriggers(QAbstractItemView::NoEditTriggers);//禁止修改
    ui->tableWidget->setSelectionMode(QAbstractItemView::SingleSelection);//可以选中单个
    ui->tableWidget->horizontalHeader()->setSectionResizeMode(QHeaderView::ResizeToContents);

    int i=0;
    int sockfd;
    struct ifconf ifconf;
    unsigned char buf[512];
    struct ifreq *ifreq;
    //初始化ifconf
    ifconf.ifc_len = 512;
    ifconf.ifc_buf = (char*)buf;
    if ((sockfd = socket(AF_INET, SOCK_DGRAM, 0))<0){
        perror("socket" );
        exit(1);
    }
    ioctl(sockfd, SIOCGIFCONF, &ifconf); //获取所有接口信息
    //接下来一个一个的获取IP地址
    ifreq = (struct ifreq*)buf;
    for (i=(ifconf.ifc_len/sizeof (struct ifreq)); i>0; i--){
        int RowCont = ui->tableWidget->rowCount();
        ui->tableWidget->insertRow(RowCont);//增加一行

        ui->tableWidget->setItem(RowCont, 0, new QTableWidgetItem(QString(ifreq->ifr_name)));
        ui->tableWidget->setItem(RowCont, 1, new QTableWidgetItem(QString(inet_ntoa(((struct sockaddr_in*)&(ifreq->ifr_addr))->sin_addr))));
        printf("name = [%s]\n" , ifreq->ifr_name);
        printf("local addr = [%s]\n" ,inet_ntoa(((struct sockaddr_in*)&(ifreq->ifr_addr))->sin_addr));
        ifreq++;
    }
    ::close(sockfd);
}

getDevInfo::~getDevInfo()
{
    delete ui;
}
