#ifndef DIALOG_H
#define DIALOG_H


#include <QDialog>

#include "server.h"
//#include "blowfish.h"
namespace Ui {
class Dialog;
}


class Dialog : public QDialog
{
    Q_OBJECT
    

    signals:


    void signalSendMessage(QString name, QString mess);
    void changePasswd(BLOWFISH_CTX *ctx,bool * encrypt);
public:
    explicit Dialog(QWidget *parent = 0);
    BLOWFISH_CTX *ctx;
    bool encrypt;

    ~Dialog();

    public slots:

    void onAddUserToGui(QString name);
    void onRemoveUserFromGui(QString name);
    void onMessageToGui(QString name,QString mes);
    void runServer();
    void slotSendMessae();
    void slotInitPass();




protected:

  virtual void 	keyPressEvent ( QKeyEvent * event );

private slots:


private:
    Ui::Dialog *ui;
    server *_serv;
};

#endif // DIALOG_H
