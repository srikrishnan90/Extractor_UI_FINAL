#include "mainwindow.h"
#include "ui_mainwindow.h"

#include <QPushButton>
static int opt=0;
static int from_edit=0;
static QString array[80]={""};
static int array_len=0;
static int timer_val=0;
static int sub_time_loop=0;
static int into_pname=0;
static int proc_countdown=0;
static int proc1_countdown=0;
static int total_time=0;
static int hold_base=0;
static int hold_magnet=0;
static int hold_sleeve=0;
static int clicks=0;

MainWindow::MainWindow(QWidget *parent) :
    QMainWindow(parent),
    ui(new Ui::MainWindow)
{
    ui->setupUi(this);
    setWindowFlags(Qt::FramelessWindowHint);

    QSqlDatabase sqdb = QSqlDatabase::addDatabase("QSQLITE");
    sqdb.setDatabaseName("/home/pi/support/ext.db");
    if(!sqdb.open())
    {
        //qDebug() << "Can't Connect to DB !";
        ui->label_4->setText("DB Error");
    }
    else
    {
        // qDebug() << "Connected Successfully to DB !";
        ui->label_4->setText("Initialization Successful");
    }
    ui->stackedWidget->setCurrentIndex(6);
    ui->label_93->hide();
    ui->label_94->hide();
    ui->toolButton->hide();
    ui->toolButton_2->hide();
    ui->toolButton_8->hide();
    ui->label_91->setVisible(true);
    ui->label_91->setText("ALTA NUCLEIC ACID EXTACTOR");
    QSqlQuery query;
    //query.prepare("SELECT name FROM sqlite_master WHERE type='table'");
    query.prepare("SELECT name FROM DNA");
    query.exec();
    //int i=0;
    ui->listWidget->clear();
    while(query.next())
    {
        //qDebug()<<query.value(0).toString();
        //qDebug()<<i;
        //i++;
        //if(i!=0)
        ui->listWidget->addItem(query.value(0).toString());
        //i++;
    }
    ui->listWidget->setCurrentRow(0);
    //ui->listWidget->setVerticalScrollBar(ui->verticalScrollBar);
    //ui->listWidget->setStyleSheet("QScrollBar:vertical { width: 60px; }");
    ui->listWidget->verticalScrollBar()->setStyleSheet("QScrollBar:vertical { width: 50px; border:5px solid black;}");
    //QScrollBar::handle{background : pink;}");
    int fd = wiringPiI2CSetup(DEVICE_ID);
    qDebug()<<fd<<DEVICE_ID;
    if (fd == -1) {
        qDebug() << "Failed to init I2C communication.\n";
        //return -1;
    }
    //qDebug() << "I2C communication successfully setup.\n";

    timer = new QTimer(this);
    timer1 = new QTimer(this);
    timer2 = new QTimer(this);
    timer3 = new QTimer(this);
    proctimer = new QTimer(this);
    proctimer1 = new QTimer(this);
    mtesttimer = new QTimer(this);
    mtesttimer1 = new QTimer(this);
    connect(timer, SIGNAL(timeout()), this,SLOT(testing()));
    connect(timer1, SIGNAL(timeout()), this,SLOT(realtime_temperature()));
    connect(timer2, SIGNAL(timeout()), this,SLOT(init_motor()));
    connect(timer3, SIGNAL(timeout()), this,SLOT(init()));
    connect(proctimer, SIGNAL(timeout()), this,SLOT(proc_timer()));
    connect(proctimer1, SIGNAL(timeout()), this,SLOT(proc_timer1()));
    connect(mtesttimer, SIGNAL(timeout()), this,SLOT(motor_test()));
    connect(mtesttimer1, SIGNAL(timeout()), this,SLOT(motor_test1()));


    timer2->start(500);
    QMovie *movie = new QMovie("/home/pi/support/animation.gif");
    ui->label_18->setMovie(movie);
    movie->start();

    ui->label_35->hide();

}

MainWindow::~MainWindow()
{
    delete ui;
}


void MainWindow::on_toolButton_9_clicked()
{
    clicks+=1;
    if(clicks==3)
        qApp->exit();
}

void MainWindow::on_toolButton_3_clicked()
{
    ui->stackedWidget->setCurrentIndex(1);
    ui->lineEdit->clear();
    ui->comboBox->clear();
    QSqlQuery query;
    //query.prepare("SELECT name FROM sqlite_master WHERE type='table'");
    query.prepare("SELECT name FROM DNA");
    query.exec();
    //int i=0;
    ui->comboBox->addItem("None");
    while(query.next())
    {
        //qDebug()<<query.value(0).toString();
        //qDebug()<<i;
        //i++;
        //if(i!=0)
        ui->comboBox->addItem(query.value(0).toString());
        //i++;
    }
}

void MainWindow::on_pushButton_110_clicked()
{
    opt=1;
    ui->stackedWidget->setCurrentIndex(5);
    ui->stackedWidget_2->setCurrentIndex(0);
    ui->lineEdit_145->setText(ui->lineEdit->text());

}

void MainWindow::on_toolButton_25_clicked()
{
    ui->toolButton->hide();
    ui->toolButton_2->hide();
    ui->toolButton_8->hide();
    ui->label_91->setVisible(true);
    ui->label_91->setText(ui->lineEdit->text());
    QMessageBox msg;
    QString temp, name;

    temp=ui->comboBox->currentText();
    name=ui->lineEdit->text();

    if(temp!="None")
    {
        QSqlQuery query;
        query.prepare("select pname1,pname2,pname3,pname4,pname5,pname6,pname7 FROM DNA where name=:name");
        query.bindValue(":name",temp);
        query.exec();
        while(query.next())
        {
            ui->lineEdit_3->setText(query.value(0).toString());
            ui->lineEdit_14->setText(query.value(1).toString());
            ui->lineEdit_20->setText(query.value(2).toString());
            ui->lineEdit_26->setText(query.value(3).toString());
            ui->lineEdit_32->setText(query.value(4).toString());
            ui->lineEdit_38->setText(query.value(5).toString());
            ui->lineEdit_44->setText(query.value(6).toString());
        }

        query.prepare("select well1,well2,well3,well4,well5,well6,well7 FROM DNA where name=:name");
        query.bindValue(":name",temp);
        query.exec();
        while(query.next())
        {
            ui->comboBox_3->setCurrentIndex(query.value(0).toInt()-1);
            ui->comboBox_8->setCurrentIndex(query.value(1).toInt()-1);
            ui->comboBox_10->setCurrentIndex(query.value(2).toInt()-1);
            ui->comboBox_12->setCurrentIndex(query.value(3).toInt()-1);
            ui->comboBox_14->setCurrentIndex(query.value(4).toInt()-1);
            ui->comboBox_16->setCurrentIndex(query.value(5).toInt()-1);
            ui->comboBox_18->setCurrentIndex(query.value(6).toInt()-1);
        }

        query.prepare("select wait1,wait2,wait3,wait4,wait5,wait6,wait7 FROM DNA where name=:name");
        query.bindValue(":name",temp);
        query.exec();
        while(query.next())
        {
            ui->lineEdit_4->setText(query.value(0).toString());
            ui->lineEdit_12->setText(query.value(1).toString());
            ui->lineEdit_18->setText(query.value(2).toString());
            ui->lineEdit_24->setText(query.value(3).toString());
            ui->lineEdit_30->setText(query.value(4).toString());
            ui->lineEdit_36->setText(query.value(5).toString());
            ui->lineEdit_42->setText(query.value(6).toString());
        }

        query.prepare("select mix1,mix2,mix3,mix4,mix5,mix6,mix7 FROM DNA where name=:name");
        query.bindValue(":name",temp);
        query.exec();
        while(query.next())
        {
            ui->lineEdit_8->setText(query.value(0).toString());
            ui->lineEdit_13->setText(query.value(1).toString());
            ui->lineEdit_19->setText(query.value(2).toString());
            ui->lineEdit_25->setText(query.value(3).toString());
            ui->lineEdit_31->setText(query.value(4).toString());
            ui->lineEdit_37->setText(query.value(5).toString());
            ui->lineEdit_43->setText(query.value(6).toString());
        }
        query.prepare("select mag1,mag2,mag3,mag4,mag5,mag6,mag7 FROM DNA where name=:name");
        query.bindValue(":name",temp);
        query.exec();
        while(query.next())
        {
            ui->lineEdit_9->setText(query.value(0).toString());
            ui->lineEdit_15->setText(query.value(1).toString());
            ui->lineEdit_21->setText(query.value(2).toString());
            ui->lineEdit_27->setText(query.value(3).toString());
            ui->lineEdit_33->setText(query.value(4).toString());
            ui->lineEdit_39->setText(query.value(5).toString());
            ui->lineEdit_45->setText(query.value(6).toString());
        }
        query.prepare("select vol1,vol2,vol3,vol4,vol5,vol6,vol7 FROM DNA where name=:name");
        query.bindValue(":name",temp);
        query.exec();
        while(query.next())
        {
            ui->lineEdit_10->setText(query.value(0).toString());
            ui->lineEdit_16->setText(query.value(1).toString());
            ui->lineEdit_22->setText(query.value(2).toString());
            ui->lineEdit_28->setText(query.value(3).toString());
            ui->lineEdit_34->setText(query.value(4).toString());
            ui->lineEdit_40->setText(query.value(5).toString());
            ui->lineEdit_46->setText(query.value(6).toString());
        }
        query.prepare("select spd1,spd2,spd3,spd4,spd5,spd6,spd7 FROM DNA where name=:name");
        query.bindValue(":name",temp);
        query.exec();
        while(query.next())
        {
            ui->comboBox_6->setCurrentIndex(query.value(0).toInt()-1);
            ui->comboBox_7->setCurrentIndex(query.value(1).toInt()-1);
            ui->comboBox_9->setCurrentIndex(query.value(2).toInt()-1);
            ui->comboBox_11->setCurrentIndex(query.value(3).toInt()-1);
            ui->comboBox_13->setCurrentIndex(query.value(4).toInt()-1);
            ui->comboBox_15->setCurrentIndex(query.value(5).toInt()-1);
            ui->comboBox_17->setCurrentIndex(query.value(6).toInt()-1);
        }
        query.prepare("select temp1,temp2,temp3,temp4,temp5,temp6,temp7 FROM DNA where name=:name");
        query.bindValue(":name",temp);
        query.exec();
        while(query.next())
        {
            ui->lineEdit_11->setText(query.value(0).toString());
            ui->lineEdit_17->setText(query.value(1).toString());
            ui->lineEdit_23->setText(query.value(2).toString());
            ui->lineEdit_29->setText(query.value(3).toString());
            ui->lineEdit_35->setText(query.value(4).toString());
            ui->lineEdit_41->setText(query.value(5).toString());
            ui->lineEdit_47->setText(query.value(6).toString());
        }
        query.prepare("select temp11,temp21,temp31,temp41,temp51,temp61,temp71 FROM DNA where name=:name");
        query.bindValue(":name",temp);
        query.exec();
        while(query.next())
        {
            ui->comboBox_21->setCurrentIndex(query.value(0).toInt());
            ui->comboBox_22->setCurrentIndex(query.value(1).toInt());
            ui->comboBox_36->setCurrentIndex(query.value(2).toInt());
            ui->comboBox_37->setCurrentIndex(query.value(3).toInt());
            ui->comboBox_38->setCurrentIndex(query.value(4).toInt());
            ui->comboBox_39->setCurrentIndex(query.value(5).toInt());
            ui->comboBox_40->setCurrentIndex(query.value(6).toInt());
        }


    }
    else
    {
        ui->lineEdit_3->clear();
        ui->lineEdit_14->clear();
        ui->lineEdit_20->clear();
        ui->lineEdit_26->clear();
        ui->lineEdit_32->clear();
        ui->lineEdit_38->clear();
        ui->lineEdit_44->clear();

        ui->lineEdit_4->clear();
        ui->lineEdit_12->clear();
        ui->lineEdit_18->clear();
        ui->lineEdit_24->clear();
        ui->lineEdit_30->clear();
        ui->lineEdit_36->clear();
        ui->lineEdit_42->clear();

        ui->lineEdit_8->clear();
        ui->lineEdit_13->clear();
        ui->lineEdit_19->clear();
        ui->lineEdit_25->clear();
        ui->lineEdit_31->clear();
        ui->lineEdit_37->clear();
        ui->lineEdit_43->clear();

        ui->lineEdit_9->clear();
        ui->lineEdit_15->clear();
        ui->lineEdit_21->clear();
        ui->lineEdit_27->clear();
        ui->lineEdit_33->clear();
        ui->lineEdit_39->clear();
        ui->lineEdit_45->clear();

        ui->lineEdit_10->clear();
        ui->lineEdit_16->clear();
        ui->lineEdit_22->clear();
        ui->lineEdit_28->clear();
        ui->lineEdit_34->clear();
        ui->lineEdit_40->clear();
        ui->lineEdit_46->clear();

        ui->lineEdit_11->clear();
        ui->lineEdit_17->clear();
        ui->lineEdit_23->clear();
        ui->lineEdit_29->clear();
        ui->lineEdit_35->clear();
        ui->lineEdit_41->clear();
        ui->lineEdit_47->clear();

    }


    QByteArray bytes = name.toUtf8();
    int length = bytes.size();
    //qDebug()<<"Length = "<<length;
    if(length>0)
    {
        int clen=ui->comboBox->count();
        int cmpstat=0;
        for(int i=0;i<clen;i++)
        {
            if(name==ui->comboBox->itemText(i))
                cmpstat=1;
        }
        if(cmpstat==1)
        {

            msg.setText("Experiment Already Exists");
            msg.setWindowTitle("Warning");
            msg.setStyleSheet("QLabel{min-width:500 px; font-size: 24px;} QPushButton{ width:200px; height:50px; font-size: 18px; }");
            msg.exec();
        }
        else
            ui->stackedWidget->setCurrentIndex(2);
    }

    else {
        msg.setText("Empty Experiment Name");
        msg.setWindowTitle("Warning");
        msg.setStyleSheet("QLabel{min-width:500 px; font-size: 24px;} QPushButton{ width:200px; height:50px; font-size: 18px; }");
        msg.exec();
    }



}

