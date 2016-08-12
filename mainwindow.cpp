#include "mainwindow.h"
#include "ui_mainwindow.h"
#include <QBitmap>
#include <QPixmap>
#include <form_clock.h>
#include <QInputDialog>
#include <QMenu>
#include <QFileDialog>
#include <QThread>
#include <QFile>
//#include <windows.h>
#include <QTableWidgetItem>
#include <QMessageBox>
#include <QDebug>
#include <QDataStream>
MainWindow::MainWindow(QWidget *parent) :
    QMainWindow(parent),
    ui(new Ui::MainWindow)
{

    ui->setupUi(this);
    ui->table_log->setContextMenuPolicy(Qt::CustomContextMenu);

    //ui->table_log->setEditTriggers(QAbstractItemView::NoEditTriggers);
    ui->tableWidget_profile_result->setEditTriggers(QAbstractItemView::NoEditTriggers);
    // ui->tableWidget_profile_result->set
    setWindowFlags(Qt::FramelessWindowHint);//设置窗体无边框
    //this->setAttribute(Qt::WA_TranslucentBackground);//设置背景透明
    d_setting = NULL;
    //设置背景
    QPixmap pixmap(":/res/bk.jpg");

    QPalette palette;

    palette.setBrush(backgroundRole(), QBrush(pixmap));

    setPalette(palette);

    //设置窗体的掩码图像,抠除图像的白色区域实现不规则窗体

    // QPixmap  mask(":/res/mask1.png");

    // setMask(QBitmap(mask.mask()));

    QPixmap  b_mask(":/res/exit.png");
    //    ui->Button_Exit->setMask(QBitmap(b_mask.mask()));
    //    ui->Button_Start->setMask(QBitmap(b_mask.mask()));
    //    ui->Button_Stop->setMask(QBitmap(b_mask.mask()));
    //    ui->Button_Setting->setMask(QBitmap(b_mask.mask()));
    ui->pushButton_Exit->setMask(QBitmap(b_mask.mask()));


    ui->lineEdit_step->setStyleSheet("QLineEdit{background-color: rgba(0,0,0,0);border-width:0;border-style:outset}");
    ui->lineEdit_pattern->setStyleSheet("QLineEdit{background-color: rgba(0,0,0,0);border-width:0;border-style:outset}");
    ui->lineEdit_now_hour->setStyleSheet("QLineEdit{background-color: rgba(0,0,0,0);border-width:0;border-style:outset}");
    ui->lineEdit_now_min->setStyleSheet("QLineEdit{background-color: rgba(0,0,0,0);border-width:0;border-style:outset}");
    ui->lineEdit_settimeh->setStyleSheet("QLineEdit{background-color: rgba(0,0,0,0);border-width:0;border-style:outset}");
    ui->lineEdit_settimem->setStyleSheet("QLineEdit{background-color: rgba(0,0,0,0);border-width:0;border-style:outset}");
    ui->lineEdit_sethum->setStyleSheet("QLineEdit{background-color: rgba(0,0,0,0);border-width:0;border-style:outset}");
    ui->lineEdit_settem->setStyleSheet("QLineEdit{background-color: rgba(0,0,0,0);border-width:0;border-style:outset}");
    ui->lineEdit_shark->setStyleSheet("QLineEdit{background-color: rgba(0,0,0,0);border-width:0;border-style:outset}");
    ui->lineEdit_power->setStyleSheet("QLineEdit{background-color: rgba(0,0,0,0);border-width:0;border-style:outset}");
    ui->lineEdit_defrost->setStyleSheet("QLineEdit{background-color: rgba(0,0,0,0);border-width:0;border-style:outset}");


    ui->lineEdit_step_desc->setWordWrap(true);
    ui->lineEdit_step_desc->setAlignment(Qt::AlignTop);

    // 信号与槽
    connect(ui->commandLinkButton_Start, SIGNAL(clicked()), this, SLOT(on_Button_Start_clicked()));
    connect(ui->commandLinkButton_Stop, SIGNAL(clicked()), this, SLOT(on_Button_Stop_clicked()));
    connect(ui->commandLinkButton_Setting, SIGNAL(clicked()), this, SLOT(on_Button_Setting_clicked()));
    connect(ui->pushButton_Exit, SIGNAL(clicked()), this, SLOT(on_Button_Exit_clicked()));


    Ui_Test_Init();
    init_chart();
    //setupMultiAxisDemo(ui->chart_show);
    ui->lcdNumber_TestTime->display("00:00:00");

    timer = new QTimer(this);

    connect(timer, SIGNAL(timeout()), this, SLOT(timer_Update()));
    connect(this, SIGNAL(add_testinginfo(QString )), this, SLOT(change_testinginfo(QString)),Qt::AutoConnection);
    connect(this, SIGNAL(add_title_testinginfo(QString )), this, SLOT(change_title_testinginfo(QString)),Qt::AutoConnection);
    connect(&etest, SIGNAL(add_updateGraph(QString , double , double )), this, SLOT(updateGraph(QString,double ,double )),Qt::AutoConnection);
    connect(&etest, SIGNAL(finished()), this, SLOT(Test_Finished()));

    connect(this, SIGNAL(close()), this, SLOT(on_Button_Exit_clicked()));
    //初始化
    erot_info.goal_humidity=0;
    erot_info.goal_temperature=0;
    erot_info.now_humidity=0;
    erot_info.now_temperature=0;

    //Testing_Status::
    testing_status = stop;
    test_time = 0;
    erot_info.fail_num=0;
    erot_info.pass_num=0;

    //log info

    log_info.rows = 0;
    log_info.head_name<<"Date"<<"SN"<<"Model"<<"Result"<<"Time"<<"Measure Temperature"<<"Set Temperature"<<"Measure Humidity"<<"Set Humidity";                        ///>    init  初始化log头
    log_info.cols= log_info.head_name.count();

    Performance_Summary_Report.rows = 0;
    Performance_Summary_Report.head_name <<"Test Date"<<"Model Name"<<"Serial Number"<<"Peak Operation Temp"<<"Lowest Operation Temp"<<"EORT Result";
    Performance_Summary_Report.cols= Performance_Summary_Report.head_name.count();

    //加载配置

    QFile file("./config/config.xml");
    if (!file.open(QIODevice::ReadOnly))
        return;
    if (! Global::config_doc.setContent(&file)) {
        file.close();
        return;
    }
    file.close();

    //:init
    init_dll();

    load_test_profile();

    ui->label_sv_version->setText(Global::Soft_Version);
}


