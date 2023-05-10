#ifndef MENU_H
#define MENU_H

#include "client.h"
#include <QWidget>
#include <QPushButton>
#include <QVBoxLayout>

/*!
    \file
    \brief Заголовочный файл с описанием класса

    Данный файл содержит в себе определение класса Menu: его поля и методы.
*/

/*!
    \brief Класс главного меню

    Данный класс является меню для клиента. В дальнейшем с его помощью будет легче добавить, например, регистрацию пользователей.
*/
class Menu : public QWidget {
    Q_OBJECT

    Client *client;
    QPushButton *openChatButton;
    QPushButton *exitButton;
    QVBoxLayout *layout;

public:
    explicit Menu(QWidget *parent = nullptr);
};

#endif // MENU_H
