/*!
* \file global.h
* \brief ȫ����Ϣ
*
*ȫ����Ϣ
*
* \author ningjian
* \version v1.00
* \date ����һ��������ʮ����������ʱʮ�ķ���ʮ���� ������
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

extern MainWindow* mainw;       ///< ȫ��������ָ��


/// \brief ͼʾ��Ϣ
///
/// ͼʾ��Ϣ
struct Graph_Info
{
    QVector<double> v_x;    ///< xֵ�б�
    QVector<double> v_y;    ///< yֵ�б�
    QString  name;              ///<  ͼʾ������

    /// \brief ���ԭ������
    /// \param sname ͼʾ������
    ///    \return void
    void clear(QString sname)
    {
        v_x.clear();
        v_y.clear();
        name = sname;
    }
};


/// \brief ����LOG��Ϣ
///
/// ����LOG��Ϣ
struct Log_Info
{
    int rows;                        ///>    log ��Ŀ
    int cols;                          ///>    log ����
    QStringList  head_name;        ///>   ÿ�������
    QMap<int, QVector<QString> > log_data;      ///>   �������� �� ÿ�е�����  ����
};


///
///  \brief ���Ͻӿ�
///
struct ChamberInfo
{
    //�¶�
    double goal_temperature;        ///< �¶�
    double now_temperature;
    //ʪ��
    double now_humidity;        ///< ʪ��
    double goal_humidity;

    long hour;
    long min;

    int i_Pattern;  //��ʽ����
    int i_Step;    //�������

    char error[1024];
};

//������Ϣ
struct StepInfo
{
    int i_Pattern;  //��ʽ����
    int i_Step;    //�������

    double d_SetTem; //�¶�����ֵ;
    double d_SetHum; // ʪ������ֵ

    int i_SetTimeH; // ʱ���趨ֵСʱ
    int i_SetTimeM; // ʱ���趨ֵ����

    int i_TemStandbysignal; //�¶ȴ����ź�
    int i_HumStandbysignal;  //ʪ�ȴ����ź�
    int i_Timesignal[3]; //ʱ���ź� i_Timesignal[0] �Ƿ��� i_Timesignal[1]ͨ�ϵ� i_Timesignal[2]�Ƿ��˪
                        //0Ϊ�� 1Ϊ��

    char error[1024];
};


/// \brief ����ʱʵ��Ϣ
///
/// ����ʱʵ��Ϣ
struct Eort_Info
{
    //�¶�
    double goal_temperature;        ///< �¶�
    double now_temperature;
    //ʪ��
    double now_humidity;        ///< ʪ��
    double goal_humidity;

    //��
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
    //��
    int shake;
};


/// \brief ����״̬
///
/// ����״̬
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

 //�𶯽ӿ�
typedef bool (*StartTest)(int com_port, char data[]);
typedef bool (*StopTest)(int com_port, char data[]);
typedef bool (*StartShake)(int com_port,int iHz, char data[]);
typedef bool (*StopShake)(int com_port, char data[]);

 //�����趨
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