void MainWindow::init_dll()
{
    dll_lib.setFileName("ChamberAPI_dll.dll");
    if (dll_lib.load())              //判断是否正确加载
    {
        //QMessageBox::information(NULL,"OK","DLL load is OK!");
        getInformation = (GetInformation)dll_lib.resolve("GetInformation");
        getStepSetInfo = (GetStepSetInfo)dll_lib.resolve("GetStepSetInfo");
        startTest= (StartTest)dll_lib.resolve("StartTest");
        stopTest= (StopTest)dll_lib.resolve("StopTest");
        startShake= (StartShake)dll_lib.resolve("StartShake");
        stopShake= (StopShake)dll_lib.resolve("StopShake");
        oper_Run= (Oper_Run)dll_lib.resolve("Oper_Run");
        oper_Stop= (Oper_Stop)dll_lib.resolve("Oper_Stop");
        oper_HoldOn= (Oper_HoldOn)dll_lib.resolve("Oper_HoldOn");
        oper_KeepOn= (Oper_KeepOn)dll_lib.resolve("Oper_KeepOn");
        oper_Advence= (Oper_Advence)dll_lib.resolve("Oper_Advence");
    }
    else

        QMessageBox::information(NULL,"NO", dll_lib.errorString());

}

void MainWindow::load_test_profile()
{

    Global::test_profile.Name = "profile_1";

    QFile file("./config/test_profile.xml");
    QDomDocument   dd_test;
    if (!file.open(QIODevice::ReadOnly))
    {
        QMessageBox::information(NULL,"NO", QString::fromLocal8Bit("找不到测试模版文件"));
    }
    if (! dd_test.setContent(&file))
    {
        QMessageBox::information(NULL,"NO",QString::fromLocal8Bit("加载测试模版文件失败"));
        file.close();
    }
    file.close();

    QDomNode node;
    QDomElement root = dd_test.documentElement();
    QDomNodeList  li = root.childNodes();
    for(int i=0; i<li.count(); i++)
    {
        node= li.at(i);
        if(  node.attributes().namedItem("Name").nodeValue() ==  Global::test_profile.Name )
        {
            break;
        }
    }

    li = node.childNodes();
    Global::test_profile.L_Test_Point.clear();
    Global::test_profile.max_temperature=25;
    Global::test_profile.min_temperature=25;
    for(int i=0; i<li.count(); i++)
    {
        node= li.at(i);
        Test_Point tp;
        QString time = node.attributes().namedItem("Time").nodeValue();
        //QStringList  ti = time.split("'");
        //  tp.time = ti[0].toInt()*60*60 + ti[1].toInt()*60;
        tp.time = time;
        tp.section = node.attributes().namedItem("Section").nodeValue().toInt();
        tp.temperature = node.attributes().namedItem("Temperature").nodeValue().toDouble();
        tp.humidity = node.attributes().namedItem("Humidity").nodeValue().toDouble();
        tp.shake = node.attributes().namedItem("Shake").nodeValue().toInt();
        tp.power = node.attributes().namedItem("Power").nodeValue() == "True" ? true:false;
        tp.flow = node.attributes().namedItem("Flow").nodeValue() == "True" ? true:false;
        tp.description = node.attributes().namedItem("Description").nodeValue() ;

        tp.pass_num = 0;
        tp.fail_num =0;

        if(tp.temperature > Global::test_profile.max_temperature)
        {
            Global::test_profile.max_temperature=tp.temperature;
        }

        if(tp.temperature < Global::test_profile.min_temperature)
        {
            Global::test_profile.min_temperature=tp.temperature;
        }

        Global::test_profile.L_Test_Point.append(tp);
    }

    for(int i=0; i<li.count(); i++)
    {
        qDebug()<<Global::test_profile.L_Test_Point[i].flow;
    }

}



MainWindow::~MainWindow()
{
    delete ui;
}

void MainWindow::Ui_Test_Init()
{
    ui->label_states->setStyleSheet("color: rgb(255, 0, 0);");

    ui->textEdit_testinginfo->setReadOnly(true);
    ui->textEdit_testinginfo->clear();
    test_time=0;

    erot_info.goal_temperature_graph_info.clear("goal_temperature");
    erot_info.now_temperature_graph_info.clear("real_temperature");
    erot_info.now_humidity_graph_info.clear("real_humidity");
    erot_info.goal_humidity_graph_info.clear("goal_humidity");
    erot_info.result_graph_info.clear("flow_result");
    erot_info.shake_graph_info.clear("vibration frequencyt");

    erot_info.tx_graph_info.clear("TX");
    erot_info.rx_graph_info.clear("RX");

    log_info.log_data.clear();
    log_info.rows = 0;
    ui->table_log->clear();


    Performance_Summary_Report.log_data.clear();
    Performance_Summary_Report.rows = 0;
    ui->tableWidget_sumary_report->clear();

    //    //log info

    //    log_info.rows = 0;
    //    log_info.head_name=QStringList();
    //   log_info.head_name<<"Date"<<"SN"<<"Model"<<"Result"<<"Time"<<"Measure Temperature"<<"Set Temperature"<<"Measure Humidity"<<"Set Humidity";                        ///>    init  初始化log头
    //   log_info.cols= log_info.head_name.count();


    //    Performance_Summary_Report.rows = 0;
    //    Performance_Summary_Report.head_name.clear();
    //    Performance_Summary_Report.head_name <<"Test Date"<<"Model Name"<<"Serial Number"<<"Peak Operation Temp"<<"Lowest Operation Temp"<<"EORT Result";
    //    Performance_Summary_Report.cols= Performance_Summary_Report.head_name.count();



    load_test_profile();
}

void MainWindow::Testing_Info(QString s)
{
    emit add_testinginfo(s);
}

void MainWindow::Title_Testing_Info(QString s)
{
    emit add_title_testinginfo(s);
}

