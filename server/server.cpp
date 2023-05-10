/*!
\file
\brief Файл с реализацией методов класса Server

Данный файл содержит в себе реализацию конструкторов класса и методы, необходимые для инициализации сервера, отправки и приёма сообщений, подключения и отключения клиентов и т.д.
*/

#include "server.h"

/*!
Констурктор класса
\param parent - возможный родитель данного объекта.
*/
Server::Server(QWidget *parent) : QWidget(parent) {
    menuFile = new QMenu(tr("File"), this);
    menuBar = new QMenuBar;
    menuBar->addMenu(menuFile);
    menuFile->addAction(tr("Exit"), this, SLOT(exit()));

    layout = new QVBoxLayout(this);
    layout->setMenuBar(menuBar);
    setLayout(layout);

    socket = new QTcpSocket();
    initServer();
    resize(450, 150);
}

/*!
Метод, инициализирующий сервер. После выполнения данного метода метода у сервера появляется IP и порт.
*/
void Server::initServer() {
    server = new QTcpServer(this);
    connect(server, &QTcpServer::newConnection, this, &Server::newConnection);
    if (!server->listen()) {
        return;
    }
    QList<QHostAddress> ipAddressesList = QNetworkInterface::allAddresses();
    for (int i = 0; i < ipAddressesList.size(); ++i) {
        if (ipAddressesList.at(i) != QHostAddress::LocalHost && ipAddressesList.at(i).toIPv4Address()) {
            ipAddress = ipAddressesList.at(i).toString();
            break;
        }
    }
    if (ipAddress.isEmpty())
        ipAddress = QHostAddress(QHostAddress::LocalHost).toString();
    setWindowTitle(ipAddress + ' ' + QString::number(server->serverPort()) + ' ' + QString::number(users.size()));
}

/*!
Метод, отвечающий за отправку сообщений.

Сначала отправляется 250 (код, отвечающий за отправку сообщений в данном приложении), потом количество отправляемых сообщений
(сделано с целью возможности изменения реализации с меньшим изменением кода клиента), а потом и само сообщение.
Для хранения сообщений используется очередь, хранящая неотправленные сообщения.
*/
void Server::sendMessages() {
    int messages_num = messages.size();
    for (int i = 0; i < messages_num; i++) {
        for (int num = 0; num < users.size(); num++) {
            socket = users[num].second;
            QByteArray data;
            data.append(1, 0);
            data.append(1, (unsigned char)250);
            data.append(1, (messages_num >> 8) & 0xFF);
            data.append(1, messages_num & 0xFF);
            Message current_message = messages.head();
            int n_length = current_message.name.size();
            int m_length = current_message.text.size();
            data.append(1, (n_length >> 8) & 0xFF);
            data.append(1, n_length & 0xFF);
            data.append(current_message.name.toUtf8());
            data.append(1, (m_length >> 8) & 0xFF);
            data.append(1, m_length & 0xFF);
            data.append(current_message.text.toUtf8());
            socket->write(data);
            socket->waitForBytesWritten();
        }
        messages.dequeue();
    }
}

/*!
Метод, отвечающий за подключение новых клиентов.

При подключении пользователь добавляется в вектор, хранящий пары из имени пользователя и его сокета.
*/
void Server::newConnection() {
    socket = server->nextPendingConnection();
    users.push_back(qMakePair(QString(""), socket));
    connect(socket, &QIODevice::readyRead, this, &Server::read);
    connect(socket, SIGNAL(disconnected()), this, SLOT(disconnected()));
    setWindowTitle(ipAddress + ' ' + QString::number(server->serverPort()) + ' ' + QString::number(users.size()));
}


/*!
Метод, отвечающий за принятие сообщений от клиентов.

При подключении от клиента приходит сигнал, состоящий из имени и кода, именно этот сигнал и считывается в первую очередь.
Код 100 означает, что подключился новый клиент. Программа изменяет имя последнего пользователя в векторе, сделав его равным считанному значению.
Код 150 означает, что клиент отключился. Он будет удалён из вектора.
Код 200 означает, что от клиента пришло сообщение. Оно считывается и добавляется в очередь, Как только сообщение пришло на сервер, клиентам отправляется это сообщение.
*/
void Server::read() {
    socket = static_cast<QTcpSocket*>(sender());
    QByteArray data = socket->readAll();
    setWindowTitle(ipAddress + ' ' + QString::number(server->serverPort()) + ' ' + QString::number(users.size()));
    if (data.size() >= 2) {
        if (data.at(1) == 100 || (unsigned char)data.at(1) == 150 || (unsigned char)data.at(1) == 200) {
            QString name = "";
            int length = (unsigned char)data.at(3) | (data.at(2) << 8);
            for (int i = 4; i < 4 + length; i++)
                name += data[i];
            if (data.at(1) == 100)
                users[users.size() - 1].first = name;
            else if ((unsigned char)data.at(1) == 150) {
                int ind = 0;
                while (users[ind].first != name)
                    ind++;
                users.remove(ind);
            }
            else {
                QString message = "";
                int m_length = (unsigned char)data.at(5 + length) | (data.at(4 + length) << 8);
                for (int i = 6 + length; i < 6 + length + m_length; i++)
                    message += data[i];
                messages.enqueue(Message{name, message});
                sendMessages();
            }
        }
    }
}


/*!
Метод, отвечающий за отключение клиента.

В случае отключения клиента количество подключенных клиентов в названии окна становится равным новому числу клиентов, то есть, размеру вектора users.
*/
void Server::disconnected() {
    setWindowTitle(ipAddress + ' ' + QString::number(server->serverPort()) + ' ' + QString::number(users.size()));
}


/*!
Метод, отвечающий за закрытие окна.

При закрытии окна сервера все пользователи отсоединяются от сервера, после чего происходит выход из приложения.
*/
void Server::exit() {
    for (int i = 0; i < users.size(); i++)
        users[i].second->disconnect();
    close();
}
