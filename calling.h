#ifndef CALLING_H
#define CALLING_H

#include <QDialog>

namespace Ui {
class calling;
}

class calling : public QDialog
{
    Q_OBJECT

public:
    explicit calling(QWidget *parent = 0);
    ~calling();

private:
    Ui::calling *ui;
};

#endif // CALLING_H
