#ifndef SERVER_H
#define SERVER_H

#include <QWidget>
#include <QMenu>
#include <QMenuBar>
#include <QVBoxLayout>
#include <QTcpServer>
#include <QTcpSocket>
#include <QTimer>
#include <QNetworkInterface>
#include <QQueue>

/*!
\file
\brief Заголовочный файл с описанием класса

Данный файл содержит в себе определение класса Server: его поля и методы.
*/


/*!
    \brief Класс сервера

    Данный класс реализовывает сервер чата, умеющий принимать сообщения от клиентов и отправлять их. Сервер контролирует подключения и отключения клиентов.
    Сверху, в качестве названия окна, отображается IP, порт и количество подключенных клиентов.
*/
class Server : public QWidget {
    Q_OBJECT

    QMenu *menuFile;
    QMenuBar *menuBar;
    QVBoxLayout *layout;
    QTcpServer *server;
    QTcpSocket *socket;
    QString ipAddress;
    QVector<QPair<QString, QTcpSocket*>> users;

    //! Структура сообщения, хранящая имя отправившего сообщение пользователя и текст этого сообщения.
    struct Message {
        QString name = "";
        QString text = "";
    };

    QQueue<Message> messages;  //! < Очередь объектов структуры Message.

    void initServer();

public:
    Server(QWidget *parent = nullptr);

private slots:
    void exit();
    void read();
    void newConnection();
    void disconnected();
    void sendMessages();
};
#endif // SERVER_H
