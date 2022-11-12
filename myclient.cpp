#include "myclient.h"

/* Constructor */
MyClient::MyClient()
{
    Mode = "octet";

    cBlocks = 0;
    cBuf = 0;

    MyCode.Path = "D:/Courses File/Computer_Networks/Experiment/MyTFTP/RecvFile";

    MyTFTPClient = new QUdpSocket;

    FileSize = 0;
    MyEOF = FALSE;

    connect(MyTFTPClient, SIGNAL(readyRead()), this, SLOT(RecvData()));

}

void MyClient::Clear()
{
    MyEOF = FALSE;

    cBlocks = 0;
    cBuf = 0;

    MyCode.OpCode = 0;
    MyCode.BlockNum = 0;
}

/* CreateReadRequest
 * Create a new file
 * Call function SendRequest
 * Call function RecvData */
void MyClient::CreateReadRequest()
{
    /* Create a new file to receive data packets */
    File = new QFile(MyCode.Path + "/" + MyCode.FileName);
    qDebug()<<MyCode.Path + "/" + MyCode.FileName;
    if(File->exists())
    {
        qDebug()<<"Exist!";
        File->remove();
        File->setFileName(MyCode.Path + "/" + MyCode.FileName);
    }

    qDebug()<<"Send!";
    SendRequest(ServerPort);
}

/* CreateReadRequest
 * Create a new file
 * Call function SendRequest
 * Call function RecvData */
void MyClient::CreateWriteRequest()
{
    File = new QFile(MyCode.Path + "/" + MyCode.FileName);

    if(!File->exists())
    {
        qDebug()<<" Not Exist!";
        return;
    }
    FileSize = File->size();

    SendRequest(ServerPort);
}

/* SendRequest
 * Used to send request
 * req: operation code of request */
void MyClient::SendRequest(quint16 Port)
{
    quint8 zero = 0;
    quint8 high = HIBYTE(MyCode.BlockNum);
    quint8 low = LOBYTE(MyCode.BlockNum);

    switch(MyCode.OpCode)
    {
    /* read request */
    case RRQ:
        Datagram.clear();
        Datagram.append(zero);
        Datagram.append(RRQ);
        Datagram.append(MyCode.FileName);
        Datagram.append(zero);
        Datagram.append(Mode);
        Datagram.append(zero);
        break;
    /* write request */
    case WRQ:
        Datagram.clear();
        Datagram.append(zero);
        Datagram.append(WRQ);
        Datagram.append(MyCode.FileName);
        Datagram.append(zero);
        Datagram.append(Mode);
        Datagram.append(zero);
        break;
    case DATA:
        SendFile(&Datagram);
        break;
    case ACK:
        qDebug()<<"ACK Sent!";
        Datagram.clear();
        Datagram.append(zero);
        Datagram.append(ACK);
        Datagram.append(high);
        Datagram.append(low);
        break;
    case ERROR:
        break;
    }

    /* Send Datagram to Server */
    MyTFTPClient->writeDatagram(Datagram, Datagram.length(), QHostAddress(ServerIP), Port);
    qDebug()<<"Datagram Sent!";
}

void MyClient::SendFile(QByteArray *Datagram)
{
    quint8 zero = 0;
    quint8 high = HIBYTE(MyCode.BlockNum);
    quint8 low = LOBYTE(MyCode.BlockNum);

    Datagram->clear();
    Datagram->append(zero);
    Datagram->append(DATA);
    Datagram->append(high);
    Datagram->append(low);

    File->open(QIODevice::ReadOnly);
    File->seek(cBuf);

    QDataStream output(File);

    quint64 i;
    quint8 tmp;
    for(i = 0; i < 512 && i < FileSize - cBuf; i++)
    {
        output >> tmp;
        Datagram->append(tmp);
    }
    File->close();
    cBuf += i;
    if(i < 512)
    {
        MyEOF = TRUE;
        cBuf = 0;
        MyCode.BlockNum = 0;
    }
}

/* RecvData
 * Receive datagram from server */
void MyClient::RecvData()
{
    /* wait for the server to start */
    while(MyTFTPClient->hasPendingDatagrams())
    {
        qDebug()<<"has pending?";
        QByteArray RecvDatagram;
        QHostAddress ServerAddress;
        RecvDatagram.resize(MyTFTPClient->pendingDatagramSize());

        /* Receive packet from Server */
        MyTFTPClient->readDatagram(RecvDatagram.data(), RecvDatagram.size(), &ServerAddress, &PeerPort);

        char* Packet = (char*)RecvDatagram.data();
        quint16 OpCode = MAKEWORD((quint8)Packet[1], (quint8)Packet[0]);
        quint64 len = RecvDatagram.length();

        switch(OpCode)
        {
        case RRQ:
        case WRQ:
            break;
        case DATA:
            qDebug()<<"DATA Recvd!";
            RecvPacket(Packet, len);
            break;
        case ACK:
            RecvACK(Packet);
            break;
        case ERROR:
            qDebug()<<"ERROR";
            break;
        }

    }
}

void MyClient::RecvPacket(char* packet, quint64 len)
{
    qDebug()<<File->fileName();
    if(File->open(QIODevice::WriteOnly | QIODevice::Append))
    {
        qDebug()<<"Open Succeed";
    }
    else
    {
        qDebug()<<"Fuck!";
       return;
    }

    QDataStream input(File);
    MyCode.BlockNum = MAKEWORD((quint8)packet[3],(quint8)packet[2]);
    packet += 4;

    for(quint64 i = 0; i < len - 4; i++)
    {
        input << *(uchar *)packet++;
    }
    File->close();

    SendACK();
    qDebug()<<len;
    if(len < 516)
    {
        Clear();
        qDebug()<<"Here I am!";
        delete File;
    }

}

void MyClient::RecvACK(char *packet)
{
    quint16 Blocks = MAKEWORD((quint8)packet[3],(quint8)packet[2]);
    if(MyCode.BlockNum == Blocks)
    {
        MyCode.BlockNum++;
    }
    MyCode.OpCode = DATA;
    if(MyEOF)
    {
        return;
    }
    SendRequest(PeerPort);
}

void MyClient::SendACK()
{
    MyCode.OpCode = ACK;
    qDebug()<<MyCode.BlockNum<<' '<<cBlocks;
    if(MyCode.BlockNum == cBlocks + 1)
    {
        cBlocks = MyCode.BlockNum;

        SendRequest(PeerPort);
    }
}
