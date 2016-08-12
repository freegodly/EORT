/*!
* \file etest.h
* \brief �����߳���
*
*Eort�����߳�
*
* \author ningjian
* \version v1.00
* \date ����һ��������ʮ����������ʱʮ�ķ���ʮ���� ������
*/



#ifndef ETEST_H
#define ETEST_H
#include <QThread>
#include "global.h"
#include <QProcess>
#include <QDateTime>

/// \brief �����߳���
///
/// Eort�����߳�
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
    int  comid;  ///> ����com id
    int  shark_comid;  ///> ��com id
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
