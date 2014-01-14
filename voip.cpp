#include "voip.h"

voip::voip(quint16 localPort)
    : maxsize(8192)
{
    //socket= new QUdpSocket(this);
    localPORT=localPort;
//    remoteHOST=remoteHost;
//    remotePORT=remotePort;
}

bool voip::start()
{
    socket= new QUdpSocket(this);
    //    audio_output= new QAudioOutput(GetStreamAudioFormat());
    //    audio_device=audio_output->start();
    connect(socket,SIGNAL(readyRead()),this,SLOT(readData()));
    return socket->bind(QHostAddress::Any, localPORT, QUdpSocket::DontShareAddress);
}

void voip::call()
{
    audio_output= new QAudioOutput(GetStreamAudioFormat());
    audio_device=audio_output->start();
    //<--here
    audio_input = new QAudioInput(GetStreamAudioFormat());
    audio_device_input = audio_input->start();
    connect(audio_device_input, SIGNAL(readyRead()), this, SLOT(sendDatagrams()));
}


QAudioFormat voip::GetStreamAudioFormat()
{
    QAudioFormat format;
    format.setSampleRate(44100);
    format.setChannelCount(1);
    //    format.setChannels(1);
    format.setSampleSize(16);
    format.setCodec("audio/pcm");
    format.setByteOrder(QAudioFormat::LittleEndian);
    format.setSampleType(QAudioFormat::UnSignedInt);

    QAudioDeviceInfo info = QAudioDeviceInfo::defaultInputDevice();
    if (!info.isFormatSupported(format))
        format = info.nearestFormat(format);

    return format;
}


void voip::readData()
{
    //qDebug()<<"read";
    while (socket->hasPendingDatagrams())
    {
        qint64 size = socket->pendingDatagramSize();
        qDebug()<<"size"<<size;
        //        if (size>5)
        //        {
        //        char * data = new char[size];
        //        socket->readDatagram(data, size);
        //        audio_device->write(data, size);
        //        }
        //        else
        //        {

        char * data = new char[size];
        QHostAddress *host=new QHostAddress();
        quint16 *port=new quint16;
        socket->readDatagram(data, size,host,port);
        qDebug()<<"UDP";
        QString str;
        for(int i=5;i<size;i+=2)
        {
            //qDebug()<<data[i];
            str+=data[i];
        }
        //str.fromRawData(const_cast<QChar*>(data),size);
        qDebug()<<"str="<<str<<"host"<<*host<<"port"<<*port;
        emit clientAddr(str,*host,*port);



        //        }

    }
}

void voip::sendDatagrams()
{
    //qDebug()<<"send";
    QByteArray tmp = audio_device_input->read(maxsize);
    socket->writeDatagram(tmp.data(), tmp.size(), remoteHOST, remotePORT);
}