void MainWindow::on_toolButton_16_clicked()
{
    ui->toolButton->setVisible(true);
    ui->toolButton_2->setVisible(true);
    ui->toolButton_8->setVisible(true);
    ui->label_91->hide();
    ui->stackedWidget->setCurrentIndex(0);
    ui->listWidget->setCurrentRow(0);
    QString name,p1name,p2name,p3name,p4name,p5name,p6name,p7name;
    int p1pos,p1wait,p1mix,p1mag,p1vol,p1spd,p1temp,p11temp;
    int p2pos,p2wait,p2mix,p2mag,p2vol,p2spd,p2temp,p21temp;
    int p3pos,p3wait,p3mix,p3mag,p3vol,p3spd,p3temp,p31temp;
    int p4pos,p4wait,p4mix,p4mag,p4vol,p4spd,p4temp,p41temp;
    int p5pos,p5wait,p5mix,p5mag,p5vol,p5spd,p5temp,p51temp;
    int p6pos,p6wait,p6mix,p6mag,p6vol,p6spd,p6temp,p61temp;
    int p7pos,p7wait,p7mix,p7mag,p7vol,p7spd,p7temp,p71temp;

    name=ui->lineEdit->text();
    p1name=ui->lineEdit_3->text();
    p2name=ui->lineEdit_14->text();
    p3name=ui->lineEdit_20->text();
    p4name=ui->lineEdit_26->text();
    p5name=ui->lineEdit_32->text();
    p6name=ui->lineEdit_38->text();
    p7name=ui->lineEdit_44->text();

    p1pos=ui->comboBox_3->currentText().toInt();
    p2pos=ui->comboBox_8->currentText().toInt();
    p3pos=ui->comboBox_10->currentText().toInt();
    p4pos=ui->comboBox_12->currentText().toInt();
    p5pos=ui->comboBox_14->currentText().toInt();
    p6pos=ui->comboBox_16->currentText().toInt();
    p7pos=ui->comboBox_18->currentText().toInt();

    p1wait=ui->lineEdit_4->text().toInt();
    p2wait=ui->lineEdit_12->text().toInt();
    p3wait=ui->lineEdit_18->text().toInt();
    p4wait=ui->lineEdit_24->text().toInt();
    p5wait=ui->lineEdit_30->text().toInt();
    p6wait=ui->lineEdit_36->text().toInt();
    p7wait=ui->lineEdit_42->text().toInt();

    p1mix=ui->lineEdit_8->text().toInt();
    p2mix=ui->lineEdit_13->text().toInt();
    p3mix=ui->lineEdit_19->text().toInt();
    p4mix=ui->lineEdit_25->text().toInt();
    p5mix=ui->lineEdit_31->text().toInt();
    p6mix=ui->lineEdit_37->text().toInt();
    p7mix=ui->lineEdit_43->text().toInt();

    p1mag=ui->lineEdit_9->text().toInt();
    p2mag=ui->lineEdit_15->text().toInt();
    p3mag=ui->lineEdit_21->text().toInt();
    p4mag=ui->lineEdit_27->text().toInt();
    p5mag=ui->lineEdit_33->text().toInt();
    p6mag=ui->lineEdit_39->text().toInt();
    p7mag=ui->lineEdit_45->text().toInt();

    p1vol=ui->lineEdit_10->text().toInt();
    p2vol=ui->lineEdit_16->text().toInt();
    p3vol=ui->lineEdit_22->text().toInt();
    p4vol=ui->lineEdit_28->text().toInt();
    p5vol=ui->lineEdit_34->text().toInt();
    p6vol=ui->lineEdit_40->text().toInt();
    p7vol=ui->lineEdit_46->text().toInt();

    p1spd=ui->comboBox_6->currentText().toInt();
    p2spd=ui->comboBox_7->currentText().toInt();
    p3spd=ui->comboBox_9->currentText().toInt();
    p4spd=ui->comboBox_11->currentText().toInt();
    p5spd=ui->comboBox_13->currentText().toInt();
    p6spd=ui->comboBox_15->currentText().toInt();
    p7spd=ui->comboBox_17->currentText().toInt();

    p1temp=ui->lineEdit_11->text().toInt();
    p2temp=ui->lineEdit_17->text().toInt();
    p3temp=ui->lineEdit_23->text().toInt();
    p4temp=ui->lineEdit_29->text().toInt();
    p5temp=ui->lineEdit_35->text().toInt();
    p6temp=ui->lineEdit_41->text().toInt();
    p7temp=ui->lineEdit_47->text().toInt();

    p11temp=ui->comboBox_21->currentIndex();
    p21temp=ui->comboBox_22->currentIndex();
    p31temp=ui->comboBox_36->currentIndex();
    p41temp=ui->comboBox_37->currentIndex();
    p51temp=ui->comboBox_38->currentIndex();
    p61temp=ui->comboBox_39->currentIndex();
    p71temp=ui->comboBox_40->currentIndex();
    qDebug()<<p11temp<<p21temp<<p31temp;


    QSqlQuery query;
    if(from_edit==0)
    {
        query.prepare("insert into DNA(name,pname1,pname2,pname3,pname4,pname5,pname6,pname7) values(:name,:pname1,:pname2,:pname3,:pname4,:pname5,:pname6,:pname7)");
        query.bindValue(":name",name);
        query.bindValue(":pname1",p1name);
        query.bindValue(":pname2",p2name);
        query.bindValue(":pname3",p3name);
        query.bindValue(":pname4",p4name);
        query.bindValue(":pname5",p5name);
        query.bindValue(":pname6",p6name);
        query.bindValue(":pname7",p7name);
        query.exec();
    }
    else
    {
        query.prepare("update DNA set pname1=:pname1,pname2=:pname2,pname3=:pname3,pname4=:pname4,pname5=:pname5,pname6=:pname6,pname7=:pname7 where name=:name");
        query.bindValue(":name",name);
        query.bindValue(":pname1",p1name);
        query.bindValue(":pname2",p2name);
        query.bindValue(":pname3",p3name);
        query.bindValue(":pname4",p4name);
        query.bindValue(":pname5",p5name);
        query.bindValue(":pname6",p6name);
        query.bindValue(":pname7",p7name);
        query.exec();
        from_edit=0;
    }



    query.prepare("update DNA set well1=:p1pos,well2=:p2pos,well3=:p3pos,well4=:p4pos,well5=:p5pos,well6=:p6pos,well7=:p7pos where name=:name");
    query.bindValue(":p1pos",p1pos);
    query.bindValue(":p2pos",p2pos);
    query.bindValue(":p3pos",p3pos);
    query.bindValue(":p4pos",p4pos);
    query.bindValue(":p5pos",p5pos);
    query.bindValue(":p6pos",p6pos);
    query.bindValue(":p7pos",p7pos);
    query.bindValue(":name",name);
    query.exec();

    query.prepare("update DNA set wait1=:p1wait,wait2=:p2wait,wait3=:p3wait,wait4=:p4wait,wait5=:p5wait,wait6=:p6wait,wait7=:p7wait where name=:name");
    query.bindValue(":p1wait",p1wait);
    query.bindValue(":p2wait",p2wait);
    query.bindValue(":p3wait",p3wait);
    query.bindValue(":p4wait",p4wait);
    query.bindValue(":p5wait",p5wait);
    query.bindValue(":p6wait",p6wait);
    query.bindValue(":p7wait",p7wait);
    query.bindValue(":name",name);
    query.exec();

    query.prepare("update DNA set mix1=:p1mix,mix2=:p2mix,mix3=:p3mix,mix4=:p4mix,mix5=:p5mix,mix6=:p6mix,mix7=:p7mix where name=:name");
    query.bindValue(":p1mix",p1mix);
    query.bindValue(":p2mix",p2mix);
    query.bindValue(":p3mix",p3mix);
    query.bindValue(":p4mix",p4mix);
    query.bindValue(":p5mix",p5mix);
    query.bindValue(":p6mix",p6mix);
    query.bindValue(":p7mix",p7mix);
    query.bindValue(":name",name);
    query.exec();

    query.prepare("update DNA set mag1=:p1mag,mag2=:p2mag,mag3=:p3mag,mag4=:p4mag,mag5=:p5mag,mag6=:p6mag,mag7=:p7mag where name=:name");
    query.bindValue(":p1mag",p1mag);
    query.bindValue(":p2mag",p2mag);
    query.bindValue(":p3mag",p3mag);
    query.bindValue(":p4mag",p4mag);
    query.bindValue(":p5mag",p5mag);
    query.bindValue(":p6mag",p6mag);
    query.bindValue(":p7mag",p7mag);
    query.bindValue(":name",name);
    query.exec();

    query.prepare("update DNA set vol1=:p1vol,vol2=:p2vol,vol3=:p3vol,vol4=:p4vol,vol5=:p5vol,vol6=:p6vol,vol7=:p7vol where name=:name");
    query.bindValue(":p1vol",p1vol);
    query.bindValue(":p2vol",p2vol);
    query.bindValue(":p3vol",p3vol);
    query.bindValue(":p4vol",p4vol);
    query.bindValue(":p5vol",p5vol);
    query.bindValue(":p6vol",p6vol);
    query.bindValue(":p7vol",p7vol);
    query.bindValue(":name",name);
    query.exec();

    query.prepare("update DNA set spd1=:p1spd,spd2=:p2spd,spd3=:p3spd,spd4=:p4spd,spd5=:p5spd,spd6=:p6spd,spd7=:p7spd where name=:name");
    query.bindValue(":p1spd",p1spd);
    query.bindValue(":p2spd",p2spd);
    query.bindValue(":p3spd",p3spd);
    query.bindValue(":p4spd",p4spd);
    query.bindValue(":p5spd",p5spd);
    query.bindValue(":p6spd",p6spd);
    query.bindValue(":p7spd",p7spd);
    query.bindValue(":name",name);
    query.exec();

    query.prepare("update DNA set temp1=:p1temp,temp2=:p2temp,temp3=:p3temp,temp4=:p4temp,temp5=:p5temp,temp6=:p6temp,temp7=:p7temp where name=:name");
    query.bindValue(":p1temp",p1temp);
    query.bindValue(":p2temp",p2temp);
    query.bindValue(":p3temp",p3temp);
    query.bindValue(":p4temp",p4temp);
    query.bindValue(":p5temp",p5temp);
    query.bindValue(":p6temp",p6temp);
    query.bindValue(":p7temp",p7temp);
    query.bindValue(":name",name);
    query.exec();

    query.prepare("update DNA set temp11=:p11temp,temp21=:p21temp,temp31=:p31temp,temp41=:p41temp,temp51=:p51temp,temp61=:p61temp,temp71=:p71temp where name=:name");
    query.bindValue(":p11temp",p11temp);
    query.bindValue(":p21temp",p21temp);
    query.bindValue(":p31temp",p31temp);
    query.bindValue(":p41temp",p41temp);
    query.bindValue(":p51temp",p51temp);
    query.bindValue(":p61temp",p61temp);
    query.bindValue(":p71temp",p71temp);
    query.bindValue(":name",name);
    query.exec();



    query.prepare("SELECT name FROM DNA");
    query.exec();
    ui->listWidget->clear();
    while(query.next())
    {
        ui->listWidget->addItem(query.value(0).toString());
    }
    ui->listWidget->setCurrentRow(0);


}

void MainWindow::on_toolButton_15_clicked()
{
    ui->stackedWidget->setCurrentIndex(0);
    ui->listWidget->setCurrentRow(0);
    ui->toolButton->setVisible(true);
    ui->toolButton_2->setVisible(true);
    ui->toolButton_8->setVisible(true);
    ui->label_91->hide();
}

