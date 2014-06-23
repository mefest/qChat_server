#ifndef DIALOG_H
#define DIALOG_H


#include <QDialog>

#include "server.h"
#include <QSystemTrayIcon>


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
    QSystemTrayIcon *tray;
    QMenu *menuUserList;

    ~Dialog();

    public slots:

    void onAddUserToGui(QString name);
    void onRemoveUserFromGui(QString name);
    void onMessageToGui(QString name,QString mes);
    void runServer();
    void slotSendMessae();
    void slotInitPass();

    void closeEvent2();
    void showEvent();
    void createMenuUserList();
    void closeTab(int index);




protected:

  virtual void 	keyPressEvent ( QKeyEvent * event );

private slots:


    void on_lw_users_customContextMenuRequested(const QPoint &pos);

    void on_act_Kick_triggered();

    void on_actCall_triggered();

    void on_btn_start_clicked();

    void addNewRoom(QString);

private:
    Ui::Dialog *ui;
    server *_serv;

    void closeEvent(QCloseEvent *eClose);
    int closeid=0;
};

#endif // DIALOG_H
