#include "client.h"
#include <QIntValidator>

/*!
 * \brief Конструктор класса
 * \param parent - возможный родитель объекта.
*/
Client::Client(QWidget *parent) : QWidget(parent) {
    layout = new QVBoxLayout(this);
    menuBar = new QMenuBar();

    menuFile = new QMenu(tr("File"), this);
    menuBar->addMenu(menuFile);
    menuFile->addAction(tr("Connect"), this, SLOT(connect()));
    menuFile->addAction(tr("Disconnect"), this, SLOT(disconnect()));
    menuFile->addAction(tr("Exit"), this, SLOT(exit()));

    menuSettings = new QMenu(tr("Settings"), this);
    menuBar->addMenu(menuSettings);
    menuSettings->addAction(tr("Server"), this, [&](){settingsServer->open();});
    menuSettings->addAction(tr("Name"), this, [&](){settingsProfile->open();});

    menuView = new QMenu(tr("View"), this);
    menuBar->addMenu(menuView);
    menuView->addAction(tr("Background color"), this, SLOT(setBackgroundColor()));

    clientMessage = new QLineEdit;
    reviewer = new QTextEdit;
    send = new QPushButton(tr("Send"));
    sendLayout = new QHBoxLayout;
    sendLayout->addWidget(clientMessage);
    sendLayout->addWidget(send);

    layout->setMenuBar(menuBar);
    layout->addWidget(reviewer);
    layout->addLayout(sendLayout);

    setLayout(layout);
    setWindowTitle("disconnected");
    resize(600, 800);

    // dialogs
    settingsProfile = new QDialog();
    settingsServer = new QDialog();
    wrongAddressDialog = new QDialog();
    wrongNameDialog = new QDialog();

    // user dialog
    nameLabel = new QLabel(tr("Name:"));
    nameEdit = new QLineEdit();
    setNameButton = new QPushButton(tr("Ok"));
    closeNameDialogButton = new QPushButton(tr("Close"));

    // user dialog layouts
    lineNameEdit = new QHBoxLayout;
    lineNameButtons = new QHBoxLayout;
    settingsProfileLayout = new QVBoxLayout;
    lineNameEdit->addWidget(nameLabel);
    lineNameEdit->addWidget(nameEdit);
    lineNameButtons->addWidget(setNameButton);
    lineNameButtons->addWidget(closeNameDialogButton);
    settingsProfileLayout->addLayout(lineNameEdit);
    settingsProfileLayout->addLayout(lineNameButtons);
    settingsProfile->setLayout(settingsProfileLayout);
    settingsProfile->setWindowTitle("Profile");

    // server dialog
    ipLabel = new QLabel(tr("IP:"));
    ipEdit = new QLineEdit();
    portLabel = new QLabel(tr("Port:"));
    portEdit = new QLineEdit();
    portEdit->setValidator(new QIntValidator(1, 65535, this));
    setServerButton = new QPushButton(tr("Ok"));
    closeServerDialogButton = new QPushButton(tr("Close"));

    // server dialog layouts
    lineIpEdit = new QHBoxLayout;
    linePortEdit = new QHBoxLayout;
    lineServerButtons = new QHBoxLayout;
    settingsServerLayout = new QVBoxLayout;
    lineIpEdit->addWidget(ipLabel);
    lineIpEdit->addWidget(ipEdit);
    linePortEdit->addWidget(portLabel);
    linePortEdit->addWidget(portEdit);
    lineServerButtons->addWidget(setServerButton);
    lineServerButtons->addWidget(closeServerDialogButton);
    settingsServerLayout->addLayout(lineIpEdit);
    settingsServerLayout->addLayout(linePortEdit);
    settingsServerLayout->addLayout(lineServerButtons);
    settingsServer->setLayout(settingsServerLayout);
    settingsServer->setWindowTitle("Network");

    // wrong ip or port dialog
    wrongAddressMessage = new QLabel(tr("Empty IP or port! Go to \"Settings\" -> \"Server\""));
    wrongAddressButton = new QPushButton(tr("OK"));

    // wrong ip or port dialog layout
    wrongAddressLayout = new QVBoxLayout;
    wrongAddressLayout->addWidget(wrongAddressMessage);
    wrongAddressLayout->addWidget(wrongAddressButton);
    wrongAddressDialog->setLayout(wrongAddressLayout);
    wrongAddressDialog->setWindowTitle("Attention!");

    // wrong name dialog
    wrongNameMessage = new QLabel(tr("Empty name! Go to \"Settings\" -> \"Name\""));
    wrongNameButton = new QPushButton(tr("OK"));

    // wrong name dialog layout
    wrongNameLayout = new QVBoxLayout;
    wrongNameLayout->addWidget(wrongNameMessage);
    wrongNameLayout->addWidget(wrongNameButton);
    wrongNameDialog->setLayout(wrongNameLayout);
    wrongNameDialog->setWindowTitle("Attention!");

    QObject::connect(setNameButton, &QPushButton::clicked, this, &Client::setName);
    QObject::connect(closeNameDialogButton, &QPushButton::clicked, this, &Client::closeProfileDialog);
    QObject::connect(setServerButton, &QPushButton::clicked, this, &Client::setServer);
    QObject::connect(closeServerDialogButton, &QPushButton::clicked, this, &Client::closeServerDialog);
    QObject::connect(wrongAddressButton, &QPushButton::clicked, this, [&](){wrongAddressDialog->close();});
    QObject::connect(wrongNameButton, &QPushButton::clicked, this, [&](){wrongNameDialog->close();});
    QObject::connect(send, &QPushButton::clicked, this, &Client::sendMessage);

    socket = new QTcpSocket;
    reviewer->setReadOnly(true);
}

