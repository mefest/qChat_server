#include "wait.h"

waited::waited(QString name, QString name2, QObject *parent)
{
    client1=name;
    client2=name2;
    portClient1=0;
    portClient2=0;
}

void waited::setAddr(QString name, QHostAddress addr, quint16 port)
{
    if(name==client1)
    {
        addrClient1=addr;
        portClient1=port;
    }
    if(name==client2)
    {
        addrClient2=addr;
        portClient2=port;
    }
    if(portClient1!=0 && portClient2!=0)
        emit completed(client1,addrClient1,portClient1,client2,addrClient2,portClient2);
}