void MainWindow::init_chart()
{

    //| QCP::iSelectAxes  |QCP::iSelectLegend | QCP::iSelectPlottables   | QCP::iSelectAxes
    ui->chart_show->setInteractions(QCP::iRangeDrag | QCP::iRangeZoom | QCP::iSelectAxes);
    ui->chart_show->xAxis->setRange(0, 1500);
    ui->chart_show->yAxis->setRange(-60, 110);
    //    ui->chart_show->xAxis2->setVisible(true);
    //    ui->chart_show->yAxis2->setVisible(true);
    //    ui->chart_show->yAxis2->setRange(0, 170);
    //    ui->chart_show->xAxis2->setRange(0, 1500);
    ui->chart_show->axisRect()->setupFullAxesBox();

    ui->chart_show->legend->setVisible(true);
    QFont legendFont = font();  // start out with MainWindow's font..
    legendFont.setPointSize(9); // and make a bit smaller for legend
    ui->chart_show->legend->setFont(legendFont);
    ui->chart_show->legend->setBrush(QBrush(QColor(255,255,255,230)));
    // by default, the legend is in the inset layout of the main axis rect. So this is how we access it to change legend placement:
    ui->chart_show->axisRect()->insetLayout()->setInsetAlignment(0, Qt::AlignTop|Qt::AlignLeft);


    ui->chart_show->plotLayout()->insertRow(0);
    ui->chart_show->plotLayout()->addElement(0, 0, new QCPPlotTitle(ui->chart_show, "EORT Chart T&W"));

    ui->chart_show->xAxis->setLabel("Time(s)");
    ui->chart_show->yAxis->setLabel(QString::fromLocal8Bit("°C / % / Hz"));
    //ui->chart_show->yAxis2->setLabel(QString::fromLocal8Bit("Hz"));
    //ui->chart_show->xAxis2->setLabel(QString::fromLocal8Bit("Time(s)"));


    // connect slot that ties some axis selections together (especially opposite axes):
    connect(ui->chart_show, SIGNAL(selectionChangedByUser()), this, SLOT(selectionChanged()));
    // connect slots that takes care that when an axis is selected, only that direction can be dragged and zoomed:
    connect(ui->chart_show, SIGNAL(mousePress(QMouseEvent*)), this, SLOT(mousePress()));
    connect(ui->chart_show, SIGNAL(mouseWheel(QWheelEvent*)), this, SLOT(mouseWheel()));

    // make bottom and left axes transfer their ranges to top and right axes:
    connect(ui->chart_show->xAxis, SIGNAL(rangeChanged(QCPRange)), ui->chart_show->xAxis2, SLOT(setRange(QCPRange)));
    connect(ui->chart_show->yAxis, SIGNAL(rangeChanged(QCPRange)), ui->chart_show->yAxis2, SLOT(setRange(QCPRange)));

    // connect some interaction slots:
    connect(ui->chart_show, SIGNAL(titleDoubleClick(QMouseEvent*,QCPPlotTitle*)), this, SLOT(titleDoubleClick(QMouseEvent*,QCPPlotTitle*)));
    connect(ui->chart_show, SIGNAL(axisDoubleClick(QCPAxis*,QCPAxis::SelectablePart,QMouseEvent*)), this, SLOT(axisLabelDoubleClick(QCPAxis*,QCPAxis::SelectablePart)));
    connect(ui->chart_show, SIGNAL(legendDoubleClick(QCPLegend*,QCPAbstractLegendItem*,QMouseEvent*)), this, SLOT(legendDoubleClick(QCPLegend*,QCPAbstractLegendItem*)));

    // connect slot that shows a message in the status bar when a graph is clicked:
    connect(ui->chart_show, SIGNAL(plottableClick(QCPAbstractPlottable*,QMouseEvent*)), this, SLOT(graphClicked(QCPAbstractPlottable*)));

    // setup policy and connect slot for context menu popup:
    ui->chart_show->setContextMenuPolicy(Qt::CustomContextMenu);
    connect(ui->chart_show, SIGNAL(customContextMenuRequested(QPoint)), this, SLOT(contextMenuRequest(QPoint)));

    //add
    addGraph(erot_info.goal_temperature_graph_info,QColor(0,0,180));
    addGraph(erot_info.now_temperature_graph_info,QColor(100,100,180));
    addGraph(erot_info.goal_humidity_graph_info,QColor(170, 255 ,0));
    addGraph(erot_info.now_humidity_graph_info,QColor(220, 255 ,150));
    //addGraph(erot_info.result_graph_info,QColor(0,255,0),1);
    addGraph(erot_info.shake_graph_info,QColor(255,0,78),0);

     addGraph(erot_info.tx_graph_info,QColor(0,255,78),1);
     addGraph(erot_info.rx_graph_info,QColor(255,0,78),1);

    alwaysgraphauto = false;
    switch_Axis = false;



}



void MainWindow::mousePressEvent(QMouseEvent *event)
{
    if(event->button() == Qt::LeftButton)

    {

        oldPos = event->globalPos();

        press = true;

    }
}

void MainWindow::mouseMoveEvent(QMouseEvent *event)
{
    if (press)

    {

        this->move(this->pos() + event->globalPos() - oldPos);

        oldPos = event->globalPos();

    }
}

void MainWindow::mouseReleaseEvent(QMouseEvent *event)
{
    press = false;
}

void MainWindow::change_testinginfo(QString s)
{
    ui->textEdit_testinginfo->append(QTime().currentTime().toString()+"  ->   " +s);
}

void MainWindow::change_title_testinginfo(QString s)
{
    ui->label_states->setText(s);
}



void MainWindow::on_Button_Exit_clicked()
{
    this->close();
}

void MainWindow::on_table_log_customContextMenuRequested(QPoint pos)
{

    QMenu *popMenu;
    popMenu = new QMenu(ui->table_log);
    popMenu->addAction("Update", this, SLOT(table_log_update_to_chart()));
    popMenu->popup(ui->table_log->mapToGlobal(pos));

    //popMenu->addAction(ui->action_Update);
    // popMenu->exec(QCursor::pos());
}

void MainWindow::table_log_update_to_chart()
{
    int tx_index=-1;
    int rx_index = -1;
   for(int i = 0 ;i <mainw->log_info.head_name.length();i++){
        if( log_info.head_name.at(i) =="TX"){
              tx_index = i;
              qDebug()<< "tx_index:"<<tx_index;
              erot_info.tx_graph_info.v_y.clear();
        }

        if( log_info.head_name.at(i) =="RX"){
              rx_index = i;
               qDebug()<< "rx_index:"<<rx_index;
               erot_info.rx_graph_info.v_y.clear();
        }
   }




  for(int j = 0 ; j < ui->table_log->rowCount() - 25 ;j++){

      if(tx_index != -1){
           //log_info.log_data[tx_index].append(ui->table_log->item(j,tx_index)->text());
            log_info.log_data[j][tx_index] = ui->table_log->item(j,tx_index)->text();
          qDebug()<< "TX:" <<ui->table_log->item(j,tx_index)->text();
        erot_info.tx_graph_info.v_y.append(ui->table_log->item(j,tx_index)->text().toDouble());
      }

      if(rx_index != -1){
             log_info.log_data[j][rx_index] = ui->table_log->item(j,rx_index)->text();
            qDebug()<<"RX:" <<ui->table_log->item(j,rx_index)->text();
        erot_info.rx_graph_info.v_y.append(ui->table_log->item(j,rx_index)->text().toDouble());
      }
  }

  updateGraph(erot_info.tx_graph_info);
  updateGraph(erot_info.rx_graph_info);


   QString filename = "log/"+QDateTime().currentDateTime().toString( "yyyy-MM-dd hhmmss");
   Global::Save_report(filename+".xls",ui->textEdit_testinginfo->toPlainText());
  QMessageBox::about(this,QString::fromLocal8Bit("提示"),QString::fromLocal8Bit("更新报告成功！"));
}



