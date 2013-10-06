#include "dialog.h"
#include "server.h"
#include "client.h"

#include <QTcpSocket>
#include <QDataStream>
#include <QRegExp>

bool server::running=false;
server::server(QWidget *widget, QObject *parent)
{
    _widget=widget;
    connect(_widget,SIGNAL(signalSendMessage(QString,QString)),this,SLOT(sendMessageToAll(QString,QString)));
    connect(_widget,SIGNAL(changePasswd(BLOWFISH_CTX *,bool * )),this,SLOT(onChangePasswd(BLOWFISH_CTX*,bool*)));
}


bool server::doStartServer(QHostAddress addr, qint16 port)
{
    if(!running)
    {
        if (!listen(addr, port))
        {
            qDebug() << "Server not started at" << addr << ":" << port;
            return false;
        }
        running=true;
        connect(this,SIGNAL(newConnection()),this,SLOT(newUser()));
    }

    qDebug() << "Server started at" << addr << ":" << port;
    return true;
}

void server::sendToUser(quint8 comm, QString message, QTcpSocket *&sok)
{
    QByteArray block;
    QDataStream out(&block, QIODevice::WriteOnly);
    out << (quint16)0 << (quint8)comm << message;
    out.device()->seek(0);
    out << (quint16)(block.size() - sizeof(quint16));
    sok->write(block);
    qDebug()<<"отправлено"<<comm<<" "<<message<<"пользователю"<<sok->socketDescriptor();
}

void server::sendTextMessToAll(QString name, QString mess)
{
    QByteArray block;
    QDataStream out(&block, QIODevice::WriteOnly);
    out << (quint16)0 << (quint8)20 << name<<mess;
    out.device()->seek(0);
    out << (quint16)(block.size() - sizeof(quint16));
    for(int i=0; i< _clients.length();++i)
    {
        _clients.at(i)->write(block);
    }
    qDebug()<<"отправлено"<<20<<name<<mess<<"всем";
}

void server::sendJoinNewUser(QString name)
{
    QByteArray block;
    QDataStream out(&block, QIODevice::WriteOnly);
    out << (quint16)0 << (quint8)2 << (QString)"System>Пользователь "+name+" зашел";
    out.device()->seek(0);
    out << (quint16)(block.size() - sizeof(quint16));
   // sok->write(block);
    for(int i=0; i< _clients.length();++i)
    {
        //if(_clients.at(i)->getName()!=name)
            _clients.at(i)->write(block);
    }
    block.clear();
    out.device()->seek(0);

    out << (quint16)0 << (quint8)12 << name;
    out.device()->seek(0);
    out << (quint16)(block.size() - sizeof(quint16));
    for(int i=0; i< _clients.length();++i)
    {
        if(_clients.at(i)->getName()!=name)
            _clients.at(i)->write(block);
    }
    qDebug()<<"отправлено"<<12<<" "<<name<<"пользователям";

}

QVector<int> server::encrypt(QString str)
{
    QVector <int> vec;
    if (str!=""){
        vec.resize(0);
        for(int i=0;i<str.length();++i)
            vec.push_back(str.at(i).unicode());

        unsigned long L = 1, R = 2;
        for(int i=0;i<(vec.size());i=i+2)
        {

            if(vec.size()-i>1)
            {
                L=vec.at(i);
                R=vec.at(i+1);
                Blowfish_Encrypt(ctx, &L, &R);
                vec[i]=(L);
                vec[i+1]=(R);
            }
            else
            {
                L=vec.at(i);
                R=vec.at(0);
                Blowfish_Encrypt(ctx, &L, &R);
                vec[i]=(L);
                vec.push_back(R);
            }
        }
    }
    return vec;
}