void MainWindow::on_toolButton_6_clicked()
{
    ui->stackedWidget->setCurrentIndex(3);
    ui->lineEdit_146->clear();
    ui->lineEdit_147->clear();
    ui->lineEdit_148->clear();
    ui->label_14->clear();
    ui->lineEdit_146->setStyleSheet("QLineEdit{background-color:rgb(255, 255, 255);}");
    ui->lineEdit_147->setStyleSheet("QLineEdit{background-color:rgb(255, 255, 255);}");
    ui->lineEdit_148->setStyleSheet("QLineEdit{background-color:rgb(255, 255, 255);}");
    ui->label_18->hide();
    ui->toolButton_2->hide();
    ui->toolButton_8->hide();
    ui->label_91->setVisible(true);
    //const QString& s = ui->listWidget->currentItem()->text();
    //ui->label_91->setText(s);
    QString name;
    name=ui->listWidget->currentItem()->text();
    ui->label_91->setText(name);
    //qDebug()<<selitem;
    QSqlQuery query;
    //step 1
    query.prepare("select pname1,well1,wait1,mix1,mag1,vol1,spd1,temp1,temp11 FROM DNA where name=:name");
    query.bindValue(":name",name);
    query.exec();
    QString processname1;
    int pos1=0,wait1=0,mix1=0,mag1=0,vol1=0,spd1=0,temp1=0,temp11=0;
    while(query.next())
    {
        processname1=query.value(0).toString();
        pos1=query.value(1).toInt();
        wait1=query.value(2).toInt();
        mix1=query.value(3).toInt();
        mag1=query.value(4).toInt();
        vol1=query.value(5).toInt();
        spd1=query.value(6).toInt();
        temp1=query.value(7).toInt();
        temp11=query.value(8).toInt();
    }
    //step 2
    query.prepare("select pname2,well2,wait2,mix2,mag2,vol2,spd2,temp2,temp21 FROM DNA where name=:name");
    query.bindValue(":name",name);
    query.exec();
    QString processname2;
    int pos2=0,wait2=0,mix2=0,mag2=0,vol2=0,spd2=0,temp2=0,temp21=0;
    while(query.next())
    {
        processname2=query.value(0).toString();
        pos2=query.value(1).toInt();
        wait2=query.value(2).toInt();
        mix2=query.value(3).toInt();
        mag2=query.value(4).toInt();
        vol2=query.value(5).toInt();
        spd2=query.value(6).toInt();
        temp2=query.value(7).toInt();
        temp21=query.value(8).toInt();
    }
    //step 3
    query.prepare("select pname3,well3,wait3,mix3,mag3,vol3,spd3,temp3,temp31 FROM DNA where name=:name");
    query.bindValue(":name",name);
    query.exec();
    QString processname3;
    int pos3=0,wait3=0,mix3=0,mag3=0,vol3=0,spd3=0,temp3=0,temp31=0;
    while(query.next())
    {
        processname3=query.value(0).toString();
        pos3=query.value(1).toInt();
        wait3=query.value(2).toInt();
        mix3=query.value(3).toInt();
        mag3=query.value(4).toInt();
        vol3=query.value(5).toInt();
        spd3=query.value(6).toInt();
        temp3=query.value(7).toInt();
        temp31=query.value(8).toInt();
    }
    //step 4
    query.prepare("select pname4,well4,wait4,mix4,mag4,vol4,spd4,temp4,temp41 FROM DNA where name=:name");
    query.bindValue(":name",name);
    query.exec();
    QString processname4;
    int pos4=0,wait4=0,mix4=0,mag4=0,vol4=0,spd4=0,temp4=0,temp41=0;
    while(query.next())
    {
        processname4=query.value(0).toString();
        pos4=query.value(1).toInt();
        wait4=query.value(2).toInt();
        mix4=query.value(3).toInt();
        mag4=query.value(4).toInt();
        vol4=query.value(5).toInt();
        spd4=query.value(6).toInt();
        temp4=query.value(7).toInt();
        temp41=query.value(8).toInt();
    }
    //step 5
    query.prepare("select pname5,well5,wait5,mix5,mag5,vol5,spd5,temp5,temp51 FROM DNA where name=:name");
    query.bindValue(":name",name);
    query.exec();
    QString processname5;
    int pos5=0,wait5=0,mix5=0,mag5=0,vol5=0,spd5=0,temp5=0,temp51=0;
    while(query.next())
    {
        processname5=query.value(0).toString();
        pos5=query.value(1).toInt();
        wait5=query.value(2).toInt();
        mix5=query.value(3).toInt();
        mag5=query.value(4).toInt();
        vol5=query.value(5).toInt();
        spd5=query.value(6).toInt();
        temp5=query.value(7).toInt();
        temp51=query.value(8).toInt();
    }
    //step 6
    query.prepare("select pname6,well6,wait6,mix6,mag6,vol6,spd6,temp6,temp61 FROM DNA where name=:name");
    query.bindValue(":name",name);
    query.exec();
    QString processname6;
    int pos6=0,wait6=0,mix6=0,mag6=0,vol6=0,spd6=0,temp6=0,temp61=0;
    while(query.next())
    {
        processname6=query.value(0).toString();
        pos6=query.value(1).toInt();
        wait6=query.value(2).toInt();
        mix6=query.value(3).toInt();
        mag6=query.value(4).toInt();
        vol6=query.value(5).toInt();
        spd6=query.value(6).toInt();
        temp6=query.value(7).toInt();
        temp61=query.value(8).toInt();
    }
    //step 7
    query.prepare("select pname7,well7,wait7,mix7,mag7,vol7,spd7,temp7,temp71 FROM DNA where name=:name");
    query.bindValue(":name",name);
    query.exec();
    QString processname7;
    int pos7=0,wait7=0,mix7=0,mag7=0,vol7=0,spd7=0,temp7=0,temp71=0;
    while(query.next())
    {
        processname7=query.value(0).toString();
        pos7=query.value(1).toInt();
        wait7=query.value(2).toInt();
        mix7=query.value(3).toInt();
        mag7=query.value(4).toInt();
        vol7=query.value(5).toInt();
        spd7=query.value(6).toInt();
        temp7=query.value(7).toInt();
        temp71=query.value(8).toInt();
    }

    int cur_pos=1;
    int mov_pos=0;
    QString d="";
    QString s="";

    array[0]=processname1;
    array[1]="mhmm";
    array[2]="shm";
    array[3]="bhm";

    //step 1
    s = QString::number(temp1);


    if(temp11==1)
        array[4]="W lys "+s;
    else if(temp11==2)
        array[4]="W elu "+s;
    else
        array[4]="W end";


    if(pos1>cur_pos)
    {
        mov_pos=pos1-cur_pos;
        s = QString::number(mov_pos);
        array[5]="bmf "+s;
        cur_pos=pos1;
    }
    else if(pos1<cur_pos)
    {
        mov_pos=cur_pos-pos1;
        s = QString::number(mov_pos);
        array[5]="bmb "+s;
        cur_pos=pos1;
    }
    else
    {
        array[5]="bmb 0";
    }
    array[6]="smf 1";
    s = QString::number(wait1);
    array[7]="idl "+s;
    s = QString::number(mix1);
    d = QString::number(spd1);
    if(vol1>=150)
        array[8]="mix "+s+" "+d;
    else
        array[8]="mis "+s+" "+d;
    array[9]="mmf 1";
    s = QString::number(mag1);
    array[10]="mag "+s;
    array[11]="msb 1";

    //step 2
    array[12]=processname2;
    s = QString::number(temp2);


    if(temp21==1)
        array[13]="W lys "+s;
    else if(temp21==2)
        array[13]="W elu "+s;
    else
        array[13]="W end";



    if(pos2>cur_pos)
    {
        mov_pos=pos2-cur_pos;
        s = QString::number(mov_pos);
        array[14]="bmf "+s;
        cur_pos=pos2;
    }
    else if(pos2<cur_pos)
    {
        mov_pos=cur_pos-pos2;
        s = QString::number(mov_pos);
        array[14]="bmb "+s;
        cur_pos=pos2;
    }
    else
    {
        array[14]="bmb 0";
    }
    array[15]="msf 1";
    array[16]="mhm";
    s = QString::number(wait2);
    array[17]="idl "+s;
    s = QString::number(mix2);
    d = QString::number(spd2);
    if(vol2>=150)
        array[18]="mix "+s+" "+d;
    else
        array[18]="mis "+s+" "+d;
    array[19]="mmf 1";
    s = QString::number(mag2);
    array[20]="mag "+s;
    array[21]="msb 1";


    //step 3
    array[22]=processname3;
    s = QString::number(temp3);


    if(temp31==1)
        array[23]="W lys "+s;
    else if(temp31==2)
        array[23]="W elu "+s;
    else
        array[23]="W end";


    if(pos3>cur_pos)
    {
        mov_pos=pos3-cur_pos;
        s = QString::number(mov_pos);
        array[24]="bmf "+s;
        cur_pos=pos3;
    }
    else if(pos3<cur_pos)
    {
        mov_pos=cur_pos-pos3;
        s = QString::number(mov_pos);
        array[24]="bmb "+s;
        cur_pos=pos3;
    }
    else
    {
        array[24]="bmb 0";
    }
    array[25]="msf 1";
    array[26]="mhm";
    s = QString::number(wait3);
    array[27]="idl "+s;
    s = QString::number(mix3);
    d = QString::number(spd3);
    if(vol3>=150)
        array[28]="mix "+s+" "+d;
    else
        array[28]="mis "+s+" "+d;
    array[29]="mmf 1";
    s = QString::number(mag3);
    array[30]="mag "+s;
    array[31]="msb 1";


    //step 4
    array[32]=processname4;
    s = QString::number(temp4);


    if(temp41==1)
        array[33]="W lys "+s;
    else if(temp41==2)
        array[33]="W elu "+s;
    else
        array[33]="W end";


    if(pos4>cur_pos)
    {
        mov_pos=pos4-cur_pos;
        s = QString::number(mov_pos);
        array[34]="bmf "+s;
        cur_pos=pos4;
    }
    else if(pos4<cur_pos)
    {
        mov_pos=cur_pos-pos4;
        s = QString::number(mov_pos);
        array[34]="bmb "+s;
        cur_pos=pos4;
    }
    else
    {
        array[34]="bmb 0";
    }
    array[35]="msf 1";
    array[36]="mhm";
    s = QString::number(wait4);
    array[37]="idl "+s;
    s = QString::number(mix4);
    d = QString::number(spd4);
    if(vol4>=150)
        array[38]="mix "+s+" "+d;
    else
        array[38]="mis "+s+" "+d;
    array[39]="mmf 1";
    s = QString::number(mag4);
    array[40]="mag "+s;
    array[41]="msb 1";


    //step 5
    array[42]=processname5;
    s = QString::number(temp5);


    if(temp51==1)
        array[43]="W lys "+s;
    else if(temp51==2)
        array[43]="W elu "+s;
    else
        array[43]="W end";



    if(pos5>cur_pos)
    {
        mov_pos=pos5-cur_pos;
        s = QString::number(mov_pos);
        array[44]="bmf "+s;
        cur_pos=pos5;
    }
    else if(pos5<cur_pos)
    {
        mov_pos=cur_pos-pos5;
        s = QString::number(mov_pos);
        array[44]="bmb "+s;
        cur_pos=pos5;
    }
    else
    {
        array[44]="bmb 0";
    }
    array[45]="msf 1";
    array[46]="mhm";
    s = QString::number(wait5);
    array[47]="idl "+s;
    s = QString::number(mix5);
    d = QString::number(spd5);
    if(vol5>=150)
        array[48]="mix "+s+" "+d;
    else
        array[48]="mis "+s+" "+d;    array[49]="mmf 1";
    s = QString::number(mag5);
    array[50]="mag "+s;
    array[51]="msb 1";


    //step 6
    array[52]=processname6;
    s = QString::number(temp6);


    if(temp61==1)
        array[53]="W lys "+s;
    else if(temp61==2)
        array[53]="W elu "+s;
    else
        array[53]="W end";


    if(pos6>cur_pos)
    {
        mov_pos=pos6-cur_pos;
        s = QString::number(mov_pos);
        array[54]="bmf "+s;
        cur_pos=pos6;
    }
    else if(pos6<cur_pos)
    {
        mov_pos=cur_pos-pos6;
        s = QString::number(mov_pos);
        array[54]="bmb "+s;
        cur_pos=pos6;
    }
    else
    {
        array[54]="bmb 0";
    }
    array[55]="msf 1";
    array[56]="mhm";
    s = QString::number(wait6);
    array[57]="idl "+s;
    s = QString::number(mix6);
    d = QString::number(spd6);
    if(vol6>=150)
        array[58]="mix "+s+" "+d;
    else
        array[58]="mis "+s+" "+d;
    array[59]="mmf 1";
    s = QString::number(mag6);
    array[60]="mag "+s;
    array[61]="msb 1";



    //step 7
    array[62]=processname7;
    s = QString::number(temp7);


    if(temp71==1)
        array[63]="W lys "+s;
    else if(temp71==2)
        array[63]="W elu "+s;
    else
        array[63]="W end";


    if(pos7>cur_pos)
    {
        mov_pos=pos7-cur_pos;
        s = QString::number(mov_pos);
        array[64]="bmf "+s;
        cur_pos=pos7;
    }
    else if(pos7<cur_pos)
    {
        mov_pos=cur_pos-pos7;
        s = QString::number(mov_pos);
        array[64]="bmb "+s;
        cur_pos=pos7;
    }
    else
    {
        array[64]="bmb 0";
    }
    array[65]="msf 1";
    array[66]="mhm";
    s = QString::number(wait7);
    array[67]="idl "+s;
    s = QString::number(mix7);
    d = QString::number(spd7);
    if(vol7>=150)
        array[68]="mix "+s+" "+d;
    else
        array[68]="mis "+s+" "+d;
    array[69]="mmf 1";
    s = QString::number(mag7);
    array[70]="mag "+s;
    array[71]="msb 1";

    for(int i=0;i<80;i++)
    {
        qDebug()<<array[i];
        if(array[i]=="")
        {
            array_len=i;
            break;
        }
    }
    array[0]="P"+array[0];
    for(int i=12;i<array_len;i+=10)
    {
        array[i]="P"+array[i];
    }
    array_len=array_len+1;
    array[array_len-4]="W end";
    array[array_len-3]="mhmm";
    array[array_len-2]="shm";
    array[array_len-1]="bhm";
    array[array_len]="buz 5";
    for(int i=0;i<=array_len;i++)
    {
        qDebug()<<array[i];
    }
    ui->label_91->setText(name);
    total_time=wait1+mix1+mag1+wait2+mix2+mag2+wait3+mix3+mag3+wait4+mix4+mag4+wait5+mix5+mag5+wait6+mix6+mag6+wait7+mix7+mag7;
    ui->lineEdit_149->setText(QDateTime::fromTime_t(total_time).toUTC().toString("hh:mm:ss"));
    timer1->start(1000);
    sub_time_loop=1;
}

void MainWindow::on_toolButton_clicked()
{
    ui->stackedWidget->setCurrentIndex(0);
    ui->label_91->hide();
    ui->toolButton_2->setVisible(true);
    ui->toolButton_8->setVisible(true);

    timer1->stop();
}

void MainWindow::on_toolButton_2_clicked()
{
    ui->stackedWidget->setCurrentIndex(4);
    ui->lineEdit_144->clear();
    ui->lineEdit_144->setStyleSheet("QLineEdit{background-color:rgb(255, 255, 255);}");
    ui->label_91->hide();
    timer1->stop();
}

void MainWindow::on_toolButton_26_clicked()
{
    ui->stackedWidget->setCurrentIndex(0);
    ui->listWidget->setCurrentRow(0);
}

void MainWindow::on_pushButton_3_clicked()
{
    ui->stackedWidget_2->setCurrentIndex(1);
}

void MainWindow::on_pushButton_32_clicked()
{
    ui->stackedWidget_2->setCurrentIndex(1);
}

void MainWindow::on_pushButton_66_clicked()
{
    ui->stackedWidget_2->setCurrentIndex(2);
}