void MainWindow::timer_Update()
{
    if(testing_status == running )
    {
        //        ui->commandLinkButton_Start->setEnabled(false);
        ui->commandLinkButton_Start->setEnabled(false);
        ui->commandLinkButton_Setting->setEnabled(false);
        ui->commandLinkButton_Stop->setEnabled(true);
        test_time += 1;

        if(alwaysgraphauto)
        {
            graphAuto();
        }
    }
    else if (testing_status == stop)
    {
        ui->commandLinkButton_Start->setEnabled(true);
        ui->commandLinkButton_Setting->setEnabled(true);
        ui->commandLinkButton_Stop->setEnabled(false);
    }
    else if (testing_status == pause)
    {
        //        ui->Button_Setting->setEnabled(false);
        //        ui->Button_Start->setEnabled(true);
        //        ui->Button_Start->setText("contiune");
        //        ui->Button_Exit->setEnabled(false);
        //        ui->Button_Stop->setEnabled(true);
    }
    //更新显示时间
    int hour = test_time/60/60;
    int mi =  test_time/60%60;
    int sec = test_time%60%60;
    QString shour =QString::number(hour);
    QString smi = QString::number(mi);;
    QString ssec =QString::number(sec);;
    if(hour<10)  shour = "0"+shour;
    if(mi<10)  smi = "0"+smi;
    if(sec<10)  ssec = "0"+ssec;
    ui->lcdNumber_TestTime->display(QString("%1:%2:%3").arg(shour).arg(smi).arg(ssec));

    //更新仪表
    ui-> lineEdit_now_temp->display (QString::number(erot_info.now_temperature));
    ui-> lineEdit_gold_temp->display(QString::number(erot_info.goal_temperature));
    ui-> lineEdit_now_hum->display(QString::number(erot_info.now_humidity));
    ui-> lineEdit_gold_hum->display(QString::number(erot_info.goal_humidity));

    ui-> lineEdit_step->setText(QString::number(erot_info.chaminfo.i_Step));
    ui-> lineEdit_pattern->setText(QString::number(erot_info.chaminfo.i_Pattern));
    ui-> lineEdit_now_hour->setText(QString::number(erot_info.chaminfo.hour));
    ui-> lineEdit_now_min->setText(QString::number(erot_info.chaminfo.min));
    ui-> lineEdit_settimeh->setText(QString::number(erot_info.stepInfo.i_SetTimeH));
    ui-> lineEdit_settimem->setText(QString::number(erot_info.stepInfo.i_SetTimeM));
    ui-> lineEdit_sethum->setText(QString::number(erot_info.stepInfo.d_SetHum));
    ui-> lineEdit_settem->setText(QString::number(erot_info.stepInfo.d_SetTem));
    ui-> lineEdit_shark->setText(QString::number(erot_info.stepInfo.i_Timesignal[0]));
    ui-> lineEdit_power->setText(QString::number(erot_info.stepInfo.i_Timesignal[1]));
    ui-> lineEdit_defrost->setText(QString::number(erot_info.stepInfo.i_Timesignal[2]));

    ui->lineEdit_step_desc->setText(erot_info.description);
    if(erot_info.fail_num+erot_info.pass_num <= 0)
    {
        ui->lineEdit_fpy->setText("-------");
    }
    else
    {
        double pa= (erot_info.pass_num * 1.0) / (erot_info.fail_num+erot_info.pass_num) *100;
        ui->lineEdit_fpy->setText(QString::number(pa)+"%");    // setText(QString::number(pa));
        if(pa>90)
        {
            ui->lineEdit_fpy->setStyleSheet("color: rgb(0, 255, 0);");
        }
        else
        {
            ui->lineEdit_fpy->setStyleSheet("color: rgb(255, 0, 127);");
        }
    }

    ui->label_model_name->setText(etest.Model_Name);
    ui->label_sn->setText(SN);


    //更新图显示
    updateGraph(erot_info.goal_temperature_graph_info);
    updateGraph(erot_info.now_temperature_graph_info);
    updateGraph(erot_info.goal_humidity_graph_info);
    updateGraph(erot_info.now_humidity_graph_info);
    //updateGraph(erot_info.result_graph_info);
    updateGraph(erot_info.tx_graph_info);
    updateGraph(erot_info.rx_graph_info);
    updateGraph(erot_info.shake_graph_info);
    ui->chart_show->replot();

    //更新log显示
    //ui->table_log->clear();
    ui->table_log->setColumnCount(log_info.head_name.count());
    ui->table_log->setRowCount( log_info.rows+25);
    ui->table_log->setHorizontalHeaderLabels(log_info.head_name);
    for(int i =0 ; i < log_info.rows ; i++)
    {
        for(int j=0 ; j < log_info.cols ; j++)
        {
            ui->table_log->setItem(i,j, new QTableWidgetItem(log_info.log_data[i][j]));
        }
    }

    //Performance_Summary_Report
    ui->tableWidget_sumary_report->setColumnCount(Performance_Summary_Report.head_name.count());
    ui->tableWidget_sumary_report->setRowCount( Performance_Summary_Report.rows);
    ui->tableWidget_sumary_report->setHorizontalHeaderLabels(Performance_Summary_Report.head_name);
    for(int i =0 ; i < Performance_Summary_Report.rows ; i++)
    {
        for(int j=0 ; j < Performance_Summary_Report.cols ; j++)
        {
            ui->tableWidget_sumary_report->setItem(i,j, new QTableWidgetItem(Performance_Summary_Report.log_data[i][j]));
        }
    }

    //ui->tableWidget_profile_result
    ui->tableWidget_profile_result->setRowCount(Global::test_profile.L_Test_Point.count());
    for(int i =0 ; i <  ui->tableWidget_profile_result->rowCount() ; i++)
    {
        ui->tableWidget_profile_result->setItem(i,0, new QTableWidgetItem(QString::number(Global::test_profile.L_Test_Point[i].section)));
        ui->tableWidget_profile_result->setItem(i,1, new QTableWidgetItem(Global::test_profile.L_Test_Point[i].time));
        ui->tableWidget_profile_result->setItem(i,2, new QTableWidgetItem(QString::number(Global::test_profile.L_Test_Point[i].temperature)));
        ui->tableWidget_profile_result->setItem(i,3, new QTableWidgetItem(QString::number(Global::test_profile.L_Test_Point[i].humidity)));
        ui->tableWidget_profile_result->setItem(i,4, new QTableWidgetItem(QString::number(Global::test_profile.L_Test_Point[i].shake)));
        ui->tableWidget_profile_result->setItem(i,5, new QTableWidgetItem(Global::test_profile.L_Test_Point[i].power == true ? "1":"0"));
        ui->tableWidget_profile_result->setItem(i,6, new QTableWidgetItem(Global::test_profile.L_Test_Point[i].flow == true ? "1":"0"));
        ui->tableWidget_profile_result->setItem(i,7, new QTableWidgetItem(Global::test_profile.L_Test_Point[i].description));
        ui->tableWidget_profile_result->setItem(i,8, new QTableWidgetItem(QString::number(Global::test_profile.L_Test_Point[i].pass_num)));
        ui->tableWidget_profile_result->setItem(i,9, new QTableWidgetItem(QString::number(Global::test_profile.L_Test_Point[i].fail_num)));
        if(Global::test_profile.L_Test_Point[i].pass_num+Global::test_profile.L_Test_Point[i].fail_num  == 0)
        {
            ui->tableWidget_profile_result->setItem(i,10, new QTableWidgetItem("NA"));
        }
        else
        {
            ui->tableWidget_profile_result->setItem(i,10, new QTableWidgetItem(QString::number((Global::test_profile.L_Test_Point[i].pass_num*1.0)/(Global::test_profile.L_Test_Point[i].pass_num+Global::test_profile.L_Test_Point[i].fail_num)*100)+"%"));
        }
    }

}

