#ifndef HEAD_H
#define HEAD_H

#endif // HEAD_H

#include <string.h>
#include <set>
#include <unistd.h>

#include <sys/socket.h>
#include <sys/ioctl.h>
#include <sys/types.h>
#include <sys/time.h>
#include <arpa/inet.h>

#include <linux/if.h>
#include <linux/if_packet.h>
#include <linux/if_ether.h>
//#include <linux/if_arp.h>
#include <linux/sockios.h>

#include <net/ethernet.h>
#include <netinet/if_ether.h>
#include <netinet/in_systm.h>
#include <netinet/in.h>
#include <netinet/tcp.h>
#include <netinet/udp.h>
#include <netinet/ip.h>
#include <netinet/ip_icmp.h>

#include <QDebug>
#include <QMessageBox>
#include <QThread>

#define IPV4_LENGTH 4
#define buffer_len 60   //ARP请求包大小为60B,抓包时会抓到一些42B的包，这是抓包软件没有显示18B的Padding字段，Padding全0填充在包的末尾



