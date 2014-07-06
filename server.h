#ifndef SERVER_H
#define SERVER_H

#include <QTcpServer>
#include "blowfish.h"
#include "room.h"



class client;
class Dialog;
class room;

class server : public QTcpServer
{
    Q_OBJECT

public:
    BLOWFISH_CTX *ctx;
    bool _encrypt;
    explicit server(QWidget *widget, QObject *parent = 0);
    bool doStartServer(QHostAddress addr, qint16 port);
    void sendToUser(quint8 comm, QString message, QTcpSocket* &sok);
    void sendTextMessToAll(QString name,QString mess);
    void sendJoinNewUser(QString name);
    void sendListRoom(QTcpSocket* &sok);
    QVector<int> encrypt(QString str);
    QString dencrypt(QVector<int> vec);
    void sendOpenUdp(QString name);
    int getClientCount();
    void sendKeepAlive(QTcpSocket* &sok);

    QStringList getUsersOnline() const;

    void closeAllConnection();

    bool isNameValid(QString name) const;
    bool isNameUsed(QString name) const;

    static  bool running;

    QList <room*> roomList;

signals:
    void newRoom(QString);
public slots:
    void sendMessageToAll(QString name,QString mess);
    void onChangePasswd(BLOWFISH_CTX* ctx_, bool* encr_);
    void sendCryptToAll(QString nam, QVector <int> vec);
    void sendRemoveUser(QString name);
    void stopServer();
    void kick(QString name);


private slots:
    void newUser();
    void onRemoveUser(client*);
protected:
    //    void incomingConnection(int handle);
private:
    QList <client *> _clients;
    QWidget *_widget;


};

#endif // SERVER_H