void MainWindow::switch_timer_Update()
{
    switch_Axis=!switch_Axis;
}


void MainWindow::on_Button_Start_clicked()
{

    if(testing_status == stop)
    {
        bool ok;
        QString text = QInputDialog::getText(this,QString::fromLocal8Bit("请输入SN"),
                                             tr("Serial Number:"), QLineEdit::Normal,"", &ok);
        if (ok && !text.isEmpty())
        {
            SN=text;
        }
        else
        {
            QMessageBox::information(NULL,"NO", QString::fromLocal8Bit("输入SN有误请确认！"));
            return;
        }

        Ui_Test_Init();
        //ui->label_states->setText(QString().fromLocal8Bit("测试初始化完成"));
        // Testing_Info(QString().fromLocal8Bit("测试初始化完成"));
        etest.start();
        timer->start(1000);
        // testing_status =running;

    }
    else  if(testing_status == pause)
    {

        //        Testing_Info("测试继续运行");
        //        //etest.start();
        //        etest.pause(false);
        //        testing_status =running;
    }
    else  if(testing_status == running)
    {
        //        Testing_Info("测试暂停");
        //        etest.pause(true);
        //        testing_status =pause;
    }
}



void MainWindow::on_Button_Setting_clicked()
{
    if(d_setting == NULL)
    {
        d_setting = new Dialog_Setting(this);

    }
    d_setting->show();
}



void MainWindow::on_Button_Stop_clicked()
{
    ui->label_states->setText(QString().fromLocal8Bit("正在停止测试..."));
    testing_status = stop;
}



void MainWindow::Test_Finished()
{
    testing_status = stop;
    if(!etest.isFinished())
    {
        etest.stop();
        etest.terminate();
    }
    Testing_Info(QString().fromLocal8Bit("测试完成"));
    ui->label_states->setText(QString().fromLocal8Bit("测试完成"));

    Testing_Info(etest.Test_Result ==true ? "PASS" : "FAIL" );
    if(etest.Test_Result )
    {
        ui->label_states->setText(QString().fromLocal8Bit( "测试结果:PASS"));
        ui->label_states->setStyleSheet("color: rgb(0, 255, 0);");
    }
    else
    {
        ui->label_states->setText( QString().fromLocal8Bit("测试结果:FAIL"));
        ui->label_states->setStyleSheet("color: rgb(255, 0, 0);");
    }

    QString filename = "log/"+QDateTime().currentDateTime().toString( "yyyy-MM-dd hhmmss");

    QFile qf(filename+".txt");
    qf.open(QIODevice::WriteOnly | QIODevice::Text);
    qf.write( ui->textEdit_testinginfo->toPlainText().toLocal8Bit(),ui->textEdit_testinginfo->toPlainText().toLocal8Bit().length());
    qf.close();

   Global::Save_report(filename+".xls",ui->textEdit_testinginfo->toPlainText());
    timer->stop();
}



/*******************************************
 * *
 *
 * 以下是控制图形显示
 *
 *
 * ****************************************/

void MainWindow::Always_graphAuto()
{
    alwaysgraphauto=!alwaysgraphauto;
}



void MainWindow::titleDoubleClick(QMouseEvent *event, QCPPlotTitle *title)
{
    Q_UNUSED(event)
    // Set the plot title by double clicking on it
    bool ok;
    QString newTitle = QInputDialog::getText(this, "Eort Chart", "New plot title:", QLineEdit::Normal, title->text(), &ok);
    if (ok)
    {
        title->setText(newTitle);
        ui->chart_show->replot();
    }
}