/*!
    \brief Метод для получения состояния клиента
    Клиент может быть подключен или не подключен к серверу.
*/
QString Client::getConnectionState() {
    QString cur_status;
    if (connection)
        cur_status = "connected";
    else
        cur_status = "disconnected";
    return cur_status;
}

/*!
    \brief Метод для подключения к серверу
    В случае пустого имени, порта или IP покажется диалоговое окно с предупреждением. В случае, если все эти поля заполнены выполнится подключение.
    При подключении отправляется код 100, длина имени и само имя. В случае, если подключение не выполнено, статус в названии окна будет об этом говорить.
*/
void Client::connect() {
    if (!(serverAddress.isEmpty() || serverPort == 0 || name.isEmpty())) {
        socket->connectToHost(serverAddress, serverPort);
        setWindowTitle(serverAddress + ' ' + QString::number(serverPort) + " connecting...");
        if (socket->waitForConnected(3000)) {
            unsigned int length = name.size();
            QByteArray data;
            data.append(1, 0);
            data.append(1, 100);
            data.append(1, (length >> 8) & 0xFF);
            data.append(1, length & 0xFF);
            data.append(name.toUtf8());
            socket->write(data);
            for (size_t i = 0; i < 4 + length; i++)
                socket->waitForBytesWritten(100);
            QObject::connect(socket, &QIODevice::readyRead, this, &Client::read);
            connection = true;
            setWindowTitle(serverAddress + ' ' + QString::number(serverPort));
        }
        else {
            connection = false;
            setWindowTitle(serverAddress + ' ' + QString::number(serverPort) + ' ' + getConnectionState());
        }
    }
    else if (name.isEmpty()) {
        wrongNameDialog->show();
    }
    else {
        wrongAddressDialog->show();
    }
}

/*!
    \brief Метод для отключения от сервера
    При отключении на сервер отправляется код 150, длина имени и имя.
*/
void Client::disconnect() {
    unsigned int length = name.size();
    QByteArray data;
    data.append(1, 0);
    data.append(1, (unsigned char)150);
    data.append(1, (length >> 8) & 0xFF);
    data.append(1, length & 0xFF);
    data.append(name.toUtf8());
    socket->write(data);
    socket->waitForBytesWritten();
    socket->disconnectFromHost();
    connection = false;
    setWindowTitle(serverAddress + ' ' + QString::number(serverPort) + ' ' + getConnectionState());
}

/*!
    \brief Метод для чтения сообщений
    При получении сообщения от сервера клиенту приходит код 250, длина имени, имя, длина сообщения и само сообщение,
    которое вместе с именем в квадратных скобках отображается в QTextEdit'оре.
*/
void Client::read() {
    QByteArray data = socket->readAll();
    if (data.size() >= 4) {
        if ((unsigned char)data.at(1) == 250) {
            int n = (unsigned char)data.at(3) | (data.at(2) << 8);
            int ind = 4;
            for (int i = 0; i < n; i++) {
                QString cur_name = "";
                int length = (unsigned char)data.at(ind + 1) | (data.at(ind) << 8);
                ind += 2;
                for (int i = ind; i < ind + length; i++)
                    cur_name += data[i];
                ind += length;
                QString cur_message = "";
                length = (unsigned char)data.at(ind + 1) | (data.at(ind) << 8);
                ind += 2;
                for (int i = ind; i < ind + length; i++)
                    cur_message += data[i];
                ind += length;
                reviewer->append('[' + cur_name + "] " + cur_message);
            }
        }
    }
}

/*!
    \brief Метод закрытия окна
    При закрытии окна клиент отключается от сервера.
*/
void Client::exit() {
    disconnect();
    close();
}

/*!
    \brief Метод для указания сервера
    Введённые пользователем данные считываются и устанавливается название окна, состоящее из адреса сервера, порта и статуса подключения. После этого диалоговое окно закрывается.
*/
void Client::setServer() {
    serverAddress = ipEdit->text();
    serverPort = portEdit->text().toInt();
    setWindowTitle(serverAddress + ' ' + QString::number(serverPort) + " disconnected");
    closeServerDialog();
}

/*!
    \brief Метод для указания имени пользователя
    Введённые пользователем данные считываются и диалоговое окно закрывается.
*/
void Client::setName() {
    name = nameEdit->text();
    closeProfileDialog();
}

/*!
    \brief Закрытие диалогового окна для указания имени пользователя
*/
void Client::closeProfileDialog() {
    settingsProfile->close();
}

/*!
    \brief Закрытие диалогового окна для указания IP-адреса и порта сервера
*/
void Client::closeServerDialog() {
    settingsServer->close();
}

/*!
    \brief Метод для установки фона чата
*/
void Client::setBackgroundColor() {
    backgroundColorDialog = new QColorDialog();
    backgroundColorDialog->open();
    QColor color = QColorDialog::getColor();
    if (!color.isValid())
        return;
    setPalette(color);
    setAutoFillBackground(true);
    backgroundColorDialog->close();
}

/*!
    \brief Метод для отправки сообщения
    Сначала отправляется код 200, потом длина имени, имя, длина сообщения и само сообщение. Поле ввода текста очищается.
*/
void Client::sendMessage() {
    QString message = clientMessage->text();
    unsigned int length = name.size();
    QByteArray data;
    data.append(1, 0);
    data.append(1, (unsigned char)200);
    data.append(1, (length >> 8) & 0xFF);
    data.append(1, length & 0xFF);
    data.append(name.toUtf8());
    length = message.size();
    data.append(1, (length >> 8) & 0xFF);
    data.append(1, length & 0xFF);
    data.append(message.toUtf8());
    socket->write(data);
    socket->waitForBytesWritten();
    clientMessage->clear();
}
