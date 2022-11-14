#ifndef WIDGET_H
#define WIDGET_H

#include "myclient.h"
#include <QWidget>
#include <QUdpSocket>
#include <QNetworkDatagram>
#include <QHostAddress>



QT_BEGIN_NAMESPACE
namespace Ui { class Widget; }
QT_END_NAMESPACE

class Widget : public QWidget
{
    Q_OBJECT

public:
    Widget(QWidget *parent = nullptr);

    MyClient* myclient;
    QStringList Message;

    QString GetLastFileName(QString Path);


    ~Widget();

private slots:
    void on_GetBtn_clicked();

    void on_SendBtn_clicked();

    void ShowMessage();

    void on_ChangeModeBtn_clicked();

    void on_ClearBtn_clicked();

private:
    Ui::Widget *ui;
};
#endif // WIDGET_H
