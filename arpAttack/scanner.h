#ifndef SCANNER_H
#define SCANNER_H

#include <QObject> 
#include "head.h"

class Scanner : public QObject
{
    Q_OBJECT
public:
    explicit Scanner(QObject *parent = nullptr);
    void setFlag(bool);
    void getArpReply();

public slots:

signals:
    void arpReplyinfo(char* ip, char *mac, bool flag);

private:
    bool isRunning;
    char *mac_ntoa(u_char* d);
};

#endif // SCANNER_H
