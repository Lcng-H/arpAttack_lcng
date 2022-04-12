#ifndef SENDREQUEST_H
#define SENDREQUEST_H

#include <QObject>
#include "head.h"

class SendRequest : public QObject
{
    Q_OBJECT
public:
    explicit SendRequest(QObject *parent = nullptr);
    void setflag(bool flag);

public slots:
    void send(QString Dev);

signals:

private:
    int stopSend;
    void sendArpRequest(QString Dev);
};

#endif // SENDREQUEST_H
