#include "menu.h"

#include <QApplication>

/*!
    \file
    \brief Главная функция приложения
*/

/*!
    \brief Функция, запускающая приложение, создающая объект класса и отображающая его.
*/
int main(int argc, char *argv[])
{
    QApplication a(argc, argv);
    Menu menu;
    menu.show();
    return a.exec();
}