void MainWindow::setupMultiAxisDemo(QCustomPlot *customPlot)
{
    customPlot->setInteractions(QCP::iRangeDrag | QCP::iRangeZoom);

    customPlot->setLocale(QLocale(QLocale::English, QLocale::UnitedKingdom)); // period as decimal separator and comma as thousand separator
    customPlot->legend->setVisible(true);
    QFont legendFont = font();  // start out with MainWindow's font..
    legendFont.setPointSize(9); // and make a bit smaller for legend
    customPlot->legend->setFont(legendFont);
    customPlot->legend->setBrush(QBrush(QColor(255,255,255,230)));
    // by default, the legend is in the inset layout of the main axis rect. So this is how we access it to change legend placement:
    customPlot->axisRect()->insetLayout()->setInsetAlignment(0, Qt::AlignBottom|Qt::AlignRight);

    // setup for graph 0: key axis left, value axis bottom
    // will contain left maxwell-like function
    customPlot->addGraph(customPlot->yAxis, customPlot->xAxis);
    customPlot->graph(0)->setPen(QPen(QColor(255, 100, 0)));
    customPlot->graph(0)->setBrush(QBrush(QPixmap("./balboa.jpg"))); // fill with texture of specified image
    customPlot->graph(0)->setLineStyle(QCPGraph::lsLine);
    customPlot->graph(0)->setScatterStyle(QCPScatterStyle(QCPScatterStyle::ssDisc, 5));
    customPlot->graph(0)->setName("Left maxwell function");

    // setup for graph 1: key axis bottom, value axis left (those are the default axes)
    // will contain bottom maxwell-like function
    customPlot->addGraph();
    customPlot->graph(1)->setPen(QPen(Qt::red));
    customPlot->graph(1)->setBrush(QBrush(QPixmap("./balboa.jpg"))); // same fill as we used for graph 0
    customPlot->graph(1)->setLineStyle(QCPGraph::lsStepCenter);
    customPlot->graph(1)->setScatterStyle(QCPScatterStyle(QCPScatterStyle::ssCircle, Qt::red, Qt::white, 7));
    customPlot->graph(1)->setErrorType(QCPGraph::etValue);
    customPlot->graph(1)->setName("Bottom maxwell function");

    // setup for graph 2: key axis top, value axis right
    // will contain high frequency sine with low frequency beating:
    customPlot->addGraph(customPlot->xAxis2, customPlot->yAxis2);
    customPlot->graph(2)->setPen(QPen(Qt::blue));
    customPlot->graph(2)->setName("High frequency sine");

    // setup for graph 3: same axes as graph 2
    // will contain low frequency beating envelope of graph 2
    customPlot->addGraph(customPlot->xAxis2, customPlot->yAxis2);
    QPen blueDotPen;
    blueDotPen.setColor(QColor(30, 40, 255, 150));
    blueDotPen.setStyle(Qt::DotLine);
    blueDotPen.setWidthF(4);
    customPlot->graph(3)->setPen(blueDotPen);
    customPlot->graph(3)->setName("Sine envelope");

    // setup for graph 4: key axis right, value axis top
    // will contain parabolically distributed data points with some random perturbance
    customPlot->addGraph(customPlot->yAxis2, customPlot->xAxis2);
    customPlot->graph(4)->setPen(QColor(50, 50, 50, 255));
    customPlot->graph(4)->setLineStyle(QCPGraph::lsNone);
    customPlot->graph(4)->setScatterStyle(QCPScatterStyle(QCPScatterStyle::ssCircle, 4));
    customPlot->graph(4)->setName("Some random data around\na quadratic function");

    // generate data, just playing with numbers, not much to learn here:
    QVector<double> x0(25), y0(25);
    QVector<double> x1(15), y1(15), y1err(15);
    QVector<double> x2(250), y2(250);
    QVector<double> x3(250), y3(250);
    QVector<double> x4(250), y4(250);
    for (int i=0; i<25; ++i) // data for graph 0
    {
        x0[i] = 3*i/25.0;
        y0[i] = exp(-x0[i]*x0[i]*0.8)*(x0[i]*x0[i]+x0[i]);
    }
    for (int i=0; i<15; ++i) // data for graph 1
    {
        x1[i] = 3*i/15.0;;
        y1[i] = exp(-x1[i]*x1[i])*(x1[i]*x1[i])*2.6;
        y1err[i] = y1[i]*0.25;
    }
    for (int i=0; i<250; ++i) // data for graphs 2, 3 and 4
    {
        x2[i] = i/250.0*3*M_PI;
        x3[i] = x2[i];
        x4[i] = i/250.0*100-50;
        y2[i] = sin(x2[i]*12)*cos(x2[i])*10;
        y3[i] = cos(x3[i])*10;
        y4[i] = 0.01*x4[i]*x4[i] + 1.5*(rand()/(double)RAND_MAX-0.5) + 1.5*M_PI;
    }

    // pass data points to graphs:
    customPlot->graph(0)->setData(x0, y0);
    customPlot->graph(1)->setDataValueError(x1, y1, y1err);
    customPlot->graph(2)->setData(x2, y2);
    customPlot->graph(3)->setData(x3, y3);
    customPlot->graph(4)->setData(x4, y4);
    // activate top and right axes, which are invisible by default:
    customPlot->xAxis2->setVisible(true);
    customPlot->yAxis2->setVisible(true);
    // set ranges appropriate to show data:
    customPlot->xAxis->setRange(0, 2.7);
    customPlot->yAxis->setRange(0, 2.6);
    customPlot->xAxis2->setRange(0, 3.0*M_PI);
    customPlot->yAxis2->setRange(-70, 35);
    // set pi ticks on top axis:
    QVector<double> piTicks;
    QVector<QString> piLabels;
    piTicks << 0  << 0.5*M_PI << M_PI << 1.5*M_PI << 2*M_PI << 2.5*M_PI << 3*M_PI;
    piLabels << "0" << QString::fromUtf8("?π") << QString::fromUtf8("π") << QString::fromUtf8("1?π") << QString::fromUtf8("2π") << QString::fromUtf8("2?π") << QString::fromUtf8("3π");
    customPlot->xAxis2->setAutoTicks(false);
    customPlot->xAxis2->setAutoTickLabels(false);
    customPlot->xAxis2->setTickVector(piTicks);
    customPlot->xAxis2->setTickVectorLabels(piLabels);
    // add title layout element:
    customPlot->plotLayout()->insertRow(0);
    customPlot->plotLayout()->addElement(0, 0, new QCPPlotTitle(customPlot, "Way too many graphs in one plot"));
    // set labels:
    customPlot->xAxis->setLabel("Bottom axis with outward ticks");
    customPlot->yAxis->setLabel("Left axis label");
    customPlot->xAxis2->setLabel("Top axis label");
    customPlot->yAxis2->setLabel("Right axis label");
    // make ticks on bottom axis go outward:
    customPlot->xAxis->setTickLength(0, 5);
    customPlot->xAxis->setSubTickLength(0, 3);
    // make ticks on right axis go inward and outward:
    customPlot->yAxis2->setTickLength(3, 3);
    customPlot->yAxis2->setSubTickLength(1, 1);
}

