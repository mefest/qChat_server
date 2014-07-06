#include "dialog.h"
#include "ui_dialog.h"
#include <QTime>
#include <QDebug>
#include <QKeyEvent>
#include <QMenu>

Dialog::Dialog(QWidget *parent) :
    QDialog(parent),

    ui(new Ui::Dialog)
{
    ui->setupUi(this);
    closeid=0;
    this->setWindowFlags(Qt::WindowFullscreenButtonHint);
    //this->setWindowFlags(Qt::WindowCloseButtonHint);
    _serv = new server(this,this);
    encrypt=false;
    ctx= new BLOWFISH_CTX;
    slotInitPass();
    connect(ui->btn_start,SIGNAL(clicked()),this,SLOT(runServer()));
    connect(ui->btn_send,SIGNAL(clicked()),this,SLOT(slotSendMessae()));
    connect(ui->tabWidget,SIGNAL(tabCloseRequested(int)),this,SLOT(closeTab(int)));
    connect(_serv,SIGNAL(newRoom(QString)),this,SLOT(addNewRoom(QString)));
    ui->ln_addr->setVisible(false);
    createMenuUserList();
#ifdef Q_OS_WIN32
    tray = new QSystemTrayIcon(QIcon(":/icon/resource/icon/icon.ico"),this);
    // tray->setProperty("_sni_qt_category", "ApplicationStatus");
    tray->show();
    QMenu *Menu = new QMenu(this);
    QAction *showw = new QAction("Show", this);
    connect(showw, SIGNAL(triggered()), this, SLOT(showEvent()));
    QAction *quitAction = new QAction("Quit", this);
    connect(quitAction, SIGNAL(triggered()), this, SLOT(closeEvent2()));
    Menu->addAction(showw);
    Menu->addAction(quitAction);
    tray->setContextMenu(Menu);

#endif

}

Dialog::~Dialog()
{
    delete ui;
}

void Dialog::onAddUserToGui(QString name)
{
    //    ui->lwUsers->addItem(name);
    //    ui->lwLog->insertItem(0, QTime::currentTime().toString()+" "+name+" joined");
    //    ui->lwLog->item(0)->setTextColor(Qt::green);

    ui->lw_users->addItem(name);
    if(_serv->getClientCount()>0)
        ui->act_Kick->setEnabled(true);
}

void Dialog::runServer()
{
    if(!_serv->running)
    {
        if (_serv->doStartServer(QHostAddress::Any,ui->ln_port->text().toInt() ))
        {
            qDebug()<<"start server on "<<QHostAddress::Any<< " :"<<ui->ln_port->text().toInt();
            ui->btn_start->setText("stop");
        }
        else
        {
            qDebug()<<"ERROR:start server fail";
        }
    }
    else
    {
        _serv->close();
        _serv->closeAllConnection();
        ui->btn_start->setText("start");
        ui->act_Kick->setEnabled(false);
        _serv->running=false;

        qDebug()<<"server closed";
    }
}

void Dialog::slotSendMessae()
{
    onMessageToGui("admin",ui->te_inputMess->toPlainText());
    emit signalSendMessage("admin",ui->te_inputMess->toPlainText());
    ui->te_inputMess->clear();
}


void Dialog::onRemoveUserFromGui(QString name)
{
    for (int i = 0; i < ui->lw_users->count(); ++i)
        if (ui->lw_users->item(i)->text() == name)
        {
            ui->lw_users->takeItem(i);
            //            ui->lwLog->insertItem(0, QTime::currentTime().toString()+" "+name+" left");
            //            ui->lwLog->item(0)->setTextColor(Qt::green);
            break;
        }
    if(_serv->getClientCount()<1)
        ui->act_Kick->setEnabled(false);
}

void Dialog::onMessageToGui(QString name, QString mes)
{
    QTextEdit *output = ui->te_outputMess;
    output->moveCursor(QTextCursor::End);
    output->setTextColor(Qt::blue);
    output->insertPlainText(QTime::currentTime().toString());
    output->setTextColor(Qt::red);
    output->insertPlainText("<"+name+">");
    output->setTextColor(Qt::black);
    output->insertPlainText(mes+"\n");
}

void Dialog::slotInitPass()
{
    if(ui->ln_passwd->text()!="" && ui->ln_passwd->text().length()>6)
    {
        QString key=ui->ln_passwd->text();
        Blowfish_Init (ctx, &key, key.length());
        encrypt=true;
        qDebug()<<"Шифрование включенно";
    }
    else
        encrypt=false;
    emit changePasswd(ctx,&encrypt);
    qDebug()<<"Изменения пароля"<<encrypt;
}

void Dialog::closeEvent2()
{
    closeid=2;
    this->close();
}

void Dialog::showEvent()
{
    this->show();
}

void Dialog::createMenuUserList()
{
    menuUserList = new QMenu(this);
    menuUserList->addAction(ui->act_Kick);
    menuUserList->addAction(ui->actCall);
}

void Dialog::closeTab(int index)
{
    if (ui->tabWidget->tabText(index)!="general")
        ui->tabWidget->removeTab(index);
}

void Dialog::keyPressEvent(QKeyEvent *event)
{
    if ( event->key() == Qt::Key_Return) {
        if(event->modifiers() & Qt::CTRL){
            qDebug()<<"Hажата ентер";
            onMessageToGui("admin",ui->te_inputMess->toPlainText());
            emit signalSendMessage("admin",ui->te_inputMess->toPlainText());
            ui->te_inputMess->clear();
        }
    }
}

void Dialog::closeEvent(QCloseEvent *eClose)
{
#ifdef Q_OS_WIN32
    if(closeid!=2){
        this->hide();
        eClose->ignore();
    } else {
        _serv->stopServer();
        eClose->accept();
    }
#endif
}



void Dialog::on_lw_users_customContextMenuRequested(const QPoint &pos)
{
    menuUserList->exec(ui->lw_users->mapToGlobal(pos));
}

void Dialog::on_act_Kick_triggered()
{
    if(ui->lw_users->count()>0)
        _serv->kick(ui->lw_users->selectedItems().at(0)->text());
}

void Dialog::on_actCall_triggered()
{

}

void Dialog::on_btn_start_clicked()
{

}

void Dialog::addNewRoom(QString roomName)
{
    ui->listRoomWidget->addItem(roomName);
}
