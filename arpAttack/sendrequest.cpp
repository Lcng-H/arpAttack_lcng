#include "sendrequest.h"

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

SendRequest::SendRequest(QObject *parent) : QObject(parent){
    stopSend = true;
}

void SendRequest::send(QString Dev){
    sendArpRequest(Dev);
}

void SendRequest::setflag(bool flag){
    stopSend = flag;
}

void SendRequest::sendArpRequest(QString Dev){
    unsigned char sender_ip[4] = {0};    //ARP请求的源IP
    unsigned char target_ip[4] = {192,168,147,1};     //ARP请求的目标IP

    //创建buffer
    unsigned char buffer[buffer_len];
    memset(buffer, 0, buffer_len);
    //创建以太网头部指针,指向buffer
    struct ethhdr *eth_req = (struct ethhdr*)buffer;
    //创建ARP包指针，指向buffer的后46字节，因为以太网头包含：2*6B(MAC地址)+2B(协议地址)=14B
    struct arp_head *arp_req = (struct arp_head*)(buffer+14);
    //创建sockaddr_ll结构地址
    struct sockaddr_ll sock_addr;

    int sock_fd = socket(AF_PACKET, SOCK_RAW, htons(ETH_P_ARP));
    if(sock_fd == -1){
        perror("socket()");
        exit(-1);
    }

    struct ifreq ifr;
    /*根据网卡设备名获取Index*/
    strcpy(ifr.ifr_name, Dev.toLocal8Bit().data());
    if(ioctl(sock_fd, SIOCGIFINDEX, &ifr) == -1){
        perror("SIOCGIFINDEX");
        exit(-1);
    }
    int ifindex = ifr.ifr_ifindex;
    printf("网卡索引为:%d\n",ifindex);

    /*获取网卡设备IP地址*/
    if(ioctl(sock_fd, SIOCGIFADDR, &ifr) == -1){
        perror("SIOCGIFADDR");
        exit(-1);
    }
    char *t = inet_ntoa(((struct sockaddr_in*)&(ifr.ifr_addr))->sin_addr);
    int j=0;
    for(int i=0; i<4;i++){
        int num = 0;
        for(; j<(int)strlen(t); j++){
            if( t[j] == '.' ){
                j++;
                break;
            }
            else{
                num *= 10;
                num += t[j] -'0';
            }
        }
        sender_ip[i] = num;
    }

    /*获取网卡设备MAC地址*/
    if(ioctl(sock_fd, SIOCGIFHWADDR, &ifr) == -1){
        perror("SIOCGIFHWADDR");
        exit(-1);
    }

    /*将MAC地址写入所需结构*/
    for(int i=0;i<6;i++)
    {
        //以太网帧的目标MAC，即广播MAC，全1
        eth_req->h_dest[i] = (unsigned char)0xff;
        //ARP请求包目标MAC，全0
        arp_req->target_mac[i] = (unsigned char)0x00;
        //以太网帧源MAC，即本机MAC
        //ifr_hwaddr是sockaddr结构体格式
        eth_req->h_source[i] = (unsigned char)ifr.ifr_hwaddr.sa_data[i];
        //ARP请求包源MAC，即本机MAC
        arp_req->sender_mac[i] = (unsigned char)ifr.ifr_hwaddr.sa_data[i];
        //sockaddr中的MAC，也是本地MAC
        sock_addr.sll_addr[i] = (unsigned char)ifr.ifr_hwaddr.sa_data[i];
    }

    /*打印MAC地址*/
    printf("网卡MAC地址: %02X:%02X:%02X:%02X:%02X:%02X\n",
            eth_req->h_source[0],
            eth_req->h_source[1],
            eth_req->h_source[2],
            eth_req->h_source[3],
            eth_req->h_source[4],
            eth_req->h_source[5]);

    /*完善sockaddr_ll结构体*/
    sock_addr.sll_family = PF_PACKET;
    sock_addr.sll_protocol = htons(ETH_P_ARP);
    sock_addr.sll_ifindex = ifindex;
    sock_addr.sll_hatype = htons(ARPHRD_ETHER);
    sock_addr.sll_halen = ETH_ALEN;

    /*完善以太网帧头*/
    eth_req->h_proto = htons(ETH_P_ARP);

    /*完善ARP包头*/
    arp_req->hardware_type = htons(0x01);
    arp_req->protocol_type = htons(ETH_P_IP);
    arp_req->hardware_size = ETH_ALEN;
    arp_req->protocol_size = IPV4_LENGTH;
    arp_req->opcode = htons(ARPOP_REQUEST);
    memcpy(arp_req->sender_ip,sender_ip,IPV4_LENGTH);

    while(stopSend){
        for(int j=0; j<256; j++){
            for(int i=0; i<256; i++){
                target_ip[0] = 192;
                target_ip[1] = 168;
                target_ip[2] = j;
                target_ip[3] = i;
                memcpy(arp_req->target_ip,target_ip,IPV4_LENGTH);
                /*发送ARP请求*/
                if(sendto(sock_fd, buffer, 60, 0, (struct sockaddr*)&sock_addr, sizeof(sock_addr)) == -1){
                    perror("sendto()");
                    exit(-1);
                }
            }
        }

        for(int k=16; k<32; k++){
            for(int j=0; j<256; j++){
                for(int i=0; i<256; i++){
                    target_ip[0] = 172;
                    target_ip[1] = k;
                    target_ip[2] = j;
                    target_ip[3] = i;
                    memcpy(arp_req->target_ip,target_ip,IPV4_LENGTH);
                    /*发送ARP请求*/
                    if(sendto(sock_fd, buffer, 60, 0, (struct sockaddr*)&sock_addr, sizeof(sock_addr)) == -1){
                        perror("sendto()");
                        exit(-1);
                    }
                }
            }
        }
    }

    printf("end!!\n");
    close(sock_fd);
}