void MainWindow::axisLabelDoubleClick(QCPAxis *axis, QCPAxis::SelectablePart part)
{
    // Set an axis label by double clicking on it
    if (part == QCPAxis::spAxisLabel) // only react when the actual axis label is clicked, not tick label or axis backbone
    {
        bool ok;
        QString newLabel = QInputDialog::getText(this, "Eort Chart", "New axis label:", QLineEdit::Normal, axis->label(), &ok);
        if (ok)
        {
            axis->setLabel(newLabel);
            ui->chart_show->replot();
        }
    }
}

void MainWindow::legendDoubleClick(QCPLegend *legend, QCPAbstractLegendItem *item)
{
    // Rename a graph by double clicking on its legend item
    Q_UNUSED(legend)
    if (item) // only react if item was clicked (user could have clicked on border padding of legend where there is no item, then item is 0)
    {
        QCPPlottableLegendItem *plItem = qobject_cast<QCPPlottableLegendItem*>(item);
        bool ok;
        QString newName = QInputDialog::getText(this, "Eort Chart", "New graph name:", QLineEdit::Normal, plItem->plottable()->name(), &ok);
        if (ok)
        {
            plItem->plottable()->setName(newName);
            ui->chart_show->replot();
        }
    }
}

void MainWindow::selectionChanged()
{
    /*
       normally, axis base line, axis tick labels and axis labels are selectable separately, but we want
       the user only to be able to select the axis as a whole, so we tie the selected states of the tick labels
       and the axis base line together. However, the axis label shall be selectable individually.

       The selection state of the left and right axes shall be synchronized as well as the state of the
       bottom and top axes.

       Further, we want to synchronize the selection of the graphs with the selection state of the respective
       legend item belonging to that graph. So the user can select a graph by either clicking on the graph itself
       or on its legend item.
      */

    // make top and bottom axes be selected synchronously, and handle axis and tick labels as one selectable object:
    if (ui->chart_show->xAxis->selectedParts().testFlag(QCPAxis::spAxis) || ui->chart_show->xAxis->selectedParts().testFlag(QCPAxis::spTickLabels) ||
            ui->chart_show->xAxis2->selectedParts().testFlag(QCPAxis::spAxis) || ui->chart_show->xAxis2->selectedParts().testFlag(QCPAxis::spTickLabels))
    {
        ui->chart_show->xAxis2->setSelectedParts(QCPAxis::spAxis|QCPAxis::spTickLabels);
        ui->chart_show->xAxis->setSelectedParts(QCPAxis::spAxis|QCPAxis::spTickLabels);
    }
    // make left and right axes be selected synchronously, and handle axis and tick labels as one selectable object:
    if (ui->chart_show->yAxis->selectedParts().testFlag(QCPAxis::spAxis) || ui->chart_show->yAxis->selectedParts().testFlag(QCPAxis::spTickLabels) ||
            ui->chart_show->yAxis2->selectedParts().testFlag(QCPAxis::spAxis) || ui->chart_show->yAxis2->selectedParts().testFlag(QCPAxis::spTickLabels))
    {
        ui->chart_show->yAxis2->setSelectedParts(QCPAxis::spAxis|QCPAxis::spTickLabels);
        ui->chart_show->yAxis->setSelectedParts(QCPAxis::spAxis|QCPAxis::spTickLabels);
    }

    // synchronize selection of graphs with selection of corresponding legend items:
    for (int i=0; i<ui->chart_show->graphCount(); ++i)
    {
        QCPGraph *graph = ui->chart_show->graph(i);
        QCPPlottableLegendItem *item = ui->chart_show->legend->itemWithPlottable(graph);
        if (item->selected() || graph->selected())
        {
            item->setSelected(true);
            graph->setSelected(true);
        }
    }
}

void MainWindow::mousePress()
{
    // if an axis is selected, only allow the direction of that axis to be dragged
    // if no axis is selected, both directions may be dragged

    if (ui->chart_show->xAxis->selectedParts().testFlag(QCPAxis::spAxis))
        ui->chart_show->axisRect()->setRangeDrag(ui->chart_show->xAxis->orientation());
    else if (ui->chart_show->yAxis->selectedParts().testFlag(QCPAxis::spAxis))
        ui->chart_show->axisRect()->setRangeDrag(ui->chart_show->yAxis->orientation());
    else
        ui->chart_show->axisRect()->setRangeDrag(Qt::Horizontal|Qt::Vertical);
}

void MainWindow::mouseWheel()
{
    // if an axis is selected, only allow the direction of that axis to be zoomed
    // if no axis is selected, both directions may be zoomed

    if (ui->chart_show->xAxis->selectedParts().testFlag(QCPAxis::spAxis))
        ui->chart_show->axisRect()->setRangeZoom(ui->chart_show->xAxis->orientation());
    else if (ui->chart_show->yAxis->selectedParts().testFlag(QCPAxis::spAxis))
        ui->chart_show->axisRect()->setRangeZoom(ui->chart_show->yAxis->orientation());
    else
        ui->chart_show->axisRect()->setRangeZoom(Qt::Horizontal|Qt::Vertical);
}

void MainWindow::addRandomGraph()
{
    int n = 50; // number of points in graph
    double xScale = (rand()/(double)RAND_MAX + 0.5)*2;
    double yScale = (rand()/(double)RAND_MAX + 0.5)*2;
    double xOffset = (rand()/(double)RAND_MAX - 0.5)*4;
    double yOffset = (rand()/(double)RAND_MAX - 0.5)*5;
    double r1 = (rand()/(double)RAND_MAX - 0.5)*2;
    double r2 = (rand()/(double)RAND_MAX - 0.5)*2;
    double r3 = (rand()/(double)RAND_MAX - 0.5)*2;
    double r4 = (rand()/(double)RAND_MAX - 0.5)*2;
    QVector<double> x(n), y(n);
    for (int i=0; i<n; i++)
    {
        x[i] = (i/(double)n-0.5)*10.0*xScale + xOffset;
        y[i] = (sin(x[i]*r1*5)*sin(cos(x[i]*r2)*r4*3)+r3*cos(sin(x[i])*r4*2))*yScale + yOffset;
    }

    ui->chart_show->addGraph();
    ui->chart_show->graph()->setName(QString("New graph %1").arg(ui->chart_show->graphCount()-1));
    ui->chart_show->graph()->setData(x, y);
    ui->chart_show->graph()->setLineStyle((QCPGraph::LineStyle)(rand()%5+1));
    if (rand()%100 > 75)
        ui->chart_show->graph()->setScatterStyle(QCPScatterStyle((QCPScatterStyle::ScatterShape)(rand()%9+1)));
    QPen graphPen;
    graphPen.setColor(QColor(rand()%245+10, rand()%245+10, rand()%245+10));
    graphPen.setWidthF(rand()/(double)RAND_MAX*2+1);
    ui->chart_show->graph()->setPen(graphPen);
    ui->chart_show->replot();
}

