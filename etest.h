/*!
* \file etest.h
* \brief 测试线程类
*
*Eort测试线程
*
* \author ningjian
* \version v1.00
* \date 二〇一四年七月十六日下午三时十四分四十八秒 星期三
*/



#ifndef ETEST_H
#define ETEST_H
#include <QThread>
#include "global.h"
#include <QProcess>
#include <QDateTime>

/// \brief 测试线程类
///
/// Eort测试线程
class ETest : public QThread
{
    Q_OBJECT
public:
    explicit ETest(QObject *parent = 0);
    bool init();
    void pause(bool ispause);
    void stop();
    bool  Test_Result;

    void add_log();

    void simplification_graph(Graph_Info &graph,int times = 20);


    QString Model_Name;

private:
    QProcess *flow_process;
    int  comid;  ///> 温箱com id
    int  shark_comid;  ///> 震动com id
    ChamberInfo  chaminfo;
    StepInfo stepinfo;

    int End_Test_Section;

    QDateTime last_setp_run_time;
protected:
   void run();

signals:
    void done();
    void add_updateGraph(QString Name, double x, double y);
public slots:

};

#endif // ETEST_H
