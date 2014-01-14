#ifndef SERVER_H
#define SERVER_H

#include <QTcpServer>
#include "blowfish.h"
#include "voip.h"
#include "wait.h"


class client;
class Dialog;

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
    QVector<int> encrypt(QString str);
    QString dencrypt(QVector<int> vec);
    void sendOpenUdp(QString name);


    QStringList getUsersOnline() const;

    waited *waiting;
    void closeAllConnection();

    voip *voipServ;
    bool isNameValid(QString name) const;
    bool isNameUsed(QString name) const;

    static  bool running;
public slots:
    void sendMessageToAll(QString name,QString mess);
    void onChangePasswd(BLOWFISH_CTX* ctx_, bool* encr_);
    void sendCryptToAll(QString nam, QVector <int> vec);
    void sendRemoveUser(QString name);
    void stopServer();
    void kick(QString name);
    void startNewWait(QString name,QString name2);
    void sendAddr(QString name1,QHostAddress addr1,quint16 port1,QString name2,QHostAddress addr2,quint16 port2);


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
