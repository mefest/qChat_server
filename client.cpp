#include "client.h"
#include <QDebug>
#include <QStringList>
#include <QTcpSocket>
#include <QDataStream>
#include <QVector>
#include <QMessageBox>
#include <QFile>
#include <QDir>

const QString client::constNameUnknown = QString(".Unknown");

client::client(int desc, server *serv, QObject *parent)
{

    //храниим указатель на объект-сервер
    _serv = serv;
    //клиент не прошел авторизацию
    _isAutched = false;
    _name = constNameUnknown;
    //размер принимаемого блока 0
    _blockSize = 0;
    //создаем сокет
    _sok = new QTcpSocket(this);
    //устанавливаем дескриптор из incomingConnection()
    _sok->setSocketDescriptor(desc);
    //подключаем сигналы
    connect(_sok, SIGNAL(connected()), this, SLOT(onConnect()));
    connect(_sok, SIGNAL(disconnected()), this, SLOT(onDisconnect()));
    connect(_sok, SIGNAL(readyRead()), this, SLOT(onReadyRead()));
    connect(_sok, SIGNAL(error(QAbstractSocket::SocketError)), this, SLOT(onError(QAbstractSocket::SocketError)));

    qDebug() << "Client connected" << desc;
}

client::client(QTcpSocket *sok, server *serv, QObject *parent)
{
    //храниим указатель на объект-сервер

    _serv = serv;
    //клиент не прошел авторизацию
    _isAutched = false;
    _name = constNameUnknown;
    //размер принимаемого блока 0
    _blockSize = 0;
    //создаем сокет
    _sok = sok;
    //устанавливаем дескриптор из incomingConnection()
    //_sok->setSocketDescriptor(desc);
    //подключаем сигналы
    connect(_sok, SIGNAL(connected()), this, SLOT(onConnect()));
    connect(_sok, SIGNAL(disconnected()), this, SLOT(onDisconnect()));
    connect(_sok, SIGNAL(readyRead()), this, SLOT(onReadyRead()));
    connect(_sok, SIGNAL(error(QAbstractSocket::SocketError)), this, SLOT(onError(QAbstractSocket::SocketError)));

    qDebug() << "Client connected" << _sok->socketDescriptor();
}

void client::write(QByteArray &block)
{
    _sok->write(block);
}

void client::doSendUsersOnline() const
{
    QByteArray block;
    QDataStream out(&block, QIODevice::WriteOnly);
    // out << (quint16)0;
    // out << comUsersOnline;
    QStringList l = _serv->getUsersOnline();

    //    QString s;
    //    for (int i = 0; i < l.length(); ++i)
    //        if (l.at(i) != _name)
    //            s += l.at(i)+(QString)",";
    //    s.remove(s.length()-1, 1);
    qDebug()<<l;
    for(int i=0;i<l.length();++i)
    {
        out.device()->seek(0);
        out << (quint16)0;
        out << (quint8)12;
        out << l.at(i);
        out.device()->seek(0);
        out << (quint16)(block.size() - sizeof(quint16));
        _sok->write(block);
        block.clear();
    }
    qDebug() << "Send user list to" << _name << ":" ;
}

client::~client()
{
}

void client::close()
{
    _sok->close();
}

void client::onConnect()
{
}

void client::onConnectSoket()
{
    QByteArray block;
    QDataStream out(&block, QIODevice::WriteOnly);
    out << (quint16)0 << (quint8)20 << "это другой текст";
    out.device()->seek(0);
    out << (quint16)(block.size() - sizeof(quint16));
    soket->write(block);
}

void client::onDisconnect()
{
    qDebug() << "Client disconnected";


    _sok->close();
    deleteLater();
    //delete this;
    emit removeUser(this);
    emit removeUserFromGui(_name);


}

