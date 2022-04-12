#include "scanner.h"
#define MAXSIZE 4096

Scanner::Scanner(QObject *parent) : QObject(parent){
    isRunning = true;
}

void Scanner::setFlag(bool flag){
    isRunning = flag;
}

void Scanner::getArpReply(){
    struct ether_header* eth; 	//Ethernet structure
    struct ether_arp* arp;	  	//Arp structure
    int s;					  	//socket descriptor
    int len;				  	//data length received
    char buff[MAXSIZE];		  	//buffer
    char* p;				  	//initial pointer
    char* p0;				  	//packet pointer

    if ((s = socket(AF_PACKET, SOCK_RAW, htons(ETH_P_ARP))) == -1){
        perror("socket");
        exit(EXIT_FAILURE);
    }
    std::set<QString> mySet;
    qDebug() <<"Start!\n";
    while (isRunning)			//从socket套接字中获取数据包
    {
        if ((len = read(s, buff, MAXSIZE)) < 0)
        {
            perror("read");
            exit(EXIT_FAILURE);
        }
        p = p0 = buff;
        eth = (struct ether_header*)p;
        p = p + sizeof(struct ether_header);

        //for arp
        if (ntohs(eth->ether_type) == ETHERTYPE_ARP){
            arp = (struct ether_arp*)p;
            char *smac  = 0;
            char *sip = 0;
            if(ntohs(arp->ea_hdr.ar_op) == ARPOP_REPLY){
                bool flag = false;
                smac = mac_ntoa(arp->arp_sha);
                sip = inet_ntoa(*(struct in_addr*)&arp->arp_spa);
                qDebug() << sip;
                if(mySet.count(QString(sip)) == 0){
                    if(mySet.empty())   flag = true;
                    emit arpReplyinfo(sip, smac, flag);
                    mySet.insert(QString(sip));
                }

            }
        }
    }
    ::close(s);
    qDebug()<<"end!";
}

char *Scanner::mac_ntoa(u_char *d){
    static char str[50];
    sprintf(str, "%02x:%02x:%02x:%02x:%02x:%02x", d[0], d[1], d[2], d[3], d[4], d[5]);
    return str;
}
