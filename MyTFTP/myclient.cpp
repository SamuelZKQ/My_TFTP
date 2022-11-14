#include "myclient.h"
#include "widget.h"

/* Constructor */
MyClient::MyClient()
{
    Mode = "octet";

    MyCode.BlockNum = 0;

    cBlocks = 0;
    cBuf = 0;
    lastBuf = 0;
    lastBufflag = FALSE;

    MyCode.Path = "D:/Courses File/Computer_Networks/Experiment/MyTFTP/RecvFile";

    MyTFTPClient = new QUdpSocket;

    FileSize = 0;
    MyEOF = FALSE;

    MyERRORCode = -1;
    ProcessTime = 0;

    connect(MyTFTPClient, SIGNAL(readyRead()), this, SLOT(RecvData()));
    connect(&MyTimer, SIGNAL(timeout()), this, SLOT(ResendDatagram()));

}

void MyClient::ResetTimer()
{
    TimeOutCount = 0;
    MyTimer.start(1000);
}

void MyClient::StopTimer()
{
    TimeOutCount = 0;
    MyTimer.stop();
}

void MyClient::Clear()
{
    MyEOF = FALSE;

    lastBuf = 0;
    lastBufflag = FALSE;

    cBlocks = 0;
    cBuf = 0;

    ProcessTime = 0;

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
    ResetTimer();

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
    ResetTimer();
    SendRequest(ServerPort);
}

void MyClient::RecordLog(quint8 flag)
{
    QTime time;
    switch(flag)
    {
    case DATA_RECEIVE:
        MessageLog.append("Receive DATA\t");
        MessageLog.append("Block:");
        MessageLog.append(QString::number(MyCode.BlockNum));
        MessageLog.append("\t");
        if(cBuf < 1024)
        {
            MessageLog.append(QString::number(cBuf));
            MessageLog.append("Byte received so far\t");
        }
        else if(cBuf < 1024 * 1024)
        {
            MessageLog.append(QString::number(cBuf/(1024.0)));
            MessageLog.append("KB received so far\t");
        }
        else
        {
            MessageLog.append(QString::number(cBuf/(1024*1024.0)));
            MessageLog.append("MB received so far\t");
        }
        MessageLog.append(time.currentTime().toString());
        emit LogWritten();
        MessageLog.append("\n");
        break;
    case ACK_RECEIVE:
        MessageLog.append("Receive ACK\t");
        MessageLog.append("Block:");
        MessageLog.append(QString::number(MyCode.BlockNum));
        MessageLog.append("\t");
        MessageLog.append(time.currentTime().toString());
        emit LogWritten();
        MessageLog.append("\n");
        break;
    case ERROR_RECEIVE:
        DealERRORMsg();
        break;
    case RRQ_SEND:
        MessageLog.append("Send RRQ\t");
        MessageLog.append("FileName:");
        MessageLog.append(MyCode.FileName);
        MessageLog.append("\t");
        MessageLog.append(time.currentTime().toString());
        emit LogWritten();
        MessageLog.append("\n");
        break;
    case WRQ_SEND:
        MessageLog.append("Send WRQ\t");
        MessageLog.append("FileName:");
        MessageLog.append(MyCode.FileName);
        MessageLog.append("\t");
        MessageLog.append(time.currentTime().toString());
        emit LogWritten();
        MessageLog.append("\n");
        break;
    case DATA_SEND:
        MessageLog.append("Send DATA\t");
        MessageLog.append("Block:");
        MessageLog.append(QString::number(MyCode.BlockNum));
        MessageLog.append("\t");
        if(cBuf < 1024)
        {
            MessageLog.append(QString::number(cBuf));
            MessageLog.append("Byte sent so far\t");
        }
        else if(cBuf < 1024 * 1024)
        {
            MessageLog.append(QString::number(cBuf/(1024.0)));
            MessageLog.append("KB sent so far\t");
        }
        else
        {
            MessageLog.append(QString::number(cBuf/(1024*1024.0)));
            MessageLog.append("MB sent so far\t");
        }
        MessageLog.append(time.currentTime().toString());
        emit LogWritten();
        MessageLog.append("\n");
        break;
    case ACK_SEND:
        MessageLog.append("Send ACK\t");
        MessageLog.append("Block:");
        MessageLog.append(QString::number(MyCode.BlockNum));
        MessageLog.append("\t");
        MessageLog.append(time.currentTime().toString());
        emit LogWritten();
        MessageLog.append("\n");
        break;
    case ERROR_SEND:
        break;
    case END_OF_READ:
        MessageLog.append("Read File Finished!\t");
        MessageLog.append(QString::number(cBuf/(1024.0)));
        MessageLog.append(" KB Received in ");
        MessageLog.append(QString::number(ProcessTime/1000.0));
        MessageLog.append(" s\tSpeed of Read:");
        MessageLog.append(QString::number((cBuf/(1024.0))/(ProcessTime/1000.0)) + "KB/s\n");
        emit LogWritten();
        break;
    case END_OF_WRITE:
        MessageLog.append("Write File Finished!\t");
        MessageLog.append(QString::number(cBuf/(1024.0)));
        MessageLog.append(" KB Sent in ");
        MessageLog.append(QString::number(ProcessTime/1000.0));
        MessageLog.append(" s\tSpeed of Read:");
        MessageLog.append(QString::number((cBuf/(1024.0))/(ProcessTime/1000.0)) + "KB/s\n");
        emit LogWritten();
        break;
    case RETRY_TOO_MUCH:
        MessageLog.append("Resend 10 times with no response!\t");
        MessageLog.append("Stop sending!");
        emit LogWritten();
    default:
        break;
    }
    OutputLog();
}