void MainWindow::on_pushButton_26_clicked()
{
    if (opt==1)
    {
        ui->stackedWidget->setCurrentIndex(1);
        ui->lineEdit->setText(ui->lineEdit_145->text());
    }
    else if (opt==2)
    {
        ui->stackedWidget->setCurrentIndex(2);
        ui->lineEdit_3->setText(ui->lineEdit_145->text());
    }
    else if (opt==3)
    {
        ui->stackedWidget->setCurrentIndex(2);
        ui->lineEdit_14->setText(ui->lineEdit_145->text());
    }
    else if (opt==4)
    {
        ui->stackedWidget->setCurrentIndex(2);
        ui->lineEdit_20->setText(ui->lineEdit_145->text());
    }
    else if (opt==5)
    {
        ui->stackedWidget->setCurrentIndex(2);
        ui->lineEdit_26->setText(ui->lineEdit_145->text());
    }
    else if (opt==6)
    {
        ui->stackedWidget->setCurrentIndex(2);
        ui->lineEdit_32->setText(ui->lineEdit_145->text());
    }
    else if (opt==7)
    {
        ui->stackedWidget->setCurrentIndex(2);
        ui->lineEdit_38->setText(ui->lineEdit_145->text());
    }
    else if (opt==8)
    {
        ui->stackedWidget->setCurrentIndex(2);
        ui->lineEdit_44->setText(ui->lineEdit_145->text());
    }
    else if (opt==9)
    {
        ui->stackedWidget->setCurrentIndex(2);
        ui->lineEdit_4->setText(ui->lineEdit_145->text());
    }
    else if (opt==10)
    {
        ui->stackedWidget->setCurrentIndex(2);
        ui->lineEdit_12->setText(ui->lineEdit_145->text());
    }
    else if (opt==11)
    {
        ui->stackedWidget->setCurrentIndex(2);
        ui->lineEdit_18->setText(ui->lineEdit_145->text());
    }
    else if (opt==12)
    {
        ui->stackedWidget->setCurrentIndex(2);
        ui->lineEdit_24->setText(ui->lineEdit_145->text());
    }
    else if (opt==13)
    {
        ui->stackedWidget->setCurrentIndex(2);
        ui->lineEdit_30->setText(ui->lineEdit_145->text());
    }
    else if (opt==14)
    {
        ui->stackedWidget->setCurrentIndex(2);
        ui->lineEdit_36->setText(ui->lineEdit_145->text());
    }
    else if (opt==15)
    {
        ui->stackedWidget->setCurrentIndex(2);
        ui->lineEdit_42->setText(ui->lineEdit_145->text());
    }

    else if (opt==16)
    {
        ui->stackedWidget->setCurrentIndex(2);
        ui->lineEdit_8->setText(ui->lineEdit_145->text());
    }
    else if (opt==17)
    {
        ui->stackedWidget->setCurrentIndex(2);
        ui->lineEdit_13->setText(ui->lineEdit_145->text());
    }
    else if (opt==18)
    {
        ui->stackedWidget->setCurrentIndex(2);
        ui->lineEdit_19->setText(ui->lineEdit_145->text());
    }
    else if (opt==19)
    {
        ui->stackedWidget->setCurrentIndex(2);
        ui->lineEdit_25->setText(ui->lineEdit_145->text());
    }
    else if (opt==20)
    {
        ui->stackedWidget->setCurrentIndex(2);
        ui->lineEdit_31->setText(ui->lineEdit_145->text());
    }
    else if (opt==21)
    {
        ui->stackedWidget->setCurrentIndex(2);
        ui->lineEdit_37->setText(ui->lineEdit_145->text());
    }
    else if (opt==22)
    {
        ui->stackedWidget->setCurrentIndex(2);
        ui->lineEdit_43->setText(ui->lineEdit_145->text());
    }

    else if (opt==23)
    {
        ui->stackedWidget->setCurrentIndex(2);
        ui->lineEdit_9->setText(ui->lineEdit_145->text());
    }
    else if (opt==24)
    {
        ui->stackedWidget->setCurrentIndex(2);
        ui->lineEdit_15->setText(ui->lineEdit_145->text());
    }
    else if (opt==25)
    {
        ui->stackedWidget->setCurrentIndex(2);
        ui->lineEdit_21->setText(ui->lineEdit_145->text());
    }
    else if (opt==26)
    {
        ui->stackedWidget->setCurrentIndex(2);
        ui->lineEdit_27->setText(ui->lineEdit_145->text());
    }
    else if (opt==27)
    {
        ui->stackedWidget->setCurrentIndex(2);
        ui->lineEdit_33->setText(ui->lineEdit_145->text());
    }
    else if (opt==28)
    {
        ui->stackedWidget->setCurrentIndex(2);
        ui->lineEdit_39->setText(ui->lineEdit_145->text());
    }
    else if (opt==29)
    {
        ui->stackedWidget->setCurrentIndex(2);
        ui->lineEdit_45->setText(ui->lineEdit_145->text());
    }

    else if (opt==30)
    {
        ui->stackedWidget->setCurrentIndex(2);
        ui->lineEdit_10->setText(ui->lineEdit_145->text());
    }
    else if (opt==31)
    {
        ui->stackedWidget->setCurrentIndex(2);
        ui->lineEdit_16->setText(ui->lineEdit_145->text());
    }
    else if (opt==32)
    {
        ui->stackedWidget->setCurrentIndex(2);
        ui->lineEdit_22->setText(ui->lineEdit_145->text());
    }
    else if (opt==33)
    {
        ui->stackedWidget->setCurrentIndex(2);
        ui->lineEdit_28->setText(ui->lineEdit_145->text());
    }
    else if (opt==34)
    {
        ui->stackedWidget->setCurrentIndex(2);
        ui->lineEdit_34->setText(ui->lineEdit_145->text());
    }
    else if (opt==35)
    {
        ui->stackedWidget->setCurrentIndex(2);
        ui->lineEdit_40->setText(ui->lineEdit_145->text());
    }
    else if (opt==36)
    {
        ui->stackedWidget->setCurrentIndex(2);
        ui->lineEdit_46->setText(ui->lineEdit_145->text());
    }

    else if (opt==37)
    {
        ui->stackedWidget->setCurrentIndex(2);
        ui->lineEdit_11->setText(ui->lineEdit_145->text());
    }
    else if (opt==38)
    {
        ui->stackedWidget->setCurrentIndex(2);
        ui->lineEdit_17->setText(ui->lineEdit_145->text());
    }
    else if (opt==39)
    {
        ui->stackedWidget->setCurrentIndex(2);
        ui->lineEdit_23->setText(ui->lineEdit_145->text());
    }
    else if (opt==40)
    {
        ui->stackedWidget->setCurrentIndex(2);
        ui->lineEdit_29->setText(ui->lineEdit_145->text());
    }
    else if (opt==41)
    {
        ui->stackedWidget->setCurrentIndex(2);
        ui->lineEdit_35->setText(ui->lineEdit_145->text());
    }
    else if (opt==42)
    {
        ui->stackedWidget->setCurrentIndex(2);
        ui->lineEdit_41->setText(ui->lineEdit_145->text());
    }
    else if (opt==43)
    {
        ui->stackedWidget->setCurrentIndex(2);
        ui->lineEdit_47->setText(ui->lineEdit_145->text());
    }
    else if (opt==44)
    {
        ui->stackedWidget->setCurrentIndex(4);
        ui->lineEdit_144->setText(ui->lineEdit_145->text());
    }
    else if (opt==45)
    {
        ui->stackedWidget->setCurrentIndex(9);
        ui->lineEdit_2->setText(ui->lineEdit_145->text());
    }
    else if (opt==46)
    {
        ui->stackedWidget->setCurrentIndex(11);
        ui->lineEdit_111->setText(ui->lineEdit_145->text());
    }
}

void MainWindow::on_pushButton_95_clicked()
{
    ui->stackedWidget_2->setCurrentIndex(0);
}

void MainWindow::on_pushButton_99_clicked()
{
    ui->stackedWidget_2->setCurrentIndex(0);
}

void MainWindow::on_pushButton_104_clicked()
{
    ui->stackedWidget_2->setCurrentIndex(2);
}

void MainWindow::on_pushButton_88_clicked()
{
    on_pushButton_26_clicked();
}

void MainWindow::on_pushButton_136_clicked()
{
    on_pushButton_26_clicked();
}

void MainWindow::on_pushButton_139_clicked()
{
    ui->stackedWidget_2->setCurrentIndex(0);
}

void MainWindow::on_pushButton_111_clicked()
{
    opt=2;
    ui->stackedWidget->setCurrentIndex(5);
    ui->stackedWidget_2->setCurrentIndex(0);
    ui->lineEdit_145->setText(ui->lineEdit_3->text());

}

void MainWindow::on_pushButton_112_clicked()
{
    opt=3;
    ui->stackedWidget->setCurrentIndex(5);
    ui->stackedWidget_2->setCurrentIndex(0);
    ui->lineEdit_145->setText(ui->lineEdit_14->text());
}

void MainWindow::on_pushButton_113_clicked()
{
    opt=4;
    ui->stackedWidget->setCurrentIndex(5);
    ui->stackedWidget_2->setCurrentIndex(0);
    ui->lineEdit_145->setText(ui->lineEdit_20->text());
}

void MainWindow::on_pushButton_117_clicked()
{
    opt=5;
    ui->stackedWidget->setCurrentIndex(5);
    ui->stackedWidget_2->setCurrentIndex(0);
    ui->lineEdit_145->setText(ui->lineEdit_26->text());
}

void MainWindow::on_pushButton_118_clicked()
{
    opt=6;
    ui->stackedWidget->setCurrentIndex(5);
    ui->stackedWidget_2->setCurrentIndex(0);
    ui->lineEdit_145->setText(ui->lineEdit_32->text());
}

void MainWindow::on_pushButton_119_clicked()
{
    opt=7;
    ui->stackedWidget->setCurrentIndex(5);
    ui->stackedWidget_2->setCurrentIndex(0);
    ui->lineEdit_145->setText(ui->lineEdit_38->text());
}

void MainWindow::on_pushButton_120_clicked()
{
    opt=8;
    ui->stackedWidget->setCurrentIndex(5);
    ui->stackedWidget_2->setCurrentIndex(0);
    ui->lineEdit_145->setText(ui->lineEdit_44->text());
}

void MainWindow::on_pushButton_122_clicked()
{
    opt=9;
    ui->stackedWidget->setCurrentIndex(5);
    ui->stackedWidget_2->setCurrentIndex(2);
    ui->lineEdit_145->setText(ui->lineEdit_4->text());
}

void MainWindow::on_pushButton_129_clicked()
{
    opt=10;
    ui->stackedWidget->setCurrentIndex(5);
    ui->stackedWidget_2->setCurrentIndex(2);
    ui->lineEdit_145->setText(ui->lineEdit_12->text());
}

void MainWindow::on_pushButton_124_clicked()
{
    opt=11;
    ui->stackedWidget->setCurrentIndex(5);
    ui->stackedWidget_2->setCurrentIndex(2);
    ui->lineEdit_145->setText(ui->lineEdit_18->text());
}

void MainWindow::on_pushButton_127_clicked()
{
    opt=12;
    ui->stackedWidget->setCurrentIndex(5);
    ui->stackedWidget_2->setCurrentIndex(2);
    ui->lineEdit_145->setText(ui->lineEdit_24->text());
}

void MainWindow::on_pushButton_126_clicked()
{
    opt=13;
    ui->stackedWidget->setCurrentIndex(5);
    ui->stackedWidget_2->setCurrentIndex(2);
    ui->lineEdit_145->setText(ui->lineEdit_30->text());
}

void MainWindow::on_pushButton_130_clicked()
{
    opt=14;
    ui->stackedWidget->setCurrentIndex(5);
    ui->stackedWidget_2->setCurrentIndex(2);
    ui->lineEdit_145->setText(ui->lineEdit_36->text());
}

void MainWindow::on_pushButton_121_clicked()
{
    opt=15;
    ui->stackedWidget->setCurrentIndex(5);
    ui->stackedWidget_2->setCurrentIndex(2);
    ui->lineEdit_145->setText(ui->lineEdit_42->text());
}

void MainWindow::on_pushButton_131_clicked()
{
    opt=17;
    ui->stackedWidget->setCurrentIndex(5);
    ui->stackedWidget_2->setCurrentIndex(2);
    ui->lineEdit_145->setText(ui->lineEdit_13->text());
}

void MainWindow::on_pushButton_132_clicked()
{
    opt=16;
    ui->stackedWidget->setCurrentIndex(5);
    ui->stackedWidget_2->setCurrentIndex(2);
    ui->lineEdit_145->setText(ui->lineEdit_8->text());
}

void MainWindow::on_pushButton_142_clicked()
{
    opt=18;
    ui->stackedWidget->setCurrentIndex(5);
    ui->stackedWidget_2->setCurrentIndex(2);
    ui->lineEdit_145->setText(ui->lineEdit_19->text());
}

void MainWindow::on_pushButton_135_clicked()
{
    opt=19;
    ui->stackedWidget->setCurrentIndex(5);
    ui->stackedWidget_2->setCurrentIndex(2);
    ui->lineEdit_145->setText(ui->lineEdit_25->text());
}

void MainWindow::on_pushButton_133_clicked()
{
    opt=20;
    ui->stackedWidget->setCurrentIndex(5);
    ui->stackedWidget_2->setCurrentIndex(2);
    ui->lineEdit_145->setText(ui->lineEdit_31->text());
}

void MainWindow::on_pushButton_144_clicked()
{
    opt=21;
    ui->stackedWidget->setCurrentIndex(5);
    ui->stackedWidget_2->setCurrentIndex(2);
    ui->lineEdit_145->setText(ui->lineEdit_37->text());
}

void MainWindow::on_pushButton_143_clicked()
{
    opt=22;
    ui->stackedWidget->setCurrentIndex(5);
    ui->stackedWidget_2->setCurrentIndex(2);
    ui->lineEdit_145->setText(ui->lineEdit_43->text());
}

void MainWindow::on_pushButton_146_clicked()
{
    opt=23;
    ui->stackedWidget->setCurrentIndex(5);
    ui->stackedWidget_2->setCurrentIndex(2);
    ui->lineEdit_145->setText(ui->lineEdit_9->text());
}

void MainWindow::on_pushButton_145_clicked()
{
    opt=24;
    ui->stackedWidget->setCurrentIndex(5);
    ui->stackedWidget_2->setCurrentIndex(2);
    ui->lineEdit_145->setText(ui->lineEdit_15->text());
}

void MainWindow::on_pushButton_149_clicked()
{
    opt=25;
    ui->stackedWidget->setCurrentIndex(5);
    ui->stackedWidget_2->setCurrentIndex(2);
    ui->lineEdit_145->setText(ui->lineEdit_21->text());
}

void MainWindow::on_pushButton_148_clicked()
{
    opt=26;
    ui->stackedWidget->setCurrentIndex(5);
    ui->stackedWidget_2->setCurrentIndex(2);
    ui->lineEdit_145->setText(ui->lineEdit_27->text());
}

void MainWindow::on_pushButton_147_clicked()
{
    opt=27;
    ui->stackedWidget->setCurrentIndex(5);
    ui->stackedWidget_2->setCurrentIndex(2);
    ui->lineEdit_145->setText(ui->lineEdit_33->text());
}

void MainWindow::on_pushButton_151_clicked()
{
    opt=28;
    ui->stackedWidget->setCurrentIndex(5);
    ui->stackedWidget_2->setCurrentIndex(2);
    ui->lineEdit_145->setText(ui->lineEdit_39->text());
}

void MainWindow::on_pushButton_150_clicked()
{
    opt=29;
    ui->stackedWidget->setCurrentIndex(5);
    ui->stackedWidget_2->setCurrentIndex(2);
    ui->lineEdit_145->setText(ui->lineEdit_45->text());
}

void MainWindow::on_pushButton_153_clicked()
{
    opt=30;
    ui->stackedWidget->setCurrentIndex(5);
    ui->stackedWidget_2->setCurrentIndex(2);
    ui->lineEdit_145->setText(ui->lineEdit_10->text());
}

