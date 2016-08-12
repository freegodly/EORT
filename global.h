/*!
* \file global.h
* \brief 全局信息
*
*全局信息
*
* \author ningjian
* \version v1.00
* \date 二〇一四年七月十六日下午三时十四分四十八秒 星期三
*/


#ifndef GLOBAL_H
#define GLOBAL_H
#include <QTextCodec>
#include <QVector>
#include <QMap>
#include <QStringList>


 #include <libxl.h>




using namespace libxl;


#include <QDomDocument>
#include <QTextStream>

class MainWindow;

extern MainWindow* mainw;       ///< 全局主窗体指针


/// \brief 图示信息
///
/// 图示信息
struct Graph_Info
{
    QVector<double> v_x;    ///< x值列表
    QVector<double> v_y;    ///< y值列表
    QString  name;              ///<  图示的名字

    /// \brief 清除原有数据
    /// \param sname 图示的名字
    ///    \return void
    void clear(QString sname)
    {
        v_x.clear();
        v_y.clear();
        name = sname;
    }
};


/// \brief 测试LOG信息
///
/// 测试LOG信息
struct Log_Info
{
    int rows;                        ///>    log 数目
    int cols;                          ///>    log 项数
    QStringList  head_name;        ///>   每项的名字
    QMap<int, QVector<QString> > log_data;      ///>   数据索引 和 每行的数据  集合
};


///
///  \brief 资料接口
///
struct ChamberInfo
{
    //温度
    double goal_temperature;        ///< 温度
    double now_temperature;
    //湿度
    double now_humidity;        ///< 湿度
    double goal_humidity;

    long hour;
    long min;

    int i_Pattern;  //程式组编号
    int i_Step;    //段数编号

    char error[1024];
};

//段数信息
struct StepInfo
{
    int i_Pattern;  //程式组编号
    int i_Step;    //段数编号

    double d_SetTem; //温度设置值;
    double d_SetHum; // 湿度设置值

    int i_SetTimeH; // 时间设定值小时
    int i_SetTimeM; // 时间设定值分钟

    int i_TemStandbysignal; //温度待机信号
    int i_HumStandbysignal;  //湿度待机信号
    int i_Timesignal[3]; //时间信号 i_Timesignal[0] 是否震动 i_Timesignal[1]通断点 i_Timesignal[2]是否除霜
                        //0为否 1为是

    char error[1024];
};


/// \brief 测试时实信息
///
/// 测试时实信息
struct Eort_Info
{
    //温度
    double goal_temperature;        ///< 温度
    double now_temperature;
    //湿度
    double now_humidity;        ///< 湿度
    double goal_humidity;

    //段
    int section;
    int pass_num;
    int fail_num;
    QString description;

    //Graph_Info
    Graph_Info goal_temperature_graph_info;
    Graph_Info now_temperature_graph_info;
    Graph_Info now_humidity_graph_info;
    Graph_Info goal_humidity_graph_info;
    Graph_Info result_graph_info;
    Graph_Info shake_graph_info;

    Graph_Info tx_graph_info;
    Graph_Info rx_graph_info;

    ChamberInfo  chaminfo;
    StepInfo    stepInfo;
    //震动
    int shake;
};


/// \brief 测试状态
///
/// 测试状态
 enum Testing_Status{running = 0 , pause , stop  };






 struct Test_Point
 {
     int section;
     QString time;
     double temperature;
     double humidity;
     int shake;
     bool power;
     bool flow;
     QString description;

     int pass_num;
     int fail_num;

 };

 struct Test_Profile
 {
         QString Name;
         QVector<Test_Point> L_Test_Point;
         double max_temperature;
         double min_temperature;
 };

struct PF
{
    int pass_num;
    int fail_num;
};





typedef     bool (*GetInformation)(int com_port, ChamberInfo &data);
typedef     bool (*GetStepSetInfo)(int com_port, int i_Pattern,int i_Step,StepInfo &data);

 //震动接口
typedef bool (*StartTest)(int com_port, char data[]);
typedef bool (*StopTest)(int com_port, char data[]);
typedef bool (*StartShake)(int com_port,int iHz, char data[]);
typedef bool (*StopShake)(int com_port, char data[]);

 //操作设定
typedef bool (*Oper_Run)(int com_port, char data[]);
typedef bool (*Oper_Stop)(int com_port, char data[]);
typedef bool (*Oper_HoldOn)(int com_port, char data[]);
typedef bool (*Oper_KeepOn)(int com_port, char data[]);
typedef bool (*Oper_Advence)(int com_port, char data[]);

extern GetInformation getInformation;
extern GetStepSetInfo getStepSetInfo;
extern StartTest startTest;
extern StopTest stopTest;
extern StartShake startShake;
extern StopShake stopShake;
extern Oper_Run  oper_Run;
extern Oper_Stop oper_Stop;
extern Oper_HoldOn oper_HoldOn;
extern Oper_KeepOn oper_KeepOn;
extern Oper_Advence oper_Advence;



class Global
{
public:
    Global();
    static QDomDocument config_doc;
    static QDomNode FindXml(QDomDocument &doc,QString nodename);
    static  Test_Profile  test_profile;
    static QString GetValueXml( QString nodename,QString att);

   static QString Soft_Version;

    static void Save_report(QString name,QString info);

    static int Query_Test_Profile_Point(int session);

    static int Query_Test_Profile_Max_Section();

    static int Find_QStringList_Index(QStringList &s, QString name);
};


#endif // GLOBAL_H
