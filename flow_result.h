/*!
* \file flow_result.h
* \brief �������Խ�������
*
*�������Խ�������
*
* \author ningjian
* \version v1.00
* \date ����һ��������ʮ����������ʱʮ�ķ���ʮ���� ������
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
////0:nustream����д��״̬	1:ƽ̨���Զ�log
////nustream:ÿ0.5���1��log,Ϊ0ʱд��LOG������Mode=1;
////ƽ̨:ÿ0.5���1��log,Ϊ1ʱ��ȡLOG������Mode=0;
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


/// \brief �������Խ�������
///
/// �������Խ�������
class Flow_Result : public QObject
{
    Q_OBJECT
public:
    explicit Flow_Result(QObject *parent = 0);
    static QString Result;  ///< ���Խ��
    static QString Reason;  ///< ʧ��ԭ��
    static QList<PortInfo> L_portinfo;  ///< ������Ϣ�������
    static QStringList ItemName;

    static QString Result_filepath; ///< ���������ļ�·��

    static void GetItmeName();

    /// \brief ��ѯ���Խ��
    ///    \return ����˵����true ��ʾ�������Խ���
    static  bool   Query_Result();
signals:

public slots:

};

#endif // FLOW_RESULT_H