void MainWindow::on_pushButton_152_clicked()
{
    opt=31;
    ui->stackedWidget->setCurrentIndex(5);
    ui->stackedWidget_2->setCurrentIndex(2);
    ui->lineEdit_145->setText(ui->lineEdit_16->text());
}

void MainWindow::on_pushButton_156_clicked()
{
    opt=32;
    ui->stackedWidget->setCurrentIndex(5);
    ui->stackedWidget_2->setCurrentIndex(2);
    ui->lineEdit_145->setText(ui->lineEdit_22->text());
}

void MainWindow::on_pushButton_155_clicked()
{
    opt=33;
    ui->stackedWidget->setCurrentIndex(5);
    ui->stackedWidget_2->setCurrentIndex(2);
    ui->lineEdit_145->setText(ui->lineEdit_28->text());
}

void MainWindow::on_pushButton_154_clicked()
{
    opt=34;
    ui->stackedWidget->setCurrentIndex(5);
    ui->stackedWidget_2->setCurrentIndex(2);
    ui->lineEdit_145->setText(ui->lineEdit_34->text());
}

void MainWindow::on_pushButton_158_clicked()
{
    opt=35;
    ui->stackedWidget->setCurrentIndex(5);
    ui->stackedWidget_2->setCurrentIndex(2);
    ui->lineEdit_145->setText(ui->lineEdit_40->text());
}

void MainWindow::on_pushButton_157_clicked()
{
    opt=36;
    ui->stackedWidget->setCurrentIndex(5);
    ui->stackedWidget_2->setCurrentIndex(2);
    ui->lineEdit_145->setText(ui->lineEdit_46->text());
}

void MainWindow::on_pushButton_164_clicked()
{
    opt=37;
    ui->stackedWidget->setCurrentIndex(5);
    ui->stackedWidget_2->setCurrentIndex(2);
    ui->lineEdit_145->setText(ui->lineEdit_11->text());
}

void MainWindow::on_pushButton_162_clicked()
{
    opt=38;
    ui->stackedWidget->setCurrentIndex(5);
    ui->stackedWidget_2->setCurrentIndex(2);
    ui->lineEdit_145->setText(ui->lineEdit_17->text());
}

void MainWindow::on_pushButton_161_clicked()
{
    opt=39;
    ui->stackedWidget->setCurrentIndex(5);
    ui->stackedWidget_2->setCurrentIndex(2);
    ui->lineEdit_145->setText(ui->lineEdit_23->text());
}

void MainWindow::on_pushButton_159_clicked()
{
    opt=40;
    ui->stackedWidget->setCurrentIndex(5);
    ui->stackedWidget_2->setCurrentIndex(2);
    ui->lineEdit_145->setText(ui->lineEdit_29->text());
}

void MainWindow::on_pushButton_160_clicked()
{
    opt=41;
    ui->stackedWidget->setCurrentIndex(5);
    ui->stackedWidget_2->setCurrentIndex(2);
    ui->lineEdit_145->setText(ui->lineEdit_35->text());
}

void MainWindow::on_pushButton_163_clicked()
{
    opt=42;
    ui->stackedWidget->setCurrentIndex(5);
    ui->stackedWidget_2->setCurrentIndex(2);
    ui->lineEdit_145->setText(ui->lineEdit_41->text());
}

void MainWindow::on_pushButton_165_clicked()
{
    opt=43;
    ui->stackedWidget->setCurrentIndex(5);
    ui->stackedWidget_2->setCurrentIndex(2);
    ui->lineEdit_145->setText(ui->lineEdit_47->text());
}

void MainWindow::on_pushButton_166_clicked()
{
    opt=44;
    ui->stackedWidget->setCurrentIndex(5);
    ui->stackedWidget_2->setCurrentIndex(2);
    ui->lineEdit_145->setText(ui->lineEdit_144->text());
}

void MainWindow::on_pushButton_clicked()
{
    ui->lineEdit_145->setText(ui->lineEdit_145->text()+ui->pushButton->text());
}

void MainWindow::on_pushButton_15_clicked()
{
    ui->lineEdit_145->backspace();
}

void MainWindow::on_pushButton_2_clicked()
{
    ui->lineEdit_145->setText(ui->lineEdit_145->text()+ui->pushButton_2->text());
}

void MainWindow::on_pushButton_6_clicked()
{
    ui->lineEdit_145->setText(ui->lineEdit_145->text()+ui->pushButton_6->text());
}

void MainWindow::on_pushButton_5_clicked()
{
    ui->lineEdit_145->setText(ui->lineEdit_145->text()+ui->pushButton_5->text());
}

void MainWindow::on_pushButton_14_clicked()
{
    ui->lineEdit_145->setText(ui->lineEdit_145->text()+ui->pushButton_14->text());
}

void MainWindow::on_pushButton_12_clicked()
{
    ui->lineEdit_145->setText(ui->lineEdit_145->text()+ui->pushButton_12->text());
}

void MainWindow::on_pushButton_13_clicked()
{
    ui->lineEdit_145->setText(ui->lineEdit_145->text()+ui->pushButton_13->text());
}

void MainWindow::on_pushButton_11_clicked()
{
    ui->lineEdit_145->setText(ui->lineEdit_145->text()+ui->pushButton_11->text());
}

void MainWindow::on_pushButton_18_clicked()
{
    ui->lineEdit_145->setText(ui->lineEdit_145->text()+ui->pushButton_18->text());
}

void MainWindow::on_pushButton_16_clicked()
{
    ui->lineEdit_145->setText(ui->lineEdit_145->text()+ui->pushButton_16->text());
}

void MainWindow::on_pushButton_27_clicked()
{
    ui->lineEdit_145->setText(ui->lineEdit_145->text()+ui->pushButton_27->text());
}

void MainWindow::on_pushButton_20_clicked()
{
    ui->lineEdit_145->setText(ui->lineEdit_145->text()+ui->pushButton_20->text());
}

void MainWindow::on_pushButton_21_clicked()
{
    ui->lineEdit_145->setText(ui->lineEdit_145->text()+ui->pushButton_21->text());
}

void MainWindow::on_pushButton_19_clicked()
{
    ui->lineEdit_145->setText(ui->lineEdit_145->text()+ui->pushButton_19->text());
}

void MainWindow::on_pushButton_28_clicked()
{
    ui->lineEdit_145->setText(ui->lineEdit_145->text()+ui->pushButton_28->text());
}

void MainWindow::on_pushButton_25_clicked()
{
    ui->lineEdit_145->setText(ui->lineEdit_145->text()+ui->pushButton_25->text());
}

void MainWindow::on_pushButton_22_clicked()
{
    ui->lineEdit_145->setText(ui->lineEdit_145->text()+ui->pushButton_22->text());
}

void MainWindow::on_pushButton_23_clicked()
{
    ui->lineEdit_145->setText(ui->lineEdit_145->text()+ui->pushButton_23->text());
}

void MainWindow::on_pushButton_29_clicked()
{
    ui->lineEdit_145->setText(ui->lineEdit_145->text()+ui->pushButton_29->text());
}

void MainWindow::on_pushButton_4_clicked()
{
    ui->lineEdit_145->setText(ui->lineEdit_145->text()+ui->pushButton_4->text());
}

void MainWindow::on_pushButton_8_clicked()
{
    ui->lineEdit_145->setText(ui->lineEdit_145->text()+ui->pushButton_8->text());
}

void MainWindow::on_pushButton_7_clicked()
{
    ui->lineEdit_145->setText(ui->lineEdit_145->text()+ui->pushButton_7->text());
}

void MainWindow::on_pushButton_33_clicked()
{
    ui->lineEdit_145->setText(ui->lineEdit_145->text()+ui->pushButton_33->text());
}

void MainWindow::on_pushButton_34_clicked()
{
    ui->lineEdit_145->setText(ui->lineEdit_145->text()+ui->pushButton_34->text());
}

void MainWindow::on_pushButton_30_clicked()
{
    ui->lineEdit_145->setText(ui->lineEdit_145->text()+ui->pushButton_30->text());
}

void MainWindow::on_pushButton_31_clicked()
{
    ui->lineEdit_145->setText(ui->lineEdit_145->text()+ui->pushButton_31->text());
}

void MainWindow::on_pushButton_24_clicked()
{
    ui->lineEdit_145->setText(ui->lineEdit_145->text()+ui->pushButton_24->text());
}

void MainWindow::on_pushButton_17_clicked()
{
    ui->lineEdit_145->setText(ui->lineEdit_145->text()+ui->pushButton_17->text());
}

void MainWindow::on_pushButton_68_clicked()
{
    ui->lineEdit_145->setText(ui->lineEdit_145->text()+" ");
}

void MainWindow::on_pushButton_69_clicked()
{
    ui->lineEdit_145->setText(ui->lineEdit_145->text()+ui->pushButton_69->text());
}

void MainWindow::on_pushButton_67_clicked()
{
    ui->lineEdit_145->setText(ui->lineEdit_145->text()+ui->pushButton_67->text());
}

void MainWindow::on_pushButton_65_clicked()
{
    ui->lineEdit_145->setText(ui->lineEdit_145->text()+ui->pushButton_65->text());
}

void MainWindow::on_pushButton_70_clicked()
{
    ui->lineEdit_145->setText(ui->lineEdit_145->text()+ui->pushButton_70->text());
}

void MainWindow::on_pushButton_79_clicked()
{
    ui->lineEdit_145->backspace();
}

void MainWindow::on_pushButton_109_clicked()
{
    ui->lineEdit_145->backspace();
}

void MainWindow::on_pushButton_71_clicked()
{
    ui->lineEdit_145->setText(ui->lineEdit_145->text()+ui->pushButton_71->text());
}

void MainWindow::on_pushButton_72_clicked()
{
    ui->lineEdit_145->setText(ui->lineEdit_145->text()+ui->pushButton_72->text());
}

void MainWindow::on_pushButton_74_clicked()
{
    ui->lineEdit_145->setText(ui->lineEdit_145->text()+ui->pushButton_74->text());
}

void MainWindow::on_pushButton_73_clicked()
{
    ui->lineEdit_145->setText(ui->lineEdit_145->text()+ui->pushButton_73->text());
}

void MainWindow::on_pushButton_78_clicked()
{
    ui->lineEdit_145->setText(ui->lineEdit_145->text()+ui->pushButton_78->text());
}

void MainWindow::on_pushButton_76_clicked()
{
    ui->lineEdit_145->setText(ui->lineEdit_145->text()+ui->pushButton_76->text());
}

void MainWindow::on_pushButton_77_clicked()
{
    ui->lineEdit_145->setText(ui->lineEdit_145->text()+ui->pushButton_77->text());
}

void MainWindow::on_pushButton_75_clicked()
{
    ui->lineEdit_145->setText(ui->lineEdit_145->text()+ui->pushButton_75->text());
}

void MainWindow::on_pushButton_81_clicked()
{
    ui->lineEdit_145->setText(ui->lineEdit_145->text()+ui->pushButton_81->text());
}

void MainWindow::on_pushButton_80_clicked()
{
    ui->lineEdit_145->setText(ui->lineEdit_145->text()+ui->pushButton_80->text());
}

void MainWindow::on_pushButton_89_clicked()
{
    ui->lineEdit_145->setText(ui->lineEdit_145->text()+ui->pushButton_89->text());
}

void MainWindow::on_pushButton_83_clicked()
{
    ui->lineEdit_145->setText(ui->lineEdit_145->text()+ui->pushButton_83->text());
}

void MainWindow::on_pushButton_84_clicked()
{
    ui->lineEdit_145->setText(ui->lineEdit_145->text()+ui->pushButton_84->text());
}

void MainWindow::on_pushButton_82_clicked()
{
    ui->lineEdit_145->setText(ui->lineEdit_145->text()+ui->pushButton_82->text());
}

void MainWindow::on_pushButton_90_clicked()
{
    ui->lineEdit_145->setText(ui->lineEdit_145->text()+ui->pushButton_90->text());
}

void MainWindow::on_pushButton_87_clicked()
{
    ui->lineEdit_145->setText(ui->lineEdit_145->text()+ui->pushButton_87->text());
}

void MainWindow::on_pushButton_85_clicked()
{
    ui->lineEdit_145->setText(ui->lineEdit_145->text()+ui->pushButton_85->text());
}

void MainWindow::on_pushButton_86_clicked()
{
    ui->lineEdit_145->setText(ui->lineEdit_145->text()+ui->pushButton_86->text());
}

void MainWindow::on_pushButton_91_clicked()
{
    ui->lineEdit_145->setText(ui->lineEdit_145->text()+ui->pushButton_91->text());
}

void MainWindow::on_pushButton_101_clicked()
{
    ui->lineEdit_145->setText(ui->lineEdit_145->text()+ui->pushButton_101->text());
}

void MainWindow::on_pushButton_98_clicked()
{
    ui->lineEdit_145->setText(ui->lineEdit_145->text()+ui->pushButton_98->text());
}

void MainWindow::on_pushButton_93_clicked()
{
    ui->lineEdit_145->setText(ui->lineEdit_145->text()+ui->pushButton_93->text());
}

void MainWindow::on_pushButton_100_clicked()
{
    ui->lineEdit_145->setText(ui->lineEdit_145->text()+ui->pushButton_100->text());
}

void MainWindow::on_pushButton_102_clicked()
{
    ui->lineEdit_145->setText(ui->lineEdit_145->text()+ui->pushButton_102->text());
}

void MainWindow::on_pushButton_96_clicked()
{
    ui->lineEdit_145->setText(ui->lineEdit_145->text()+ui->pushButton_96->text());
}

void MainWindow::on_pushButton_97_clicked()
{
    ui->lineEdit_145->setText(ui->lineEdit_145->text()+ui->pushButton_97->text());
}

void MainWindow::on_pushButton_92_clicked()
{
    ui->lineEdit_145->setText(ui->lineEdit_145->text()+ui->pushButton_92->text());
}

void MainWindow::on_pushButton_94_clicked()
{
    ui->lineEdit_145->setText(ui->lineEdit_145->text()+ui->pushButton_94->text());
}

void MainWindow::on_pushButton_106_clicked()
{
    ui->lineEdit_145->setText(ui->lineEdit_145->text()+" ");
}

void MainWindow::on_pushButton_107_clicked()
{
    ui->lineEdit_145->setText(ui->lineEdit_145->text()+ui->pushButton_107->text());
}

void MainWindow::on_pushButton_105_clicked()
{
    ui->lineEdit_145->setText(ui->lineEdit_145->text()+ui->pushButton_105->text());
}

