
#include <QApplication>
#include "dialog.h"
#include "calling.h"

int main(int argc, char *argv[])
{
    QApplication a(argc, argv);
    Dialog w;
    w.show();
    return a.exec();
}
