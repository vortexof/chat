#include "menu.h"

Menu::Menu(QWidget *parent) : QWidget{parent} {
    client = new Client;
    openChatButton = new QPushButton(tr("Open chat"));
    exitButton = new QPushButton(tr("Exit"));
    layout = new QVBoxLayout;
    layout->addWidget(openChatButton);
    layout->addWidget(exitButton);
    setLayout(layout);
    resize(220, 130);
    connect(openChatButton, &QPushButton::clicked, this, [&](){client->show();});
    connect(exitButton, &QPushButton::clicked, this, [&]() {
        client->disconnect();
        client->close();
        close();
    });
    setWindowTitle("Menu");
}