void MainWindow::on_pushButton_103_clicked()
{
    ui->lineEdit_145->setText(ui->lineEdit_145->text()+ui->pushButton_103->text());
}

void MainWindow::on_pushButton_108_clicked()
{
    ui->lineEdit_145->setText(ui->lineEdit_145->text()+ui->pushButton_108->text());
}

void MainWindow::on_pushButton_116_clicked()
{
    ui->lineEdit_145->setText(ui->lineEdit_145->text()+ui->pushButton_116->text());
}

void MainWindow::on_pushButton_114_clicked()
{
    ui->lineEdit_145->setText(ui->lineEdit_145->text()+ui->pushButton_114->text());
}

void MainWindow::on_pushButton_115_clicked()
{
    ui->lineEdit_145->setText(ui->lineEdit_145->text()+ui->pushButton_115->text());
}

void MainWindow::on_pushButton_128_clicked()
{
    ui->lineEdit_145->setText(ui->lineEdit_145->text()+ui->pushButton_128->text());
}

void MainWindow::on_pushButton_125_clicked()
{
    ui->lineEdit_145->setText(ui->lineEdit_145->text()+ui->pushButton_125->text());
}

void MainWindow::on_pushButton_123_clicked()
{
    ui->lineEdit_145->setText(ui->lineEdit_145->text()+ui->pushButton_123->text());
}

void MainWindow::on_pushButton_138_clicked()
{
    ui->lineEdit_145->setText(ui->lineEdit_145->text()+ui->pushButton_138->text());
}

void MainWindow::on_pushButton_140_clicked()
{
    ui->lineEdit_145->setText(ui->lineEdit_145->text()+ui->pushButton_140->text());
}

void MainWindow::on_pushButton_134_clicked()
{
    ui->lineEdit_145->setText(ui->lineEdit_145->text()+ui->pushButton_134->text());
}

void MainWindow::on_pushButton_141_clicked()
{
    ui->lineEdit_145->setText(ui->lineEdit_145->text()+ui->pushButton_141->text());
}

void MainWindow::on_pushButton_137_clicked()
{
    ui->lineEdit_145->setText(ui->lineEdit_145->text()+ui->pushButton_137->text());
}

void MainWindow::on_toolButton_5_clicked()
{
    QString selitem;
    selitem=ui->listWidget->currentItem()->text();
    QString txt="Confirm Delete "+selitem;

    QMessageBox msgBox;
    //msgBox.setWindowFlags(Qt::Window | Qt::FramelessWindowHint);
    msgBox.setWindowTitle("Warning...");
    msgBox.setText(txt);
    msgBox.setStandardButtons(msgBox.Yes);
    msgBox.addButton(msgBox.No);
    msgBox.setStyleSheet("QLabel{min-width:500 px; font-size: 24px;} QPushButton{ width:200px; height:50px; font-size: 18px; }");
    if(msgBox.exec() == msgBox.Yes)
    {
        QSqlQuery query;
        query.prepare("delete FROM DNA where name=:name");
        query.bindValue(":name",selitem);
        query.exec();

        query.prepare("SELECT name FROM DNA");
        query.exec();
        ui->listWidget->clear();
        while(query.next())
        {
            ui->listWidget->addItem(query.value(0).toString());
        }
        ui->listWidget->setCurrentRow(0);
    }
    else
    {
        // do something else
    }

}

void MainWindow::on_toolButton_4_clicked()
{
    ui->toolButton->hide();
    ui->toolButton_2->hide();
    ui->toolButton_8->hide();
    QString temp;
    temp=ui->listWidget->currentItem()->text();
    ui->label_91->setVisible(true);
    ui->label_91->setText(temp);
    //qDebug()<<selitem;
    QSqlQuery query;
    query.prepare("select pname1,pname2,pname3,pname4,pname5,pname6,pname7 FROM DNA where name=:name");
    query.bindValue(":name",temp);
    query.exec();
    while(query.next())
    {
        ui->lineEdit_3->setText(query.value(0).toString());
        ui->lineEdit_14->setText(query.value(1).toString());
        ui->lineEdit_20->setText(query.value(2).toString());
        ui->lineEdit_26->setText(query.value(3).toString());
        ui->lineEdit_32->setText(query.value(4).toString());
        ui->lineEdit_38->setText(query.value(5).toString());
        ui->lineEdit_44->setText(query.value(6).toString());
    }

    query.prepare("select well1,well2,well3,well4,well5,well6,well7 FROM DNA where name=:name");
    query.bindValue(":name",temp);
    query.exec();
    while(query.next())
    {
        ui->comboBox_3->setCurrentIndex(query.value(0).toInt()-1);
        ui->comboBox_8->setCurrentIndex(query.value(1).toInt()-1);
        ui->comboBox_10->setCurrentIndex(query.value(2).toInt()-1);
        ui->comboBox_12->setCurrentIndex(query.value(3).toInt()-1);
        ui->comboBox_14->setCurrentIndex(query.value(4).toInt()-1);
        ui->comboBox_16->setCurrentIndex(query.value(5).toInt()-1);
        ui->comboBox_18->setCurrentIndex(query.value(6).toInt()-1);
    }

    query.prepare("select wait1,wait2,wait3,wait4,wait5,wait6,wait7 FROM DNA where name=:name");
    query.bindValue(":name",temp);
    query.exec();
    while(query.next())
    {
        ui->lineEdit_4->setText(query.value(0).toString());
        ui->lineEdit_12->setText(query.value(1).toString());
        ui->lineEdit_18->setText(query.value(2).toString());
        ui->lineEdit_24->setText(query.value(3).toString());
        ui->lineEdit_30->setText(query.value(4).toString());
        ui->lineEdit_36->setText(query.value(5).toString());
        ui->lineEdit_42->setText(query.value(6).toString());
    }

    query.prepare("select mix1,mix2,mix3,mix4,mix5,mix6,mix7 FROM DNA where name=:name");
    query.bindValue(":name",temp);
    query.exec();
    while(query.next())
    {
        ui->lineEdit_8->setText(query.value(0).toString());
        ui->lineEdit_13->setText(query.value(1).toString());
        ui->lineEdit_19->setText(query.value(2).toString());
        ui->lineEdit_25->setText(query.value(3).toString());
        ui->lineEdit_31->setText(query.value(4).toString());
        ui->lineEdit_37->setText(query.value(5).toString());
        ui->lineEdit_43->setText(query.value(6).toString());
    }
    query.prepare("select mag1,mag2,mag3,mag4,mag5,mag6,mag7 FROM DNA where name=:name");
    query.bindValue(":name",temp);
    query.exec();
    while(query.next())
    {
        ui->lineEdit_9->setText(query.value(0).toString());
        ui->lineEdit_15->setText(query.value(1).toString());
        ui->lineEdit_21->setText(query.value(2).toString());
        ui->lineEdit_27->setText(query.value(3).toString());
        ui->lineEdit_33->setText(query.value(4).toString());
        ui->lineEdit_39->setText(query.value(5).toString());
        ui->lineEdit_45->setText(query.value(6).toString());
    }
    query.prepare("select vol1,vol2,vol3,vol4,vol5,vol6,vol7 FROM DNA where name=:name");
    query.bindValue(":name",temp);
    query.exec();
    while(query.next())
    {
        ui->lineEdit_10->setText(query.value(0).toString());
        ui->lineEdit_16->setText(query.value(1).toString());
        ui->lineEdit_22->setText(query.value(2).toString());
        ui->lineEdit_28->setText(query.value(3).toString());
        ui->lineEdit_34->setText(query.value(4).toString());
        ui->lineEdit_40->setText(query.value(5).toString());
        ui->lineEdit_46->setText(query.value(6).toString());
    }
    query.prepare("select spd1,spd2,spd3,spd4,spd5,spd6,spd7 FROM DNA where name=:name");
    query.bindValue(":name",temp);
    query.exec();
    while(query.next())
    {
        ui->comboBox_6->setCurrentIndex(query.value(0).toInt()-1);
        ui->comboBox_7->setCurrentIndex(query.value(1).toInt()-1);
        ui->comboBox_9->setCurrentIndex(query.value(2).toInt()-1);
        ui->comboBox_11->setCurrentIndex(query.value(3).toInt()-1);
        ui->comboBox_13->setCurrentIndex(query.value(4).toInt()-1);
        ui->comboBox_15->setCurrentIndex(query.value(5).toInt()-1);
        ui->comboBox_17->setCurrentIndex(query.value(6).toInt()-1);
    }
    query.prepare("select temp1,temp2,temp3,temp4,temp5,temp6,temp7 FROM DNA where name=:name");
    query.bindValue(":name",temp);
    query.exec();
    while(query.next())
    {
        ui->lineEdit_11->setText(query.value(0).toString());
        ui->lineEdit_17->setText(query.value(1).toString());
        ui->lineEdit_23->setText(query.value(2).toString());
        ui->lineEdit_29->setText(query.value(3).toString());
        ui->lineEdit_35->setText(query.value(4).toString());
        ui->lineEdit_41->setText(query.value(5).toString());
        ui->lineEdit_47->setText(query.value(6).toString());
    }
    query.prepare("select temp11,temp21,temp31,temp41,temp51,temp61,temp71 FROM DNA where name=:name");
    query.bindValue(":name",temp);
    query.exec();
    while(query.next())
    {
        ui->comboBox_21->setCurrentIndex(query.value(0).toInt());
        ui->comboBox_22->setCurrentIndex(query.value(1).toInt());
        ui->comboBox_36->setCurrentIndex(query.value(2).toInt());
        ui->comboBox_37->setCurrentIndex(query.value(3).toInt());
        ui->comboBox_38->setCurrentIndex(query.value(4).toInt());
        ui->comboBox_39->setCurrentIndex(query.value(5).toInt());
        ui->comboBox_40->setCurrentIndex(query.value(6).toInt());
    }


    ui->lineEdit->setText(temp);
    from_edit=1;
    ui->stackedWidget->setCurrentIndex(2);

}

void MainWindow::realtime_temperature()
{
    Pi2c arduino(8); //Create a new object "arduino" using address "0x07"
    char receive[30]; //Create a buffer of char (single bytes) for the data
    //Receive from the Arduino and put the contents into the "receive" char array
    QThread::msleep(100);
    arduino.i2cRead(receive,30);
    QThread::msleep(100);
    qDebug()<<receive;
    QString str=receive;
    QStringList list;
    list=str.split(" ");
    //qDebug()<<list.size();
    if(list.size()<8)
    {

    }
    else if(list[7].toInt()>0 && list[7].toInt()<110)
    {
        ui->lineEdit_48->setText(list[0]);
        ui->lineEdit_49->setText(list[1]);
        ui->lineEdit_92->setText(list[2]);
        ui->lineEdit_93->setText(list[3]);
        ui->lineEdit_140->setText(list[4]);
        ui->lineEdit_141->setText(list[5]);
        ui->lineEdit_142->setText(list[6]);
        ui->lineEdit_143->setText(list[7]);

        ui->lineEdit_107->setText(list[0]);
        ui->lineEdit_108->setText(list[1]);
        ui->lineEdit_109->setText(list[2]);
        ui->lineEdit_110->setText(list[3]);
        ui->lineEdit_160->setText(list[4]);
        ui->lineEdit_161->setText(list[5]);
        ui->lineEdit_162->setText(list[6]);
        ui->lineEdit_163->setText(list[7]);
    }
}

void MainWindow::processing()
{
restart:
    Pi2c arduino(7);
    char receive[30];
    QString data="dss";
    char* ch;
    QByteArray ba=data.toLatin1();
    ch=ba.data();
    QThread::msleep(100);
    arduino.i2cWrite(ch,30);
    QThread::msleep(100);
    arduino.i2cRead(receive,30);
    QThread::msleep(100);
    qDebug()<<receive;
    if(strncmp(receive,"dopen",4)==0)
    {
        ui->stackedWidget->setCurrentIndex(7);
    }
    else if(strncmp(receive,"done",4)==0)
    {
        ui->stackedWidget->setCurrentIndex(3);
        timer_val=1;
        if(array[0][0]=='P')
        {
            array[0].remove(0,1);
            ui->label_14->setText(array[0]);
            QString name;
            name=ui->listWidget->currentItem()->text();
            QSqlQuery query;
            if(sub_time_loop==1)
                query.prepare("select wait1,mix1,mag1 FROM DNA where name=:name");
            else if(sub_time_loop==2)
                query.prepare("select wait2,mix2,mag2 FROM DNA where name=:name");
            else if(sub_time_loop==3)
                query.prepare("select wait3,mix3,mag3 FROM DNA where name=:name");
            else if(sub_time_loop==4)
                query.prepare("select wait4,mix4,mag4 FROM DNA where name=:name");
            else if(sub_time_loop==5)
                query.prepare("select wait5,mix5,mag5 FROM DNA where name=:name");
            else if(sub_time_loop==6)
                query.prepare("select wait6,mix6,mag6 FROM DNA where name=:name");
            else if(sub_time_loop==7)
                query.prepare("select wait7,mix7,mag7 FROM DNA where name=:name");
            query.bindValue(":name",name);
            query.exec();
            while(query.next())
            {
                ui->lineEdit_146->setText(query.value(0).toString());
                ui->lineEdit_147->setText(query.value(1).toString());
                ui->lineEdit_148->setText(query.value(2).toString());
                ui->lineEdit_146->setStyleSheet("QLineEdit{background-color:rgb(0, 170, 255);}");
                ui->lineEdit_147->setStyleSheet("QLineEdit{background-color:rgb(0, 170, 255);}");
                ui->lineEdit_148->setStyleSheet("QLineEdit{background-color:rgb(0, 170, 255);}");
            }
            sub_time_loop=sub_time_loop+1;
            for(int i=0;i<array_len;i++)
            {
                array[i]=array[i+1];
            }
            array[array_len]='\0';
            array_len=array_len-1;
            //processing();
            into_pname=1;

        }

        else if(array[0][0]=='W')
        {
            Pi2c arduino(8);
            char* ch;
            QByteArray ba=array[0].toLatin1();
            ch=ba.data();
            //timer1->stop();
            QThread::msleep(100);
            arduino.i2cWrite(ch,30);
            QThread::msleep(100);
            //timer1->start(1000);
            qDebug()<<array[0];
            for(int i=0;i<array_len;i++)
            {
                array[i]=array[i+1];
            }
            array[array_len]='\0';
            array_len=array_len-1;
            into_pname=1;
        }

        else
        {
            Pi2c arduino(7);
            char* ch;
            QByteArray ba=array[0].toLatin1();
            ch=ba.data();
            //timer1->stop();
            QThread::msleep(100);
            arduino.i2cWrite(ch,30);
            QThread::msleep(100);
            char rc[30];
            arduino.i2cRead(rc,30);
            qDebug()<<"sent="<<ch<<" received="<<rc;
            QString str1=ch;
            QString str2=rc;
            qDebug()<<str1<<str2;

            while(str1!=str2)
            {
                qDebug()<<"Command Mis Matched";
                QThread::msleep(100);
                arduino.i2cWrite(ch,30);
                QThread::msleep(100);
                arduino.i2cRead(rc,30);
                QThread::msleep(100);
                str1=ch;
                str2=rc;
                qDebug()<<str1<<str2;
            }


            qDebug()<<array[0];

            //timer val
            if(array[0].left(3)=="idl" || array[0].left(3)== "mix" || array[0].left(3)== "mis" || array[0].left(3)== "mag")
            {
                //timer_val=array[0].mid(4,4).toInt();
                ui->label_35->hide();
                proctimer1->stop();
                QStringList elements = array[0].split(' ');
                //qDebug()<<elements[0]<<" "<<elements[1];
                timer_val=elements[1].toInt();
                if(timer_val>=2)
                {
                    if(array[0].left(3)=="idl")
                        proc_countdown=1;
                    else if (array[0].left(3)== "mix" || array[0].left(3)== "mis")
                        proc_countdown=2;
                    else if(array[0].left(3)== "mag")
                        proc_countdown=3;
                    proctimer->start(1000);
                }
            }
            else
            {
                timer_val=1;
                proctimer1->start(1000);
            }

            for(int i=0;i<array_len;i++)
            {
                array[i]=array[i+1];
            }
            array[array_len]='\0';
            array_len=array_len-1;
        }
        if(array_len>=0)
        {
            if(timer_val>0)
                timer->start((timer_val*1000)-1000);
            else
                timer->start(500);
        }
        else
        {
            Pi2c arduino(8);
            QString data="W end";
            char* ch;
            QByteArray ba=data.toLatin1();
            ch=ba.data();
            QThread::msleep(100);
            arduino.i2cWrite(ch,30);
            QThread::msleep(100);
            array_len=0;
            timer->stop();
            timer1->stop();
            timer2->stop();
            timer3->stop();
            proctimer->stop();
            proctimer1->stop();
            ui->toolButton_18->setText("Start");
            ui->stackedWidget->setCurrentIndex(0);
            ui->listWidget->setCurrentRow(0);
            ui->label_18->hide();
            ui->label_35->hide();
            ui->label_91->hide();
            ui->toolButton->setVisible(true);
            ui->toolButton_2->setVisible(true);
            ui->toolButton_8->setVisible(true);

        }
    }
    else
    {
        goto restart;
    }

}


