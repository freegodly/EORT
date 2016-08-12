#include "flow_result.h"
#include <QStringList>
#include <qDebug>
#include "global.h"
#include "mainwindow.h"
#include <windows.h>

QString Flow_Result::Result_filepath = "F:/Code/Qt/Eort/result.txt";
QString  Flow_Result::Result;
QString  Flow_Result::Reason;
QList<PortInfo>  Flow_Result::L_portinfo;
QStringList Flow_Result::ItemName;

Flow_Result::Flow_Result(QObject *parent) :
    QObject(parent)
{
}

void Flow_Result::GetItmeName()
{
    QSettings *configIniRead = new QSettings(Flow_Result::Result_filepath , QSettings::IniFormat);
    QString itmename= configIniRead->value("/ITEMCFG/ItemName").toString();
    Flow_Result::ItemName = itmename.split("@");
    delete configIniRead;
}

bool Flow_Result::Query_Result()
{
    Flow_Result::L_portinfo.clear();

    int moudle=GetPrivateProfileIntA("STATUS", //节名
    "Mode", //项名
    -1, //没找到此项时的缺省返回值
   Flow_Result::Result_filepath.toStdString().c_str()); //配置文件的准确路径

    if(moudle == 0)
    {
         mainw->Testing_Info(QString().fromLocal8Bit("测试信息结果等待中..."));
        return false;
    }
    else if(moudle == -1)
    {
        mainw->Testing_Info(QString().fromLocal8Bit("测试信息文件读取失败！"));
        return false;
    }
    else
    {
        QSettings *configIniRead = new QSettings(Flow_Result::Result_filepath , QSettings::IniFormat);
        Flow_Result::Result=configIniRead->value("/STATUS/Result").toString();
        Flow_Result::Reason=configIniRead->value("/STATUS/Reason").toString();
        configIniRead->beginGroup("RESULT");
        QStringList portlist = configIniRead->allKeys();

        mainw->Testing_Info( "Result:"+Flow_Result::Result);
        mainw->Testing_Info("Reason:"+ Flow_Result::Reason);
        foreach (QString var, portlist)
        {
            qDebug()<<var;
            QString re=configIniRead->value(var).toString();
            qDebug()<<re;
            mainw->Testing_Info(re);
            QStringList parts = re.split("@");
            PortInfo PI;
            for(int i =0;i <parts.count();i++)
            {
                PI.info.append( parts[i]);
            }
            Flow_Result::L_portinfo.append(PI);
        }
        configIniRead->endGroup();
        configIniRead->beginGroup("STATUS");
        configIniRead->setValue("Mode",0);
        configIniRead->endGroup();
        delete configIniRead;
        return true;
    }
}