void MainWindow::addGraph(Graph_Info gi, QColor qc, int mode,int line)
{

    if(line == 0) ui->chart_show->addGraph(ui->chart_show->xAxis,ui->chart_show->yAxis);
    else
    {
        ui->chart_show->addGraph(ui->chart_show->xAxis2,ui->chart_show->yAxis2);
    }
    ui->chart_show->graph()->setName(gi.name);
    ui->chart_show->graph()->setData(gi.v_x, gi.v_y);
    if(mode == 1)
    {
        //LineStyle::
        ui->chart_show->graph()->setLineStyle(QCPGraph::lsStepCenter);
        //::ScatterShape
        ui->chart_show->graph()->setScatterStyle(QCPScatterStyle(QCPScatterStyle::ssCrossSquare));
    }
    else
    {
        //::LineStyle
        ui->chart_show->graph()->setLineStyle(QCPGraph::lsLine);
        //::ScatterShape
        ui->chart_show->graph()->setScatterStyle(QCPScatterStyle(QCPScatterStyle::ssNone));
    }
    QPen graphPen;
    graphPen.setColor(qc);
    graphPen.setWidthF(1);
    ui->chart_show->graph()->setPen(graphPen);
    ui->chart_show->replot();
}


void  MainWindow::updateGraph(QString Name, double x, double y)
{
    //    for (int i=0; i<ui->chart_show->graphCount(); ++i)
    //    {
    //        QCPGraph *graph = ui->chart_show->graph(i);
    //        if (graph->name() == gi.name)
    //        {
    //            graph->clearData();
    //            graph->setData(gi.v_x,gi.v_y);
    //            //graph->addData(gi.v_x,gi.v_y);
    //            break;
    //        }
    //    }

    for (int i=0; i<ui->chart_show->graphCount(); ++i)
    {
        QCPGraph *graph = ui->chart_show->graph(i);
        if (graph->name() == Name)
        {
            //graph->clearData();
            // graph->setData(gi.v_x,gi.v_y);
            graph->addData(x,y);
            break;
        }
    }


}

void MainWindow::updateGraph(Graph_Info gi)
{
    for (int i=0; i<ui->chart_show->graphCount(); ++i)
    {
        QCPGraph *graph = ui->chart_show->graph(i);
        if (graph->name() == gi.name)
        {
            graph->clearData();
            graph->setData(gi.v_x,gi.v_y);
            //graph->addData(gi.v_x,gi.v_y);
            break;
        }
    }
}

void MainWindow::removeSelectedGraph()
{
    if (ui->chart_show->selectedGraphs().size() > 0)
    {
        ui->chart_show->removeGraph(ui->chart_show->selectedGraphs().first());
        ui->chart_show->replot();
    }
}

void MainWindow::removeAllGraphs()
{
    ui->chart_show->clearGraphs();
    ui->chart_show->replot();
}

void MainWindow::contextMenuRequest(QPoint pos)
{
    QMenu *menu = new QMenu(this);
    menu->setAttribute(Qt::WA_DeleteOnClose);

    if (ui->chart_show->legend->selectTest(pos, false) >= 0) // context menu on legend requested
    {
        menu->addAction("Move to top left", this, SLOT(moveLegend()))->setData((int)(Qt::AlignTop|Qt::AlignLeft));
        menu->addAction("Move to top center", this, SLOT(moveLegend()))->setData((int)(Qt::AlignTop|Qt::AlignHCenter));
        menu->addAction("Move to top right", this, SLOT(moveLegend()))->setData((int)(Qt::AlignTop|Qt::AlignRight));
        menu->addAction("Move to bottom right", this, SLOT(moveLegend()))->setData((int)(Qt::AlignBottom|Qt::AlignRight));
        menu->addAction("Move to bottom left", this, SLOT(moveLegend()))->setData((int)(Qt::AlignBottom|Qt::AlignLeft));
    }
    else  // general context menu on graphs requested
    {
        //        menu->addAction("Add random graph", this, SLOT(addRandomGraph()));
        //        if (ui->chart_show->selectedGraphs().size() > 0)
        //            menu->addAction("Remove selected graph", this, SLOT(removeSelectedGraph()));
        //        if (ui->chart_show->graphCount() > 0)
        //            menu->addAction("Remove all graphs", this, SLOT(removeAllGraphs()));

        menu->addAction("Save graphs", this, SLOT(graphSave()));
        menu->addAction("Graphs Auto", this, SLOT(graphAuto()));
        menu->addAction("Always Graphs Auto", this, SLOT(Always_graphAuto()));
        //menu->addAction("Switch Axis", this, SLOT(switch_timer_Update()));
    }

    menu->popup(ui->chart_show->mapToGlobal(pos));
}

void MainWindow::moveLegend()
{
    if (QAction* contextAction = qobject_cast<QAction*>(sender())) // make sure this slot is really called by a context menu action, so it carries the data we need
    {
        bool ok;
        int dataInt = contextAction->data().toInt(&ok);
        if (ok)
        {
            ui->chart_show->axisRect()->insetLayout()->setInsetAlignment(0, (Qt::Alignment)dataInt);
            ui->chart_show->replot();
        }
    }
}

void MainWindow::graphClicked(QCPAbstractPlottable *plottable)
{

}

void MainWindow::graphSave()
{
    QString fileName = QFileDialog::getSaveFileName(this,
                                                    tr("Save graphs"),
                                                    "",
                                                    tr("Save graphs (*.png)"));

    if (!fileName.isNull())
    {
        ui->chart_show->savePng(fileName,1280,1024);

    }
    else
    {
        //点的是取消
    }
}

void MainWindow::Auto_SaveImage()
{
    graphAuto();
    ui->chart_show->savePng("data.png",1280,1024);
}


void MainWindow::graphAuto()
{
    ui->chart_show-> rescaleAxes();
    ui->chart_show-> replot();
}

