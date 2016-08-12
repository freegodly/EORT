#include "etest.h"
#include "mainwindow.h"
#include <QTime>
#include "flow_result.h"
#include "global.h"
#include "QRegExp"
ETest::ETest(QObject *parent) :
    QThread(parent)
{
}

/*
 * 初始化
 * */
bool ETest::init()
{
    Test_Result = false;
    mainw->Testing_Info(QString().fromLocal8Bit("测试初始化中。。。"));
    mainw->Title_Testing_Info(QString().fromLocal8Bit("测试初始化中"));
    comid  = Global::GetValueXml("Instrument_Com","Value").toInt();
    shark_comid = Global::GetValueXml("Instrument_Shark_Com","Value").toInt();
    Model_Name = Global::GetValueXml("Model_Name","Value");
    Flow_Result::Result_filepath = Global::GetValueXml("Flow_File","Value");

    End_Test_Section = Global::Query_Test_Profile_Max_Section();
    mainw->erot_info.section = -1;

    mainw-> erot_info.result_graph_info.v_y.append(110);
    mainw-> erot_info.result_graph_info.v_x.append(0);
    mainw-> erot_info.fail_num=0;
    mainw->erot_info.pass_num=0;
    mainw-> erot_info.shake = 0;
    //log info

    mainw->log_info.rows = 0;
    mainw->log_info.head_name.clear();
    mainw->log_info.head_name<<"Date"<<"SN"<<"Model"<<"Result"<<"Time"<<"Measure Temperature"<<"Set Temperature"<<"Measure Humidity"<<"Set Humidity";                        ///>    init  初始化log头
    mainw-> log_info.cols= mainw->log_info.head_name.count();


    mainw->Performance_Summary_Report.rows = 0;
    mainw->Performance_Summary_Report.head_name.clear();
    mainw->Performance_Summary_Report.head_name <<"Test Date"<<"Model Name"<<"Serial Number"<<"Peak Operation Temp"<<"Lowest Operation Temp"<<"EORT Result";
    mainw-> Performance_Summary_Report.cols= mainw->Performance_Summary_Report.head_name.count();
    ///
    ///获取接口文件的Item项
    ///
    ///
    Flow_Result::GetItmeName();
    for(int i =0;i <Flow_Result::ItemName.count();i++)
    {
        mainw-> log_info.head_name.insert(4+i,Flow_Result::ItemName[i]);
    }
    mainw->  log_info.cols=mainw-> log_info.head_name.count();
    Flow_Result::L_portinfo.clear();

    ///
    ///获取每个段的描述信息 加入tableWidget_sumary_report
    ///
    mainw->  Performance_Summary_Report.log_data.clear();
    QVector<QString> value;
    value.append(QDateTime().currentDateTime().toString( "yyyy-MM-dd hh:mm:ss"));
    value.append(Model_Name);
    value.append(mainw->SN);
    value.append(QString::number( Global::test_profile.max_temperature));
    value.append(QString::number( Global::test_profile.min_temperature));
    for(int i =0 ;i <Global::test_profile.L_Test_Point.count();i++)
    {
        if(Global::test_profile.L_Test_Point[i].description !="NA")
        {
            if(!mainw->Performance_Summary_Report.head_name.contains(Global::test_profile.L_Test_Point[i].description))
            {
                mainw->Performance_Summary_Report.head_name.insert(5,Global::test_profile.L_Test_Point[i].description);
                value.append("---");
            }
        }
    }
    mainw->  Performance_Summary_Report.cols=mainw-> Performance_Summary_Report.head_name.count();
    value.append("---");
    mainw->  Performance_Summary_Report.log_data.insert(0,value);
    mainw->  Performance_Summary_Report.rows=1;

    ///
    ///启动温箱
    ///
    if( Global::GetValueXml("Instrument_Enable","Value") == "True"){
        char data[256];
        if(!oper_Run(comid,data) )
        {
            mainw->Testing_Info(QString().fromLocal8Bit("启动温箱仪器失败"));
            mainw->Title_Testing_Info(QString().fromLocal8Bit("启动温箱仪器失败"));
            return  false;
        }
        else
        {
            mainw->Testing_Info(QString().fromLocal8Bit("启动温箱仪器成功"));
            mainw->Title_Testing_Info(QString().fromLocal8Bit("启动温箱仪器成功"));
        }


        if(!startTest(shark_comid,data))
        {
            mainw->Testing_Info(QString().fromLocal8Bit("启动震动仪器失败"));
            mainw->Title_Testing_Info(QString().fromLocal8Bit("启动震动仪器失败"));
            return  false;
        }
        else
        {
            mainw->Testing_Info(QString().fromLocal8Bit("启动震动仪器成功"));
            mainw->Title_Testing_Info(QString().fromLocal8Bit("启动震动仪器成功"));
        }
    }


    ///
    ///启动打流程序
    ///

    QStringList arguments;
    flow_process = new QProcess();

    flow_process->start(Global::GetValueXml("Flow_Path","Value"), arguments);


    bool flag =    flow_process->isOpen();

    if(flag)
    {
        mainw->Testing_Info(QString().fromLocal8Bit("启动测试程序成功"));
        mainw->Title_Testing_Info(QString().fromLocal8Bit("启动测试程序成功"));
    }

    return flag;
}

