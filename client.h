#ifndef CLIENT_H
#define CLIENT_H

#include <QObject>
#include "server.h"
#include <QTimer>
class server;
class client : public QObject
{
    friend class server;
    Q_OBJECT


public:
    //    BLOWFISH_CTX *ctx;
    //    bool _encrypt;
    static int countClient;
    static const quint8 comAutchReq = 0;
    static const quint8 comGetNick = 1;
    static const quint8 comMessageToAll = 20;
    static const quint8 comMessageAllEncrypt = 21;
    QString _name;
    QTcpSocket *soket;
    explicit client(int desc, server *serv, QObject *parent = 0);
    explicit client(QTcpSocket *sok, server *serv, QObject *parent = 0);
    static const QString constNameUnknown;
    void write(QByteArray &block );
    void doSendUsersOnline() const;
    QString decrypt(QVector <int> data);
    void sendAddr(QHostAddress addr, quint16 port);
    void init();

    ~client();

    QString getName() const {return _name;}
    bool getAutched() const {return _isAutched;}
    void close();

signals:
    void sendMessageAll(QString name, QString mess);
    void addUserToGui(QString name);
    void removeUser(client *client);
    void removeUserFromGui(QString name);
    void messageToGui(QString name, QString mess);
    void sendCrypt(QString nam, QVector <int> vec);
    void newWait(QString name,QString name2);
    
public slots:

private slots:
    void alive();
    void onConnect();
    void onConnectSoket();
    void onDisconnect();
    void onReadyRead();
    void onError(QAbstractSocket::SocketError socketError) const;

private:
    bool keepAlive;
    QTcpSocket *_sok;
    QTimer *timer;
    server *_serv;
    quint16 _blockSize;

    bool _isAutched;
    
};

#endif // CLIENT_H
