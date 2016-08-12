/*!
* \file mainwindow.h
* \brief 主窗体
*
*主窗体
*
* \author ningjian
* \version v1.00
* \date 二〇一四年七月十六日下午三时十四分四十八秒 星期三
*/

#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include <QPoint>
#include <QMouseEvent>
#include "form_clock.h"
#include <QLabel>
#include "3rd/qcustomplot.h"
// 圆 2R =  184    W1 = 138  W2 =  396 W3 = 670
//
#include <QTimer>
#include "etest.h"
#include "global.h"
#include "dialog_setting.h"
#include <QLibrary>

#include <QToolButton>
#include <QMutex>

namespace Ui {
class MainWindow;
}




class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    explicit MainWindow(QWidget *parent = 0);
    ~MainWindow();

    /// \brief 测试前ui的初始化清空等动作
    /// \param void
    ///    \return void
     void Ui_Test_Init();

     /// \brief 通过信号方式添加测试过程log
     /// \param log信息
     ///    \return void
     void Testing_Info(QString s);

     void Title_Testing_Info(QString s);

     void Auto_SaveImage();


     Eort_Info    erot_info;  ///> 获取设备信息
     Log_Info    log_info;    ///>  log信息
     Log_Info    Performance_Summary_Report;

      long test_time;  ///> 测试时间
      QString SN;

      Testing_Status  testing_status;    ///> 测试状态
      ETest etest;   ///> 测试线程


signals:
    void add_testinginfo(QString s);
    void add_title_testinginfo(QString s);


private:
    Ui::MainWindow *ui;
    QTimer *timer;
    void init_chart();


    bool alwaysgraphauto;   ///> 是否Always_graphAuto()
    bool  switch_Axis ;






  Dialog_Setting    *d_setting;
  QLibrary dll_lib;



  void init_dll();
  void load_test_profile();

protected:
    QPoint oldPos;

    bool press;

    void mousePressEvent(QMouseEvent *event);

    void mouseMoveEvent(QMouseEvent *event);

    void mouseReleaseEvent(QMouseEvent *event);



private slots:

    void change_testinginfo(QString s);
    void change_title_testinginfo(QString s);

    void on_Button_Exit_clicked();

    void on_table_log_customContextMenuRequested(QPoint pos);

    void table_log_update_to_chart();

     void titleDoubleClick(QMouseEvent *event, QCPPlotTitle *title);
     void axisLabelDoubleClick(QCPAxis* axis, QCPAxis::SelectablePart part);
     void legendDoubleClick(QCPLegend* legend, QCPAbstractLegendItem* item);
     void selectionChanged();
     void mousePress();
     void mouseWheel();
     void addRandomGraph();
     void addGraph(Graph_Info gi, QColor qc =QColor(255,255,255), int mode=0, int line=0);
     void updateGraph(QString Name,double x,double y);
     void updateGraph(Graph_Info gi);
     void removeSelectedGraph();
     void removeAllGraphs();
     void contextMenuRequest(QPoint pos);
     void moveLegend();
     void graphClicked(QCPAbstractPlottable *plottable);
     void graphSave();
     void graphAuto();
     void timer_Update();
     void switch_timer_Update();
     void  Always_graphAuto();
     void on_Button_Start_clicked();
     void on_Button_Stop_clicked();
     void on_Button_Setting_clicked();
     void setupMultiAxisDemo(QCustomPlot *customPlot);


     void Test_Finished();
};


#endif // MAINWINDOW_H
