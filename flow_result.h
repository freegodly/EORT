/*!
* \file flow_result.h
* \brief 流量测试结果检查类
*
*流量测试结果检查类
*
* \author ningjian
* \version v1.00
* \date 二〇一四年七月十六日下午三时十四分四十八秒 星期三
*/

#ifndef FLOW_RESULT_H
#define FLOW_RESULT_H

#include <QObject>
#include <QSettings>
#include <QList>
#include <QProcess>



////FileVersion=V1.0
//[STATUS]
//Mode=0
////0:nustream可以写入状态	1:平台可以读log
////nustream:每0.5秒读1次log,为0时写入LOG并设置Mode=1;
////平台:每0.5秒读1次log,为1时读取LOG并设置Mode=0;
//Result=PASS
////PASS%FAIL
//Reason=

//[RESULT]
//port1=1000;1000;0;0
//port2=1000;1000;0;0
//port3=1000;1000;0;0
//port4=1000;1000;0;0
//port5=1000;1000;0;0
//port6=1000;1000;0;0
//port7=1000;1000;0;0
//port8=1000;1000;0;0
//port9=1000;1000;0;0
//port10=1000;1000;0;0

////tx;rx;loss;crc
///

struct PortInfo
{
   QStringList info;
};


/// \brief 流量测试结果检查类
///
/// 流量测试结果检查类
class Flow_Result : public QObject
{
    Q_OBJECT
public:
    explicit Flow_Result(QObject *parent = 0);
    static QString Result;  ///< 测试结果
    static QString Reason;  ///< 失败原因
    static QList<PortInfo> L_portinfo;  ///< 流量信息结果保存
    static QStringList ItemName;

    static QString Result_filepath; ///< 流量交互文件路径

    static void GetItmeName();

    /// \brief 查询测试结果
    ///    \return 返回说明：true 表示流量测试结束
    static  bool   Query_Result();
signals:

public slots:

};

#endif // FLOW_RESULT_H
