#ifndef WIDGET_H
#define WIDGET_H

#include "myclient.h"
#include <QWidget>
#include <QUdpSocket>
#include <QNetworkDatagram>
#include <QHostAddress>

#define RRQ (quint8)1
#define WRQ (short)2
#define DATA_LEN 512

QT_BEGIN_NAMESPACE
namespace Ui { class Widget; }
QT_END_NAMESPACE

class Widget : public QWidget
{
    Q_OBJECT

public:
    Widget(QWidget *parent = nullptr);

    MyClient* myclient;

    QString GetLastFileName(QString Path);


    ~Widget();

private slots:
    void on_GetBtn_clicked();

    void on_SendBtn_clicked();

    void on_ShowDirBtn_clicked();

private:
    Ui::Widget *ui;
};
#endif // WIDGET_H
