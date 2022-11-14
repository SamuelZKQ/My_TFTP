#include "widget.h"
#include "ui_widget.h"
#include "myclient.h"
#include <QFileDialog>

Widget::Widget(QWidget *parent)
    : QWidget(parent)
    , ui(new Ui::Widget)
{
    myclient = new MyClient();

    ui->setupUi(this);

    connect(myclient, SIGNAL(LogWritten()), this, SLOT(ShowMessage()));
}

Widget::~Widget()
{
    delete ui;
}

/* GetLastFileName
 * Path: the full path of target file
 * return: the file name */
QString Widget::GetLastFileName(QString Path)
{
    int cnt = Path.length();
    int i = Path.lastIndexOf("/");
    QString res = Path.right(cnt - i - 1);
    return res;
}

/* on_GetBtn_clicked
 * when the "Get" button is clicked
 * set target file, IP, port
 * send RRQ request */
void Widget::on_GetBtn_clicked()
{
    QString FilePath = QFileDialog::getOpenFileName(this, QStringLiteral("Select File"), "D:/Courses File/Computer_Networks/Experiment/Tftpd64/Datas");
    ui->FileLine->setText(FilePath);

    myclient->ServerIP = ui->IPLine->text();
    myclient->ServerPort = ui->PortLine->text().toUInt();
    myclient->MyCode.FileName = GetLastFileName(FilePath);
    myclient->MyCode.Path = "D:/Courses File/Computer_Networks/Experiment/MyTFTP/RecvFile";

    myclient->MyCode.OpCode = RRQ;

    myclient->CreateReadRequest();
}

/* on_GetBtn_clicked
 * when the "Send" button is clicked
 * set target file, IP, port
 * send WRQ request */
void Widget::on_SendBtn_clicked()
{
    QString FilePath = QFileDialog::getOpenFileName(this, QStringLiteral("Select File"), "D:/");
    ui->FileLine->setText(FilePath);

    myclient->ServerIP = ui->IPLine->text();
    myclient->ServerPort = ui->PortLine->text().toUInt();
    myclient->MyCode.FileName = GetLastFileName(FilePath);
    myclient->MyCode.Path = FilePath.left(FilePath.length() - myclient->MyCode.FileName.length() - 1);

    myclient->MyCode.OpCode = WRQ;

    myclient->CreateWriteRequest();
}


void Widget::ShowMessage()
{
    Message << myclient->MessageLog;
    QStringListModel* Model;

    Model = new QStringListModel(Message);
    ui->MessageList->setModel(Model);
}

void Widget::on_ChangeModeBtn_clicked()
{
    if(ui->ModeLabel->text()!="Current Mode: octet")
    {
        ui->ModeLabel->setText("Current Mode: octet");
        myclient->Mode = "octet";
    }
    else
    {
        ui->ModeLabel->setText("Current Mode: netascii");
        myclient->Mode = "netascii";
    }
}

void Widget::on_ClearBtn_clicked()
{
    Message.clear();
    QStringListModel* Model;
    Model = new QStringListModel(Message);
    ui->MessageList->setModel(Model);
}
