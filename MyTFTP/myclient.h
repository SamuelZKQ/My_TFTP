#ifndef MYCLIENT_H
#define MYCLIENT_H

#include <QFile>
#include <QTime>
#include <QThread>
#include <QtNetwork>
#include <QIODevice>
#include <QUdpSocket>
#include <QByteArray>
#include <QDataStream>
#include <QTextStream>
#include <QMetaObject>
#include <winsock2.h>

#define RRQ     (quint8)1
#define WRQ     (quint8)2
#define DATA    (quint8)3
#define ACK     (quint8)4
#define ERROR   (quint8)5
#define DATA_RECEIVE    (quint8)1
#define ACK_RECEIVE     (quint8)2
#define ERROR_RECEIVE   (quint8)3
#define RRQ_SEND        (quint8)4
#define WRQ_SEND        (quint8)5
#define DATA_SEND       (quint8)6
#define ACK_SEND        (quint8)7
#define ERROR_SEND      (quint8)8
#define END_OF_READ     (quint8)9
#define END_OF_WRITE    (quint8)10
#define RETRY_TOO_MUCH  (quint8)11

#define MAX_RESEND_TIME (quint8)10


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

    quint64 lastBuf;
    bool lastBufflag;
    Code MyCode;
    bool MyEOF;

    quint16 MyERRORCode;

    QTime MyTime;
    QTimer MyTimer;
    quint64 TimeOutCount;

    quint64 ProcessTime;

    QString MessageLog;

    MyClient();
    void Clear();

    void RecordLog(quint8 flag);

    void CreateReadRequest();
    void CreateWriteRequest();
    void SendRequest(quint16 Port);
    void SendFile(QByteArray* Datagram);
    void RecvACK(char* packet);
    void RecvPacket(char* packet, quint64 len);
    void SendACK();
    void RecvERROR(char* packet);
    void DealERRORMsg();

    void ResetTimer();
    void StopTimer();

    void OutputLog();

signals:
    void LogWritten();

private slots:
    void RecvData();
    void ResendDatagram();

};

#endif // MYCLIENT_H
