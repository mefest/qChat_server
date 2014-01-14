#ifndef WAIT_H
#define WAIT_H

#include <QObject>
#include<QHostAddress>

class waited : public QObject
{
    Q_OBJECT
public:
    explicit waited(QString name,QString name2,QObject *parent = 0);
    QString client1,client2;
    QHostAddress addrClient1,addrClient2;
    quint16 portClient1,portClient2;
signals:
    void completed(QString name1,QHostAddress addr1,quint16 port1,QString name2,QHostAddress addr2,quint16 port2);

public slots:
    void setAddr(QString name,QHostAddress addr,quint16 port);

};

#endif // WAIT_H