QString server::dencrypt(QVector<int> vec)
{
    QString str;
    unsigned long L = 1, R = 2;
    QVector <int> vec2;
    vec2.resize(0);
    for(int i=0;i<(vec.size());i=i+2)
    {
        if(vec.size()-i>1)
        {
            L=vec.at(i);
            R=vec.at(i+1);
            Blowfish_Decrypt(ctx, &L, &R);
            vec2.push_back(L);
            vec2.push_back(R);
        }
        else
        {

            L=vec.at(i);
            R=vec2.at(0);
            Blowfish_Decrypt(ctx, &L, &R);
            vec2.push_back(L);
        }

    }

    for(int i=0;i<vec.size();++i)
        str+=QChar(vec2[i]);
    return str;
}

void server::sendMessageToAll(QString name, QString mess)
{
    if(_encrypt)
    {
        sendCryptToAll(name+">",encrypt(mess));
    }
    else
    {
        sendTextMessToAll(name,mess);
    }
}



QStringList server::getUsersOnline() const
{
    QStringList l;
    foreach (client * c, _clients)
        if (c->getAutched())
            l << c->getName();
    return l;
}

void server::sendCryptToAll(QString nam, QVector<int> vec)
{


    QByteArray block;
    QDataStream out(&block, QIODevice::WriteOnly);
    out << (quint16)0 << (quint8)21 << nam<<vec;
    qDebug()<<21<<vec;
    out.device()->seek(0);
    out << (quint16)(block.size() - sizeof(quint16));
    for(int i=0; i<_clients.length();++i)
        _clients.at(i)->write(block);

}

void server::sendRemoveUser(QString name)
{
    QString temp=name;
    for(int i =0;i<_clients.length();++i)
        sendToUser((quint8)13,name,_clients[i]->_sok);
}

void server::closeAllConnection()
{
    for(int i =0; i<_clients.length();++i)
        _clients.at(i)->close();
}

bool server::isNameValid(QString name) const
{
    if (name.length() > 20 || name.length() < 5)
        return false;
    QRegExp r("[A-Za-z0-9_]+");
    return r.exactMatch(name);
}

bool server::isNameUsed(QString name) const
{

    for (int i = 0; i < _clients.length(); ++i)
    {
        qDebug()<<name<<_clients.at(i)->getName();
        if (_clients.at(i)->getName() == name)
            return true;
    }
    return false;
}

void server::onChangePasswd(BLOWFISH_CTX *ctx_, bool *encr_)
{
    ctx=ctx_;
    _encrypt=*encr_;
}

void server::newUser()
{
    QTcpSocket *clientSocket= this->nextPendingConnection();
    if(clientSocket!=NULL)
    {
        client *newClient =new client( clientSocket,this,this );
        if (_widget != 0)
        {
            connect(newClient,SIGNAL(addUserToGui(QString)),_widget,SLOT(onAddUserToGui(QString)));
            //connect(newClient, SIGNAL(addUserToGui(QString)), _widget, SLOT(onAddUserToGui(QString)));
            connect(newClient, SIGNAL(removeUserFromGui(QString)), _widget, SLOT(onRemoveUserFromGui(QString)));
            connect(newClient,SIGNAL(messageToGui(QString,QString)),_widget,SLOT(onMessageToGui(QString,QString)));



        }
        connect(newClient, SIGNAL(sendMessageAll(QString,QString)), this, SLOT(sendMessageToAll(QString,QString)));
        connect(newClient, SIGNAL(removeUser(client*)), this, SLOT(onRemoveUser(client*)));
        connect(newClient,SIGNAL(sendCrypt(QString,QVector<int>)),this,SLOT(sendCryptToAll(QString,QVector<int>)));
        connect(newClient,SIGNAL(removeUserFromGui(QString)),this,SLOT(sendRemoveUser(QString)));
        _clients.append(newClient);
    }
}

void server::onRemoveUser(client * thisClient)
{
    _clients.removeAt(_clients.indexOf(thisClient));
}



//void server::incomingConnection(int handle)
//{
//    qDebug()<<"incoming Connection";
//    client *newClient= new client(handle,this,this);
//}