void MainWindow::testing()
{
    if(into_pname==1)
    {
        into_pname=0;
        processing();
    }
    else
    {
        timer1->stop();
        Pi2c arduino(7);
        char receive[30];
        QThread::msleep(100);
        arduino.i2cRead(receive,30);
        QThread::msleep(100);
        qDebug() <<"FROM MOTOR BOARD : "<< receive;
        if(strncmp(receive,"done",4)==0)
        {
            timer->stop();
            timer1->start(1000);
            if(array_len>=0)
            {
                processing();
            }
        }
        else
        {
            timer->start(500);
        }
    }
}


void MainWindow::init_motor()
{
    timer2->stop();
    Pi2c arduino(7);
    char receive[30];
    QThread::msleep(100);
    arduino.i2cRead(receive,30);
    QThread::msleep(100);
    qDebug()<<receive;
    if(strncmp(receive,"done",4)==0)
    {

        //        QSqlQuery query;
        //        query.prepare("SELECT bmh, mmh, smh FROM config where sno=1");
        //        query.exec();
        //        QString bmh,mmh,smh;
        //        while(query.next())
        //        {
        //            bmh=query.value(0).toString();
        //            mmh=query.value(1).toString();
        //            smh=query.value(2).toString();
        //        }

        //        QString data="cmd "+bmh+" "+mmh+" "+smh;
        //        char* ch;
        //        qDebug()<<data;
        //        QByteArray ba=data.toLatin1();
        //        ch=ba.data();
        //        QThread::msleep(100);
        //        arduino.i2cWrite(ch,30);
        //        QThread::msleep(100);

        ui->label_93->setVisible(true);
        ui->label_94->setVisible(true);
        ui->label_95->hide();
        ui->toolButton_45->hide();
        ui->label_93->setText("Initializing Motors...");
        QString data="ini";
        char* ch;
        QByteArray ba=data.toLatin1();
        ch=ba.data();
        QThread::msleep(100);
        arduino.i2cWrite(ch,30);
        QThread::msleep(100);
        timer3->start(500);
    }
}

void MainWindow::on_toolButton_45_clicked()
{
    init_motor();
}

void MainWindow::init()
{
    Pi2c arduino(7);
    char receive[30];
    QThread::msleep(100);
    arduino.i2cRead(receive,30);
    QThread::msleep(100);
    qDebug()<<receive;
    if(strncmp(receive,"done",4)==0)
    {
        timer3->stop();
        ui->stackedWidget->setCurrentIndex(0);
        ui->toolButton->setVisible(true);
        ui->toolButton_2->setVisible(true);
        ui->toolButton_8->setVisible(true);
        ui->label_91->hide();

    }
    else if(strncmp(receive,"merr",4)==0)
    {
        timer3->stop();
        ui->label_4->setText("Motor Init Error");
        ui->label_4->setStyleSheet("QLabel{color:red;}");
        ui->stackedWidget->setCurrentIndex(0);
        ui->toolButton_6->setDisabled(true);
        ui->toolButton->setVisible(true);
        ui->toolButton_2->setVisible(true);
        ui->toolButton_8->setVisible(true);
        ui->label_91->hide();
    }
    else if(strncmp(receive,"serr",4)==0)
    {
        timer3->stop();
        ui->label_4->setText("Motor Init Error");
        ui->label_4->setStyleSheet("QLabel{color:red;}");
        ui->stackedWidget->setCurrentIndex(0);
        ui->toolButton_6->setDisabled(true);
        ui->toolButton->setVisible(true);
        ui->toolButton_2->setVisible(true);
        ui->toolButton_8->setVisible(true);
        ui->label_91->hide();
    }
    else if(strncmp(receive,"berr",4)==0)
    {
        timer3->stop();
        ui->label_4->setText("Motor Init Error");
        ui->label_4->setStyleSheet("QLabel{color:red;}");
        ui->stackedWidget->setCurrentIndex(0);
        ui->toolButton_6->setDisabled(true);
        ui->toolButton->setVisible(true);
        ui->toolButton_2->setVisible(true);
        ui->toolButton_8->setVisible(true);
        ui->label_91->hide();
    }
}

void MainWindow::on_toolButton_46_clicked()
{
    processing();
}

void MainWindow::on_toolButton_18_clicked()
{
    ui->label_18->setVisible(true);
    QString s=ui->toolButton_18->text();
    if(s=="Start")
    {
        ui->toolButton_18->setText("Stop");
        ui->toolButton_18->setStyleSheet("QToolButton{background-color:#ff0000;}");
        ui->toolButton->hide();
        ui->toolButton_2->hide();
        ui->toolButton_8->hide();
        processing();
    }
    else if(s=="Stop")
    {
        QMessageBox msgBox;
        //msgBox.setWindowFlags(Qt::Window | Qt::FramelessWindowHint);
        msgBox.setWindowTitle("Warning...");
        msgBox.setText("Confirm Stop the experiment");
        msgBox.setStandardButtons(msgBox.Yes);
        msgBox.addButton(msgBox.No);
        msgBox.setStyleSheet("QLabel{min-width:500 px; font-size: 24px;} QPushButton{ width:200px; height:50px; font-size: 18px; }");
        if(msgBox.exec() == msgBox.Yes)
        {
            Pi2c arduino(7);
            QString data="stp";
            char* ch;
            QByteArray ba=data.toLatin1();
            ch=ba.data();
            QThread::msleep(100);
            arduino.i2cWrite(ch,30);
            QThread::msleep(100);
            Pi2c arduino1(8);
            data="W end";
            ba=data.toLatin1();
            ch=ba.data();
            QThread::msleep(100);
            arduino1.i2cWrite(ch,30);
            QThread::msleep(100);
            array_len=0;
            timer->stop();
            timer1->stop();
            timer2->stop();
            timer3->stop();
            proctimer->stop();
            proctimer1->stop();
            ui->toolButton_18->setText("Start");
            ui->toolButton_18->setStyleSheet("QToolButton{background-color:rgb(1, 114, 192);}");
            ui->stackedWidget->setCurrentIndex(0);
            ui->listWidget->setCurrentRow(0);
            ui->label_35->hide();
            ui->label_18->hide();
            ui->label_91->hide();
            ui->toolButton->setVisible(true);
            ui->toolButton_2->setVisible(true);
            ui->toolButton_8->setVisible(true);
        }
    }
}


void MainWindow::on_toolButton_43_clicked()
{
    QString uvdur=ui->lineEdit_144->text();
    QString s=ui->toolButton_43->text();
    if(s=="Turn ON UV LAMP")
    {
        ui->toolButton_43->setText("Turn OFF UV LAMP");
        ui->toolButton->setDisabled(true);
        ui->toolButton_2->setDisabled(true);
        ui->toolButton_8->setDisabled(true);
        Pi2c arduino(7);
        QString data="UVN "+uvdur;
        char* ch;
        QByteArray ba=data.toLatin1();
        ch=ba.data();
        QThread::msleep(100);
        arduino.i2cWrite(ch,30);
        QThread::msleep(100);
        uvtimer = new QTimer(this);
        connect(uvtimer, SIGNAL(timeout()), this,SLOT(uv_timer()));
        uvtimer->start(1000);

    }
    else if(s=="Turn OFF UV LAMP")
    {
        Pi2c arduino(7);
        QString data="UVF";
        char* ch;
        QByteArray ba=data.toLatin1();
        ch=ba.data();
        QThread::msleep(100);
        arduino.i2cWrite(ch,30);
        QThread::msleep(100);
        uvtimer->stop();
        ui->toolButton_43->setText("Turn ON UV LAMP");
        ui->stackedWidget->setCurrentIndex(0);
        ui->toolButton->setDisabled(false);
        ui->toolButton_2->setDisabled(false);
        ui->toolButton_8->setDisabled(false);
    }


}

void MainWindow::uv_timer()
{
    int uvdur=ui->lineEdit_144->text().toInt();
    uvdur=uvdur-1;
    ui->lineEdit_144->setText(QString::number(uvdur));
    if(uvdur%2==0)
    {
        ui->lineEdit_144->setStyleSheet("QLineEdit{background-color:rgb(255, 255, 255);}");
    }
    else
    {
        ui->lineEdit_144->setStyleSheet("QLineEdit{background-color:rgb(0, 170, 255);}");

    }
    if(uvdur<0)
    {
        uvtimer->stop();
        ui->toolButton_43->setText("Turn ON UV LAMP");
        ui->toolButton->setDisabled(false);
        ui->toolButton_2->setDisabled(false);
        ui->toolButton_8->setDisabled(false);
        ui->stackedWidget->setCurrentIndex(0);
    }

}

void MainWindow::proc_timer()
{
    total_time=total_time-1;
    ui->lineEdit_149->setText(QDateTime::fromTime_t(total_time).toUTC().toString("hh:mm:ss"));
    if(total_time%2==0)
    {
        ui->lineEdit_149->setStyleSheet("QLineEdit {background-color:rgb(0, 77, 116);font-weight: bold;color:rgb(255, 255, 255);border-radius:5%;border: 4px solid rgb(0, 77, 166);}");
    }
    else
    {
        ui->lineEdit_149->setStyleSheet("QLineEdit {background-color:rgb(255,255,255);font-weight: bold;color:rgb(0, 77, 116);border-radius:5%;border: 4px solid rgb(0, 77, 166);}");
    }
    int dur=ui->lineEdit_146->text().toInt();
    int dur1=ui->lineEdit_147->text().toInt();
    int dur2=ui->lineEdit_148->text().toInt();
    if(dur==0)
        ui->lineEdit_146->setStyleSheet("QLineEdit{background-color:rgb(255, 255, 255);}");
    if(dur1==0)
        ui->lineEdit_147->setStyleSheet("QLineEdit{background-color:rgb(255, 255, 255);}");
    if(dur2==0)
        ui->lineEdit_148->setStyleSheet("QLineEdit{background-color:rgb(255, 255, 255);}");

    if(proc_countdown==1)
    {
        int dur=ui->lineEdit_146->text().toInt();
        dur=dur-1;
        ui->lineEdit_146->setText(QString::number(dur));
        if(dur%2==0)
        {
            ui->lineEdit_146->setStyleSheet("QLineEdit{background-color:rgb(255, 255, 255);}");
        }
        else
        {
            ui->lineEdit_146->setStyleSheet("QLineEdit{background-color:rgb(0, 170, 255);}");

        }
        if(dur<=0)
        {
            proctimer->stop();
            ui->lineEdit_146->setText(QString::number(0));
        }
    }
    else if(proc_countdown==2)
    {
        int dur=ui->lineEdit_147->text().toInt();
        dur=dur-1;
        ui->lineEdit_147->setText(QString::number(dur));
        if(dur%2==0)
        {
            ui->lineEdit_147->setStyleSheet("QLineEdit{background-color:rgb(255, 255, 255);}");
        }
        else
        {
            ui->lineEdit_147->setStyleSheet("QLineEdit{background-color:rgb(0, 170, 255);}");

        }
        if(dur<=0)
        {
            proctimer->stop();
            ui->lineEdit_147->setText(QString::number(0));
        }
    }
    else if(proc_countdown==3)
    {
        int dur=ui->lineEdit_148->text().toInt();
        dur=dur-1;
        ui->lineEdit_148->setText(QString::number(dur));
        if(dur%2==0)
        {
            ui->lineEdit_148->setStyleSheet("QLineEdit{background-color:rgb(255, 255, 255);}");
        }
        else
        {
            ui->lineEdit_148->setStyleSheet("QLineEdit{background-color:rgb(0, 170, 255);}");

        }
        if(dur<=0)
        {
            proctimer->stop();
            ui->lineEdit_148->setText(QString::number(0));
        }
    }
}

