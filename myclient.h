#ifndef MYCLIENT_H
#define MYCLIENT_H

#include <QTime>
#include <QUdpSocket>
#include <QByteArray>
#include <QFile>
#include <winsock2.h>
#include <QtNetwork>
#include <QDataStream>
#include <QThread>
#include <QMetaObject>
#include <QIODevice>

#define RRQ     (quint8)1
#define WRQ     (quint8)2
#define DATA    (quint8)3
#define ACK     (quint8)4
#define ERROR   (quint8)5

struct Code
{
    quint64 OpCode;
    QString Path;
    QString FileName;
    quint16 BlockNum;
};

class MyClient:public QThread
{
    Q_OBJECT

public:
    QUdpSocket *MyTFTPClient;
    QByteArray Datagram;

    QString ServerIP;
    quint64 ServerPort;
    quint16 PeerPort;

    QString FileName;
    QString Mode;
    QFile* File;
    quint64 FileSize;

    quint16 cBlocks;
    quint64 cBuf;

    Code MyCode;
    bool MyEOF;

    QTimer MyTimer;

    MyClient();
    void Clear();

    void CreateReadRequest();
    void CreateWriteRequest();
    void SendRequest(quint16 Port);
    void SendFile(QByteArray* Datagram);
    void RecvACK(char* packet);
    void RecvPacket(char* packet, quint64 len);
    void SendACK();

private slots:
    void RecvData();

};

#endif // MYCLIENT_H