void ETest::pause(bool ispause)
{
    if(ispause)
    {
        sleep(INT_MAX);
    }
    else
    {
        sleep(0);
    }
}

void ETest::stop()
{
    char data[256];
    flow_process->close();
    oper_Stop(comid,data);
}

void ETest::run()
{
    if(init())
    {
        mainw-> testing_status =running;
        mainw->Testing_Info(QString().fromLocal8Bit("测试初始化完成"));
    }
    else
    {
        return ;
    }

    mainw->Testing_Info(QString().fromLocal8Bit("测试开始..."));

    ///
    /// 初始化
    ///
    last_setp_run_time   =    QDateTime().currentDateTime();

    while(true)
    {
        //Testing_Status::
        if(mainw->testing_status == Testing_Status::stop) break;

        ///
        /// \brief 取温箱仪器信息
        ///
        if( Global::GetValueXml("Instrument_Enable","Value") == "True"){
            mainw->Title_Testing_Info(QString().fromLocal8Bit("获取仪器信息..."));
            if(! getInformation(comid,chaminfo))
            {
                mainw->Testing_Info(QString().fromLocal8Bit("获取温箱仪器信息失败"));
                mainw->Testing_Info(chaminfo.error);
            }
            else
            {
                getStepSetInfo(comid,chaminfo.i_Pattern,chaminfo.i_Step,stepinfo);


                simplification_graph(mainw-> erot_info.goal_temperature_graph_info,2);
                mainw-> erot_info.goal_temperature_graph_info.v_x.append(mainw->test_time);
                mainw-> erot_info.goal_temperature_graph_info.v_y.append(stepinfo.d_SetTem);

                simplification_graph(mainw-> erot_info.now_temperature_graph_info);
                mainw-> erot_info.now_temperature_graph_info.v_x.append(mainw->test_time);
                mainw-> erot_info.now_temperature_graph_info.v_y.append(chaminfo.now_temperature);

                simplification_graph(mainw-> erot_info.now_humidity_graph_info);
                mainw-> erot_info.now_humidity_graph_info.v_x.append(mainw->test_time);
                mainw-> erot_info.now_humidity_graph_info.v_y.append(chaminfo.now_humidity);

                simplification_graph(mainw-> erot_info.goal_humidity_graph_info,2);
                mainw-> erot_info.goal_humidity_graph_info.v_x.append(mainw->test_time);
                mainw-> erot_info.goal_humidity_graph_info.v_y.append(stepinfo.d_SetHum);

                mainw-> erot_info.goal_temperature=stepinfo.d_SetTem;

                mainw-> erot_info.now_temperature=chaminfo.now_temperature;

                mainw-> erot_info.now_humidity=chaminfo.now_humidity;

                mainw-> erot_info.goal_humidity=stepinfo.d_SetHum;

                mainw-> erot_info.chaminfo = chaminfo;
                mainw-> erot_info.stepInfo  =  stepinfo;


                ///
                /// \brief 查询是否切换段
                ///
                ///
                char data[256];
                if(mainw-> erot_info.section !=chaminfo.i_Step)
                {

                    last_setp_run_time   =    QDateTime().currentDateTime();
                    mainw-> erot_info.section =chaminfo.i_Step;
                    mainw->Testing_Info(QString().fromLocal8Bit("##################################################"));
                    mainw->Testing_Info(QString("-----------------Step :%1").arg(QString::number(chaminfo.i_Step)));
                    mainw->Testing_Info(QString("-----------------Pattern :%1").arg(QString::number(chaminfo.i_Pattern)));
                    mainw->Testing_Info(QString("-----------------now_hour :%1").arg(QString::number(chaminfo.hour)));
                    mainw->Testing_Info(QString("-----------------now_min :%1").arg(QString::number(chaminfo.min)));
                    mainw->Testing_Info("=============================");
                    mainw->Testing_Info(QString("-----------------goal_temperature :%1").arg(QString::number(chaminfo.goal_temperature)));
                    mainw->Testing_Info(QString("-----------------now_temperature :%1").arg(QString::number(chaminfo.now_temperature)));
                    mainw->Testing_Info(QString("-----------------now_humidity :%1").arg(QString::number(chaminfo.now_humidity)));
                    mainw->Testing_Info(QString("-----------------goal_humidity :%1").arg(QString::number(chaminfo.goal_humidity)));
                    mainw->Testing_Info("=============================");
                    mainw->Testing_Info(QString("-----------------SetTimeH :%1").arg(QString::number(stepinfo.i_SetTimeH)));
                    mainw->Testing_Info(QString("-----------------SetTimeM :%1").arg(QString::number(stepinfo.i_SetTimeM)));
                    mainw->Testing_Info(QString("-----------------SetHum :%1").arg(QString::number(stepinfo.d_SetHum)));
                    mainw->Testing_Info(QString("-----------------SetTem :%1").arg(QString::number(stepinfo.d_SetTem)));
                    mainw->Testing_Info(QString("-----------------Shark :%1").arg(QString::number(stepinfo.i_Timesignal[0])));
                    mainw->Testing_Info(QString("-----------------Power :%1").arg(QString::number(stepinfo.i_Timesignal[1])));
                    mainw->Testing_Info(QString("-----------------Defrost :%1").arg(QString::number(stepinfo.i_Timesignal[2])));
                    mainw->Testing_Info(QString().fromLocal8Bit("##################################################"));
                    //判断是否震动
                    int index= Global::Query_Test_Profile_Point(mainw-> erot_info.section);
                    if(index != -1)
                    {
                        mainw-> erot_info.description = Global::test_profile.L_Test_Point[index].description;
                        mainw-> erot_info.shake=Global::test_profile.L_Test_Point[index].shake;


                        if(Global::test_profile.L_Test_Point[index].shake !=0)
                        {
                            stopShake(shark_comid,data);
                            msleep(200);
                            mainw->Testing_Info(QString().fromLocal8Bit("开启震动..."));
                            if(!startShake(shark_comid,Global::test_profile.L_Test_Point[index].shake, data))
                            {
                                mainw->Testing_Info(data);
                            }
                            else
                            {
                                mainw->Testing_Info(QString().fromLocal8Bit("开启震动OK"));
                            }
                        }
                        else
                        {
                            stopShake(shark_comid,data);
                        }
                    }
                }
            }
            simplification_graph(mainw-> erot_info.shake_graph_info,2);
            mainw-> erot_info.shake_graph_info.v_x.append(mainw->test_time);
            mainw-> erot_info.shake_graph_info.v_y.append(mainw-> erot_info.shake);
        }else{

            int index = 0;

            mainw-> erot_info.section = index;

            mainw-> erot_info.description = Global::test_profile.L_Test_Point[index].description;
            mainw-> erot_info.shake=Global::test_profile.L_Test_Point[index].shake;

            simplification_graph(mainw-> erot_info.goal_temperature_graph_info,2);
            mainw-> erot_info.goal_temperature_graph_info.v_x.append(mainw->test_time);
            mainw-> erot_info.goal_temperature_graph_info.v_y.append(Global::test_profile.L_Test_Point[index].temperature);

            simplification_graph(mainw-> erot_info.now_temperature_graph_info);
            mainw-> erot_info.now_temperature_graph_info.v_x.append(mainw->test_time);
            mainw-> erot_info.now_temperature_graph_info.v_y.append(Global::test_profile.L_Test_Point[index].temperature);

            simplification_graph(mainw-> erot_info.now_humidity_graph_info);
            mainw-> erot_info.now_humidity_graph_info.v_x.append(mainw->test_time);
            mainw-> erot_info.now_humidity_graph_info.v_y.append(Global::test_profile.L_Test_Point[index].humidity);

            simplification_graph(mainw-> erot_info.goal_humidity_graph_info,2);
            mainw-> erot_info.goal_humidity_graph_info.v_x.append(mainw->test_time);
            mainw-> erot_info.goal_humidity_graph_info.v_y.append(Global::test_profile.L_Test_Point[index].humidity);

            mainw-> erot_info.goal_temperature=Global::test_profile.L_Test_Point[index].temperature;

            mainw-> erot_info.now_temperature=Global::test_profile.L_Test_Point[index].temperature;

            mainw-> erot_info.now_humidity=Global::test_profile.L_Test_Point[index].humidity;

            mainw-> erot_info.goal_humidity=Global::test_profile.L_Test_Point[index].humidity;

            stepinfo.i_SetTimeH=Global::test_profile.L_Test_Point[index].time.split("'")[0].toInt();
            stepinfo.i_SetTimeM=Global::test_profile.L_Test_Point[index].time.split("'")[1].toInt();


            chaminfo.now_temperature=mainw-> erot_info.goal_temperature;
            stepinfo.d_SetTem=mainw-> erot_info.goal_temperature;
            chaminfo.now_humidity= mainw-> erot_info.now_humidity;
            stepinfo.d_SetHum =mainw-> erot_info.now_humidity;
        }




        ///
        /// \brief 查询打流信息
        ///

        int index= Global::Query_Test_Profile_Point(mainw-> erot_info.section);
        if(index != -1 && Global::test_profile.L_Test_Point[index].flow )
        {
            mainw->Title_Testing_Info(QString().fromLocal8Bit("查询测试信息..."));
            if( Flow_Result::Query_Result() )
            {
                int i = 0;
                int tx_index =-1;
                int rx_index = -1;
                foreach( QString value, Flow_Result::ItemName){
                    if(value == "TX") tx_index = i;
                     if(value == "RX") rx_index = i;
                    i++;
                }

               PortInfo PI =   Flow_Result::L_portinfo.last();
               if(tx_index != -1){
                   mainw-> erot_info.tx_graph_info.v_y.append(PI.info[tx_index].toDouble());
                   mainw-> erot_info.tx_graph_info.v_x.append(mainw->test_time);
               }

               if(rx_index != -1){
                   mainw-> erot_info.rx_graph_info.v_y.append(PI.info[rx_index].toDouble());
                   mainw-> erot_info.rx_graph_info.v_x.append(mainw->test_time);
               }


                mainw-> erot_info.result_graph_info.v_y.append(Flow_Result::Result =="PASS"  ? 100:0);
                mainw-> erot_info.result_graph_info.v_x.append(mainw->test_time);
                if(Flow_Result::Result =="PASS" )
                {
                    mainw-> erot_info.pass_num++;
                    Global::test_profile.L_Test_Point[index].pass_num++;
                }
                else
                {
                    mainw-> erot_info.fail_num++;
                    Global::test_profile.L_Test_Point[index].fail_num++;
                }


                add_log();
            }
        }



        ///
        /// \brief 实时更新总结过
        ///
        QMap<QString,PF> pf;
        for(int i = 0; i<Global::test_profile.L_Test_Point.count();i++)
        {
            if( Global::test_profile.L_Test_Point[i].description != "NA" )
            {
                if(pf.contains(Global::test_profile.L_Test_Point[i].description))
                {
                    pf[Global::test_profile.L_Test_Point[i].description].pass_num+=Global::test_profile.L_Test_Point[i].pass_num;
                    pf[Global::test_profile.L_Test_Point[i].description].fail_num+=Global::test_profile.L_Test_Point[i].fail_num;
                }
                else
                {
                    PF pp;
                    pp.fail_num=0;
                    pp.pass_num = 0;
                    pf.insert(Global::test_profile.L_Test_Point[i].description,pp);
                    pf[Global::test_profile.L_Test_Point[i].description].pass_num+=Global::test_profile.L_Test_Point[i].pass_num;
                    pf[Global::test_profile.L_Test_Point[i].description].fail_num+=Global::test_profile.L_Test_Point[i].fail_num;
                }
            }
        }
        QMapIterator<QString,PF> iter(pf);
        while (iter.hasNext())
        {
            int index =Global::Find_QStringList_Index( mainw->Performance_Summary_Report.head_name,iter.next().key());
            if(index!=-1)
            {
                double v =(iter.value().pass_num * 1.0)/(iter.value().pass_num+iter.value().fail_num) * 100;
                if(iter.value().pass_num+iter.value().fail_num<=0)
                {
                    mainw->Performance_Summary_Report.log_data[0][index]="NA";
                }
                else
                {
                    mainw->Performance_Summary_Report.log_data[0][index]= QString::number(v)+"%";
                }
            }

        }


        ///
        /// \brief 查询测试结束
        ///
        if(End_Test_Section == chaminfo.i_Step )
        {
            mainw->Title_Testing_Info(QString().fromLocal8Bit("最后一段测试..."));
            QDateTime now = QDateTime().currentDateTime();
            if(last_setp_run_time.secsTo(now) >= stepinfo.i_SetTimeH*60*60 + stepinfo.i_SetTimeM*60 )
            {
                Test_Result = true;
                break;
            }
        }
        add_log();
        sleep(5);
    }

    stop();
}



