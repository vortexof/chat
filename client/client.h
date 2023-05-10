#ifndef CLIENT_H
#define CLIENT_H

#include <QWidget>
#include <QTcpSocket>
#include <QMenu>
#include <QMenuBar>
#include <QVBoxLayout>
#include <QTcpSocket>
#include <QLabel>
#include <QDialog>
#include <QLineEdit>
#include <QTextEdit>
#include <QPushButton>
#include <QHBoxLayout>
#include <QColorDialog>

/*!
    \file
    \brief Заголовочный файл с описанием класса

    Данный файл содержит в себе определение класса Client: его поля и методы.
*/

/*!
    \brief Класс клиента

    Данный класс является клиентом чата. Клиент может передавать сообщения серверу с целью общения с другими клиентами.
*/

class Client : public QWidget {
    Q_OBJECT

    QMenu *menuFile;
    QMenu *menuSettings;
    QMenu *menuView;
    QVBoxLayout *layout;
    QMenuBar *menuBar;

    // Элементы диалогового окна о неверном IP или порте
    QLabel *wrongAddressMessage;
    QPushButton *wrongAddressButton;
    QVBoxLayout *wrongAddressLayout;

    // Элементы диалогового окна о неверном имени пользователя
    QLabel *wrongNameMessage;
    QPushButton *wrongNameButton;
    QVBoxLayout *wrongNameLayout;

    // Элементы диалогового окна для задания имени пользователя
    QLabel *nameLabel;
    QLineEdit *nameEdit;
    QPushButton *setNameButton;
    QPushButton *closeNameDialogButton;

    // Необходимые для размещения layout'ы
    QHBoxLayout *lineNameEdit;
    QHBoxLayout *lineNameButtons;
    QVBoxLayout *settingsProfileLayout;

    // Элементы диалогового окна для задания IP-адреса и пользователя
    QLabel *ipLabel;
    QLineEdit *ipEdit;
    QLabel *portLabel;
    QLineEdit *portEdit;
    QPushButton *setServerButton;
    QPushButton *closeServerDialogButton;

    QHBoxLayout *lineIpEdit;
    QHBoxLayout *linePortEdit;
    QHBoxLayout *lineServerButtons;
    QVBoxLayout *settingsServerLayout;

    QColorDialog *backgroundColorDialog;

    QLineEdit *clientMessage;  // отправка сообщения
    QTextEdit *reviewer;  // отображение сообщений
    QPushButton *send;
    QHBoxLayout *sendLayout;

    QTcpSocket *socket;

    bool connection = false;
    QString serverAddress = "";
    int serverPort = 0;
    QString name = "";

public:
    Client(QWidget *parent = nullptr);
    QDialog *settingsProfile;  //!< Диалоговое окно для указания имени пользователя.
    QDialog *settingsServer;  //!< Диалоговое окно для указания сервера подключения.
    QDialog *wrongAddressDialog;  //!< Диалоговое окно с информацией о неправильном указании IP или порта.
    QDialog *wrongNameDialog;  //!< Диалоговое окно с информацией о неправильном указании имени пользователя.

private slots:
    void connect();
    void exit();
    void setServer();
    void setName();
    void read();
    void closeProfileDialog();
    void closeServerDialog();
    void setBackgroundColor();
    void sendMessage();
    QString getConnectionState();

public slots:
    void disconnect();
};
#endif // CLIENT_H