void MyClient::OutputLog()
{
    qDebug()<<"Output LOG！";
    QString MyLog = "D:/Courses File/Computer_Networks/Experiment/MyTFTP/Log";
    QString LogFileName = "ControlLog.log";
    QFile LogFile(MyLog + "/" + LogFileName);

    if(LogFile.open(QIODevice::WriteOnly | QIODevice::Append))
    {
        LogFile.write(MessageLog.toUtf8());
        qDebug()<<MessageLog;
        MessageLog.clear();
    }
    else
    {
        qDebug()<<"ERROR LOG";
    }
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
        MyTime.start();
        Datagram.clear();
        Datagram.append(zero);
        Datagram.append(RRQ);
        Datagram.append(MyCode.FileName);
        Datagram.append(zero);
        Datagram.append(Mode);
        Datagram.append(zero);
        RecordLog(RRQ_SEND);
        break;
    /* write request */
    case WRQ:
        MyTime.start();
        Datagram.clear();
        Datagram.append(zero);
        Datagram.append(WRQ);
        Datagram.append(MyCode.FileName);
        Datagram.append(zero);
        Datagram.append(Mode);
        Datagram.append(zero);
        RecordLog(WRQ_SEND);
        break;
    case DATA:
        SendFile(&Datagram);
        break;
    case ACK:
        Datagram.clear();
        Datagram.append(zero);
        Datagram.append(ACK);
        Datagram.append(high);
        Datagram.append(low);
        RecordLog(ACK_SEND);
        break;
    case ERROR:
        break;
    }

    /* Send Datagram to Server */
    qDebug()<<MyCode.BlockNum;
    qDebug()<<Datagram;
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

    cBuf = 512 * (MyCode.BlockNum - 1);

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
    RecordLog(DATA_SEND);
    if(i < 512)
    {        
        ProcessTime = MyTime.restart();
        MyEOF = TRUE;
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
            ResetTimer();
            RecvPacket(Packet, len);
            break;
        case ACK:
            ResetTimer();
            RecvACK(Packet);
            break;
        case ERROR:
            StopTimer();
            RecvERROR(Packet);
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
    quint64 i = 0;
    if(MyCode.BlockNum == cBlocks + 1)
        for(i = 0; i < len - 4; i++)
        {
            input << *(uchar *)packet++;
        }
    File->close();
    cBuf += i;
    RecordLog(DATA_RECEIVE);
    SendACK();
    if(len < 516)
    {
        StopTimer();
        ProcessTime = MyTime.restart();
        RecordLog(END_OF_READ);
        Clear();
        qDebug()<<"Here I am!";
        delete File;
    }

}

