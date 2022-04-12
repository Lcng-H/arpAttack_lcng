#ifndef ATTACKFUNC_H
#define ATTACKFUNC_H

#include <QObject>
#include <head.h>

class attackFunc : public QObject
{
    Q_OBJECT
public:
    explicit attackFunc(QObject *parent = nullptr);

signals:

public slots:
    void setFlag(bool flag);
    void attack(QString source_ip, QString target_ip, QString source_mac, QString target_mac, QString Dev);

private:
    void charToInt(unsigned char *IP, char *str);
    void str2Mac(unsigned char *Mac, char *str);
    unsigned char charToData(const char ch);
    bool isRunning;

};

#endif // ATTACKFUNC_H