void MainWindow::proc_timer1()
{
    proc1_countdown+=1;
    ui->label_35->setVisible(true);
    if(proc1_countdown>1)
    {
        proc1_countdown=0;
        ui->label_35->hide();
    }

}


void MainWindow::on_toolButton_7_clicked()
{
    clicks+=1;
    if(clicks==3)
        qApp->exit();
}

void MainWindow::on_pushButton_167_clicked()
{
    ui->stackedWidget_2->setCurrentIndex(3);

}

void MainWindow::on_pushButton_168_clicked()
{
    ui->stackedWidget_2->setCurrentIndex(3);
}

void MainWindow::on_pushButton_182_clicked()
{
    ui->stackedWidget_2->setCurrentIndex(0);
}

void MainWindow::on_pushButton_183_clicked()
{
    ui->stackedWidget_2->setCurrentIndex(2);
}

void MainWindow::on_pushButton_175_clicked()
{
    ui->lineEdit_145->backspace();

}

void MainWindow::on_pushButton_176_clicked()
{
    on_pushButton_26_clicked();

}

void MainWindow::on_pushButton_172_clicked()
{
    ui->lineEdit_145->setText(ui->lineEdit_145->text()+ui->pushButton_172->text());

}

void MainWindow::on_pushButton_170_clicked()
{
    ui->lineEdit_145->setText(ui->lineEdit_145->text()+ui->pushButton_170->text());

}

void MainWindow::on_pushButton_178_clicked()
{
    ui->lineEdit_145->setText(ui->lineEdit_145->text()+ui->pushButton_178->text());

}

void MainWindow::on_pushButton_180_clicked()
{
    ui->lineEdit_145->setText(ui->lineEdit_145->text()+ui->pushButton_180->text());

}

void MainWindow::on_pushButton_173_clicked()
{
    ui->lineEdit_145->setText(ui->lineEdit_145->text()+ui->pushButton_173->text());

}

void MainWindow::on_pushButton_171_clicked()
{
    ui->lineEdit_145->setText(ui->lineEdit_145->text()+"&");

}

void MainWindow::on_pushButton_179_clicked()
{
    ui->lineEdit_145->setText(ui->lineEdit_145->text()+ui->pushButton_179->text());

}

void MainWindow::on_pushButton_169_clicked()
{
    ui->lineEdit_145->setText(ui->lineEdit_145->text()+ui->pushButton_169->text());

}

void MainWindow::on_pushButton_181_clicked()
{
    ui->lineEdit_145->setText(ui->lineEdit_145->text()+ui->pushButton_181->text());

}

void MainWindow::on_pushButton_177_clicked()
{
    ui->lineEdit_145->setText(ui->lineEdit_145->text()+ui->pushButton_177->text());

}

void MainWindow::on_toolButton_47_clicked()
{
    ui->stackedWidget->setCurrentIndex(9);
    ui->label_36->setVisible(true);
    ui->page_14->setDisabled(true);
    qApp->processEvents();
    QStringList list2;
    QProcess process1;
    process1.start("sh",QStringList()<<"-c"<<"sudo iwlist wlan0 scan | grep ESSID");//scan list of wifi networks
    process1.waitForFinished();
    //ui->page_12->setVisible(true);
    ui->label_36->hide();
    ui->page_14->setDisabled(false);
    QString data = process1.readAllStandardOutput();
    QString Error= process1.readAllStandardError();
    ui->comboBox_2->clear();
    list2 = (QStringList()<<"------Select-------");//append to dropdownlist
    ui->comboBox_2->addItems(list2);
    QStringList list = data.split("\n");//split data
    for(int i=0;i<list.count()-1;i++)
    {
        QStringList list1 = list.at(i).split("ESSID:");
        QString data1 = list1.at(1);
        list2 = (QStringList()<<data1);
        ui->comboBox_2->addItems(list2);//adding wifi names to dropdownlist
    }
    process1.start("sh",QStringList()<<"-c"<<"hostname -I");//scan list of wifi networks
    process1.waitForFinished();
    data = process1.readAllStandardOutput();
    ui->lineEdit_5->setText(data);
}

void MainWindow::on_toolButton_8_clicked()
{
    ui->stackedWidget->setCurrentIndex(8);
    ui->label_91->hide();
    timer1->stop();
}

void MainWindow::on_pushButton_10_clicked()
{
    opt=45;
    ui->stackedWidget->setCurrentIndex(5);
    ui->stackedWidget_2->setCurrentIndex(0);
    ui->lineEdit_145->setText(ui->lineEdit_2->text());
}

void MainWindow::on_pushButton_35_clicked()
{
    QFile file("/etc/wpa_supplicant/wpa_supplicant.conf");
    if(file.open(QIODevice::WriteOnly | QIODevice::Text))
    {
        QTextStream stream(&file);
        stream<<"ctrl_interface=DIR=/var/run/wpa_supplicant GROUP=netdev\n";
        stream<<"update_config=1\n";
        stream<<"country=IN\n";
        stream<<"\n";
        stream<<"network={\n";
        stream<<"\tssid=";
        stream<<ui->comboBox_2->currentText()+"\n";
        stream<<"\tpsk=\"";
        stream<<ui->lineEdit_2->text()+"\"\n";
        stream<<"\tkey_mgmt=WPA-PSK\n";
        stream<<"}";
        file.close();
        QProcess process2;
        process2.start("sh",QStringList()<<"-c"<<"sudo wpa_cli -i wlan0 reconfigure");
        process2.waitForFinished();
    }
}

void MainWindow::on_pushButton_36_clicked()
{
    QProcess process1;
    process1.start("sh",QStringList()<<"-c"<<"hostname -I");//scan list of wifi networks
    process1.waitForFinished();
    QString data = process1.readAllStandardOutput();
    ui->lineEdit_5->setText(data);
}

void MainWindow::on_toolButton_48_clicked()
{
    ui->stackedWidget->setCurrentIndex(10);
    QSqlQuery query;
    query.prepare("SELECT bmh, mmh, smh FROM config where sno=1");
    query.exec();
    while(query.next())
    {
        ui->lineEdit_113->setText(query.value(0).toString());
        ui->lineEdit_114->setText(query.value(1).toString());
        ui->lineEdit_115->setText(query.value(2).toString());
    }

}

void MainWindow::on_toolButton_49_clicked()
{
    ui->stackedWidget->setCurrentIndex(11);
    timer1->start(1000);
}

void MainWindow::on_pushButton_37_clicked()
{
    ui->page_15->setDisabled(true);
    Pi2c arduino(7);
    QString data="ini";
    char* ch;
    QByteArray ba=data.toLatin1();
    ch=ba.data();
    QThread::msleep(100);
    arduino.i2cWrite(ch,30);
    QThread::msleep(100);
    mtesttimer->start(500);
}




void MainWindow::motor_test()
{
    Pi2c arduino(7);
    char receive[30];
    QThread::msleep(100);
    arduino.i2cRead(receive,30);
    QThread::msleep(100);
    qDebug()<<receive;
    if(strncmp(receive,"done",4)==0)
    {
        mtesttimer->stop();
        ui->lineEdit_112->setText("Pass");
        ui->page_15->setDisabled(false);

    }
    else if(strncmp(receive,"merr",4)==0)
    {
        mtesttimer->stop();
        ui->lineEdit_112->setText("Magnet Motor Error");
        ui->page_15->setDisabled(false);
    }
    else if(strncmp(receive,"serr",4)==0)
    {
        mtesttimer->stop();
        ui->lineEdit_112->setText("Sleeve Motor Error");
        ui->page_15->setDisabled(false);
    }
    else if(strncmp(receive,"berr",4)==0)
    {
        mtesttimer->stop();
        ui->lineEdit_112->setText("Base Motor Error");
        ui->page_15->setDisabled(false);
    }
}


void MainWindow::on_pushButton_323_clicked()
{
    QString s=ui->pushButton_323->text();
    if(s=="Release Base Motor")
    {
        hold_base=1;
        ui->pushButton_326->setDisabled(true);
        ui->pushButton_323->setText("Hold Base Motor");
        Pi2c arduino(7);
        QString data="rbs";
        char* ch;
        QByteArray ba=data.toLatin1();
        ch=ba.data();
        QThread::msleep(100);
        arduino.i2cWrite(ch,30);
        QThread::msleep(100);
    }
    else
    {
        hold_base=0;
        if(hold_base==0 && hold_sleeve==0 && hold_magnet==0)
            ui->pushButton_326->setDisabled(false);
        ui->pushButton_323->setText("Release Base Motor");
        Pi2c arduino(7);
        QString data="hbs";
        char* ch;
        QByteArray ba=data.toLatin1();
        ch=ba.data();
        QThread::msleep(100);
        arduino.i2cWrite(ch,30);
        QThread::msleep(100);
    }

}


void MainWindow::on_pushButton_324_clicked()
{
    QString s=ui->pushButton_324->text();
    if(s=="Release Magnet Motor")
    {
        hold_magnet=1;
        ui->pushButton_326->setDisabled(true);
        ui->pushButton_324->setText("Hold Magnet Motor");
        Pi2c arduino(7);
        QString data="rmg";
        char* ch;
        QByteArray ba=data.toLatin1();
        ch=ba.data();
        QThread::msleep(100);
        arduino.i2cWrite(ch,30);
        QThread::msleep(100);
    }
    else
    {
        hold_magnet=0;
        if(hold_base==0 && hold_sleeve==0 && hold_magnet==0)
            ui->pushButton_326->setDisabled(false);
        ui->pushButton_324->setText("Release Magnet Motor");
        Pi2c arduino(7);
        QString data="hmg";
        char* ch;
        QByteArray ba=data.toLatin1();
        ch=ba.data();
        QThread::msleep(100);
        arduino.i2cWrite(ch,30);
        QThread::msleep(100);
    }
}

void MainWindow::on_pushButton_325_clicked()
{
    QString s=ui->pushButton_325->text();
    if(s=="Release Sleeve Motor")
    {
        hold_sleeve=1;
        ui->pushButton_326->setDisabled(true);
        ui->pushButton_325->setText("Hold Sleeve Motor");
        Pi2c arduino(7);
        QString data="rsl";
        char* ch;
        QByteArray ba=data.toLatin1();
        ch=ba.data();
        QThread::msleep(100);
        arduino.i2cWrite(ch,30);
        QThread::msleep(100);
    }
    else
    {
        hold_sleeve=0;
        if(hold_base==0 && hold_sleeve==0 && hold_magnet==0)
            ui->pushButton_326->setDisabled(false);
        ui->pushButton_325->setText("Release Sleeve Motor");
        Pi2c arduino(7);
        QString data="hsl";
        char* ch;
        QByteArray ba=data.toLatin1();
        ch=ba.data();
        QThread::msleep(100);
        arduino.i2cWrite(ch,30);
        QThread::msleep(100);
    }
}

void MainWindow::on_pushButton_326_clicked()
{
    ui->page_15->setDisabled(true);
    Pi2c arduino(7);
    QString data="cal";
    char* ch;
    QByteArray ba=data.toLatin1();
    ch=ba.data();
    QThread::msleep(100);
    arduino.i2cWrite(ch,30);
    QThread::msleep(100);
    mtesttimer1->start(500);
}


void MainWindow::motor_test1()
{
    Pi2c arduino(7);
    char receive[30];
    QThread::msleep(100);
    arduino.i2cRead(receive,30);
    QThread::msleep(100);
    qDebug()<<receive;
    if(strncmp(receive,"caldone",7)==0)
    {
        mtesttimer1->stop();
        QString str=receive;
        QStringList list;
        list=str.split(" ");
        ui->lineEdit_113->setText(list[1]);
        ui->lineEdit_114->setText(list[2]);
        ui->lineEdit_115->setText(list[3]);
        QSqlQuery query;
        query.prepare("update config set bmh=:bmh,mmh=:mmh,smh=:smh where sno=1");
        query.bindValue(":bmh",list[1]);
        query.bindValue(":mmh",list[2]);
        query.bindValue(":smh",list[3]);
        query.exec();
        ui->lineEdit_112->setText("Pass");
        ui->page_15->setDisabled(false);
    }
    else if(strncmp(receive,"merr",4)==0)
    {
        mtesttimer1->stop();
        ui->lineEdit_112->setText("Magnet Motor Error");
        ui->page_15->setDisabled(false);
    }
    else if(strncmp(receive,"serr",4)==0)
    {
        mtesttimer1->stop();
        ui->lineEdit_112->setText("Sleeve Motor Error");
        ui->page_15->setDisabled(false);
    }
    else if(strncmp(receive,"berr",4)==0)
    {
        mtesttimer1->stop();
        ui->lineEdit_112->setText("Base Motor Error");
        ui->page_15->setDisabled(false);
    }

}



void MainWindow::on_pushButton_322_clicked()
{
    QString s=ui->pushButton_322->text();
    QString t=ui->lineEdit_111->text();
    QString data;
    if(s=="Start Heating")
    {
        ui->pushButton_322->setText("Stop Heating");
        ui->toolButton->setDisabled(true);
        ui->toolButton_2->setDisabled(true);
        ui->toolButton_8->setDisabled(true);
        if(ui->comboBox_47->currentText()=="Lysis")
            data="W lys "+t;
        else if(ui->comboBox_47->currentText()=="Elution")
            data="W elu "+t;
        Pi2c arduino(8);
        char* ch;
        QByteArray ba=data.toLatin1();
        ch=ba.data();
        QThread::msleep(100);
        arduino.i2cWrite(ch,30);
        QThread::msleep(100);
    }
    else if(s=="Stop Heating")
    {
        ui->pushButton_322->setText("Start Heating");
        ui->toolButton->setDisabled(false);
        ui->toolButton_2->setDisabled(false);
        ui->toolButton_8->setDisabled(false);
        data="W end";
        Pi2c arduino(8);
        char* ch;
        QByteArray ba=data.toLatin1();
        ch=ba.data();
        QThread::msleep(100);
        arduino.i2cWrite(ch,30);
        QThread::msleep(100);
    }
}

void MainWindow::on_pushButton_329_clicked()
{
    opt=46;
    ui->stackedWidget->setCurrentIndex(5);
    ui->stackedWidget_2->setCurrentIndex(2);
    ui->lineEdit_145->setText(ui->lineEdit_111->text());
}

void MainWindow::on_toolButton_10_clicked()
{
    ui->stackedWidget->setCurrentIndex(0);
    ui->toolButton->setVisible(true);
    ui->toolButton_2->setVisible(true);
    ui->toolButton_8->setVisible(true);
    ui->label_91->hide();
}
