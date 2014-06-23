#ifndef ROOM_H
#define ROOM_H
#include <QList>
#include <QString>
#include "client.h"

class client;
class room
{
public:
    room(QString name);

    unsigned int id;
    QList <client*> clients;
    QString name;
    bool isPrivate;
    QString paswd;
    bool secure;
    static unsigned int countRooms;

};

#endif // ROOM_H