void ETest::add_log()
{
    for(int i =0 ; i<Flow_Result::L_portinfo.count() ;i++  )
    {
        //"Date"<<"SN"<<"Model"<<"Port"<<"Result"<<"TX"<<"RX"<<"Loss"<<"CRC"<<"Time"<<"Measure Temperature"<<"Set Temperature"<<"Measure Humidity"<<"Set Humidity"
        QVector<QString>  nv ;
        nv.append(QDateTime().currentDateTime().toString( "yyyy-MM-dd hh:mm:ss"));
        nv.append(mainw->SN);
        nv.append(Model_Name);
        nv.append(Flow_Result::Result);

        //添加接口文件项
        for(int j =0;j <Flow_Result::ItemName.count();j++)
        {
            nv.append(Flow_Result::L_portinfo[i].info[j]);
        }

        nv.append(QString::number(mainw->test_time));
        nv.append(QString::number(chaminfo.now_temperature));
        nv.append(QString::number(stepinfo.d_SetTem));
        nv.append(QString::number(chaminfo.now_humidity));
        nv.append(QString::number(stepinfo.d_SetHum));

        mainw->log_info.log_data.insert(mainw->log_info.rows,nv);
        mainw->log_info.rows++;

    }

    Flow_Result::L_portinfo.clear();
}

void ETest::simplification_graph(Graph_Info &graph, int times)
{
    if(graph.v_x.count() < 500)  return ;

    //精简到每times次显示一个点
    for(int i = 0; i< graph.v_x.count()-500;i++)
    {
        if(((int)graph.v_x[i])%times != 0)
        {
            graph.v_x.remove(i);
            graph.v_y.remove(i);
        }
    }

}