void client::onReadyRead()
{
    if(_serv->running){

        QDataStream in(_sok);
        QString temp;
        while(!in.atEnd())
        {
            if(_blockSize==0){
                if (_sok->bytesAvailable() < (int)sizeof(quint16))
                    return;
                in >> _blockSize;
                qDebug()<<"Полученно------------";
                qDebug()<<"размер"<<_blockSize;
            }
            if (_sok->bytesAvailable() < _blockSize)
                return;
            else
                _blockSize = 0;
            quint8 command;
            in >> command;
            qDebug()<<"команда"<<command;
            switch (command)
            {
            case 0:
                in>>temp;
                qDebug()<<"текст"<<temp;
                _serv->sendToUser((quint8) 0,(QString)"connect accept",_sok);
                break;
            case 1:
                in>> temp;
                qDebug()<<temp;
                //_name=temp;
                if (!_serv->isNameValid(temp))
                {
                    //отправляем ошибку
                    qDebug()<<"имя не валидно";
                    _serv->sendToUser((quint8) 1,(QString)"Имя пользователя не подходит",_sok);
                    _sok->close();
                    return;
                }
                qDebug()<<"имя валидно";
                if (_serv->isNameUsed(temp))
                {
                    //отправляем ошибку
                    //  doSendCommand(comErrNameUsed);
                    qDebug()<<"имя занято";
                    _serv->sendToUser((quint8) 1,(QString)"Имя пользователя занято",_sok);
                    _sok->close();
                    return;
                }
                qDebug()<<"имя свободно";
                //авторизация пройдена
                _name=temp;
                _isAutched=true;
            {
                QFile file(QDir::currentPath()+"/motd.txt");
                if (!file.open(QIODevice::ReadOnly | QIODevice::Text))
                {
                    qDebug()<<"file motd.txt not found";
                }
                else{


                    QByteArray total;
                    QByteArray line;
                    while (!file.atEnd()) {
                        line = file.read(1024);
                        total.append(line);
                    }


                    _serv->sendToUser((quint8) 2,(QString)"System>"+QString(total),_sok);
                }
            }

                //отправляем клиенту список онлай пользователей

                doSendUsersOnline();
                emit addUserToGui(temp);
                //говорим всем что пользователь вошел
                _serv->sendJoinNewUser(_name);


                break;
            case 20:
            {
                in>>temp;
                qDebug()<<temp;
                _serv->sendTextMessToAll(_name,temp);
                emit messageToGui(_name,temp);
                break;
            }
            case 21:
            {
                QVector <int> vec;
                vec.resize(0);
                in>>vec;
                qDebug()<<vec;
                emit sendCrypt(_name+">",vec);
                QString mes;
                mes=_serv->dencrypt(vec);

                emit messageToGui(_name,mes);
                break;

            }
            case 25:
            {
                in>>temp;
                qDebug()<<temp;
                qDebug()<<_sok->localAddress()<<_sok->localPort();
                qDebug()<<_sok->peerAddress()<<_sok->peerPort();
                _serv->sendToUser((quint8)25,QString::number(_sok->peerPort()),_sok);
                //                QByteArray block;
                //                QDataStream out(&block, QIODevice::WriteOnly);
                //                out << (quint16)0 << (quint8)20 << "это другой текст";
                //                out.device()->seek(0);
                //                out << (quint16)(block.size() - sizeof(quint16));
                //                soket->write(block);
            }
            case 30:
            {
               in>>temp;
               qDebug()<<temp;
              _serv->sendOpenUdp(temp);
              _serv->sendOpenUdp(_name);
              emit newWait(_name,temp);
              //_serv->voipServ->waitRead();
                break;
            }
            }
        }
    }
}

void client::sendAddr(QHostAddress addr, quint16 port)
{
    QByteArray block;
    QDataStream out(&block, QIODevice::WriteOnly);
    out << (quint16)0 << (quint8)32 << addr<<port;
    out.device()->seek(0);
    out << (quint16)(block.size() - sizeof(quint16));
    _sok->write(block);
    qDebug()<<"отправлено"<<32<<" "<<addr<<port;
}
void client::onError(QAbstractSocket::SocketError socketError) const
{
    switch (socketError) {
    case QAbstractSocket::RemoteHostClosedError:
        break;
    case QAbstractSocket::HostNotFoundError:
        qDebug()<<tr("Хост не найден. Пожалуйста проверьте "
                     "настройки сервера.");

        break;
    case QAbstractSocket::ConnectionRefusedError:
        qDebug()<<tr("The connection was refused by the peer. "
                     "Убедитесь, что сервер запущен "
                     "и провельте адресс сервера и порт. "
                     "settings are correct.");

        break;
    default:
        qDebug()<<tr("The following error occurred: %1.")
                  .arg(soket->errorString());

    }
}