void MyClient::RecvACK(char *packet)
{
    quint16 Blocks = MAKEWORD((quint8)packet[3],(quint8)packet[2]);
    RecordLog(ACK_RECEIVE);
    if(MyCode.BlockNum == Blocks)
    {        
        lastBufflag = FALSE;
        MyCode.BlockNum++;
    }
    else
    {
        lastBufflag = TRUE;
    }
    MyCode.OpCode = DATA;
    if(MyEOF)
    {
        StopTimer();
        RecordLog(END_OF_WRITE);
        Clear();
        return;
    }
    SendRequest(PeerPort);
}

void MyClient::RecvERROR(char *packet)
{
    MyERRORCode = MAKEWORD((quint8)packet[3],(quint8)packet[2]);
    RecordLog(ERROR_RECEIVE);
}

void MyClient::DealERRORMsg()
{
    QTime time;
    MessageLog.append("Receive ERROR\t");
    MessageLog.append("ERROR Code:");
    switch(MyERRORCode)
    {
    case 0:
        MessageLog.append(QString::number(0));
        MessageLog.append("\t");
        MessageLog.append("Not defined, see error message\t");
        MessageLog.append(time.currentTime().toString());
        emit LogWritten();
        Clear();
        MessageLog.append("\n");
        break;
    case 1:
        MessageLog.append(QString::number(1));
        MessageLog.append("\t");
        MessageLog.append("File not found\t");
        MessageLog.append(time.currentTime().toString());
        emit LogWritten();
        MessageLog.append("\n");
        break;
    case 2:
        MessageLog.append(QString::number(2));
        MessageLog.append("\t");
        MessageLog.append("Access violation\t");
        MessageLog.append(time.currentTime().toString());
        emit LogWritten();
        MessageLog.append("\n");
        break;
    case 3:
        MessageLog.append(QString::number(3));
        MessageLog.append("\t");
        MessageLog.append("Disk full or allocation exceeded\t");
        MessageLog.append(time.currentTime().toString());
        emit LogWritten();
        MessageLog.append("\n");
        break;
    case 4:
        MessageLog.append(QString::number(4));
        MessageLog.append("\t");
        MessageLog.append("Illegal TFTP operation\t");
        MessageLog.append(time.currentTime().toString());
        emit LogWritten();
        MessageLog.append("\n");
        break;
    case 5:
        MessageLog.append(QString::number(5));
        MessageLog.append("\t");
        MessageLog.append("Unknown transfer ID\t");
        MessageLog.append(time.currentTime().toString());
        emit LogWritten();
        MessageLog.append("\n");
        break;
    case 6:
        MessageLog.append(QString::number(6));
        MessageLog.append("\t");
        MessageLog.append("File already exists\t");
        MessageLog.append(time.currentTime().toString());
        emit LogWritten();
        MessageLog.append("\n");
        break;
    case 7:
        MessageLog.append(QString::number(7));
        MessageLog.append("\t");
        MessageLog.append("No such user\t");
        MessageLog.append(time.currentTime().toString());
        emit LogWritten();
        MessageLog.append("\n");
        break;
    }
}

void MyClient::SendACK()
{
    MyCode.OpCode = ACK;
//    qDebug()<<MyCode.BlockNum<<' '<<cBlocks;
    if(MyCode.BlockNum == cBlocks + 1)
    {
        cBlocks = MyCode.BlockNum;
    }
    SendRequest(PeerPort);
}

void MyClient::ResendDatagram()
{
    SendRequest(ServerPort);
    TimeOutCount++;
    if(TimeOutCount >= MAX_RESEND_TIME)
    {
        StopTimer();
        Clear();
        RecordLog(RETRY_TOO_MUCH);
    }
}
