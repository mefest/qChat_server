#ifndef VOIP_H
#define VOIP_H

#include <QObject>
#include <QIODevice>
#include <QUdpSocket>
#include <QHostAddress>
#include <QAudioInput>
#include <QAudioOutput>


class voip: public QObject
{
    Q_OBJECT
public:
   explicit voip( quint16 localPort);
    //~voip();

    bool start();

    bool stop();
    void call();
    QAudioFormat GetStreamAudioFormat(void);


signals:
    void clientAddr(QString name,QHostAddress addr,quint16 port);

private:
    QUdpSocket *socket;
    QAudioOutput * audio_output;
    QAudioInput * audio_input;
    QIODevice * audio_device;
    QIODevice * audio_device_input;
    quint16 localPORT;
    QHostAddress remoteHOST;
    quint16 remotePORT;
    const qint64 maxsize;

public slots:

    void readData();
    void sendDatagrams();

};


#endif // VOIP_H
