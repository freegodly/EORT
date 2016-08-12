#include "global.h"
#include "mainwindow.h"

#include <QSettings>

MainWindow* mainw;


QDomDocument Global::config_doc;
Test_Profile Global::test_profile;
QString Global::Soft_Version ="SV:2.3.0";

GetInformation getInformation;
GetStepSetInfo getStepSetInfo;
StartTest startTest;
StopTest stopTest;
StartShake startShake;
StopShake stopShake;
Oper_Run  oper_Run;
Oper_Stop oper_Stop;
Oper_HoldOn oper_HoldOn;
Oper_KeepOn oper_KeepOn;
Oper_Advence oper_Advence;



Global::Global()
{
}

QDomNode Global::FindXml(QDomDocument &doc, QString nodename)
{
    QDomNode node;
    QDomElement root = doc.documentElement();
    QDomNodeList  li = root.childNodes();
    for(int i=0; i<li.count(); i++)
    {
        node= li.at(i);
        if(  node.nodeName() == nodename )
        {
            return node;
        }
    }
    return node;
}

QString Global::GetValueXml(QString nodename, QString att)
{
    QDomNode node = FindXml(Global::config_doc,QString(nodename));
    if(!node.isNull() )
    {
        return node.attributes().namedItem(att).nodeValue();
    }
    return "";
}



void Global::Save_report(QString name, QString info)
{
    Book* report =  xlCreateBook();
    if( !  report->load("config/report.xls"))
    {
        qDebug()<<report->errorMessage();
        return ;
    }
    Sheet * sheet = report->getSheet(1);
    // Sheet * sheet = report->addSheet("Test");


    for(int i=0; i <mainw->log_info.cols ;i++)
    {
        sheet->writeStr(0,i,mainw->log_info.head_name[i].toStdString().c_str());
    }

    for(int j =0 ; j<mainw->log_info.rows ; j++)
    {
        for(int i=0; i <mainw->log_info.cols ;i++)
        {
            sheet->writeStr(j+1,i,mainw->log_info.log_data[j][i].toStdString().c_str());
        }
    }


    mainw->Auto_SaveImage();
    sheet = report->getSheet(2);
    int pid = report->addPicture("data.png");
    sheet->setPicture(0,0,pid);


    sheet = report->getSheet(4);
    sheet->writeStr(0,0,"Section");
    sheet->writeStr(0,1,"Time");
    sheet->writeStr(0,2,"Temperature");
    sheet->writeStr(0,3,"Humidity");
    sheet->writeStr(0,4,"Shake");
    sheet->writeStr(0,5,"Power");
    sheet->writeStr(0,6,"IsTest");
    sheet->writeStr(0,7,"Description");
    sheet->writeStr(0,8,"Pass");
    sheet->writeStr(0,9,"Fail");
    sheet->writeStr(0,10,"FPY");
    for(int i =0 ; i <  Global::test_profile.L_Test_Point.count() ; i++)
    {
        sheet->writeStr(i+1,0, QString::number(Global::test_profile.L_Test_Point[i].section).toStdString().c_str());
        sheet->writeStr(i+1,1, Global::test_profile.L_Test_Point[i].time.toStdString().c_str());
        sheet->writeStr(i+1,2,QString::number(Global::test_profile.L_Test_Point[i].temperature).toStdString().c_str());
        sheet->writeStr(i+1,3,QString::number(Global::test_profile.L_Test_Point[i].humidity).toStdString().c_str());
        sheet->writeStr(i+1,4, QString::number(Global::test_profile.L_Test_Point[i].shake).toStdString().c_str());
        sheet->writeStr(i+1,5, Global::test_profile.L_Test_Point[i].power == true ? "True":"False");
        sheet->writeStr(i+1,6, Global::test_profile.L_Test_Point[i].flow == true ? "True":"False");
        sheet->writeStr(i+1,7, Global::test_profile.L_Test_Point[i].description.toStdString().c_str());
        sheet->writeStr(i+1,8, QString::number(Global::test_profile.L_Test_Point[i].pass_num).toStdString().c_str());
        sheet->writeStr(i+1,9, QString::number(Global::test_profile.L_Test_Point[i].fail_num).toStdString().c_str());
        sheet->writeStr(i+1,10,QString::number((Global::test_profile.L_Test_Point[i].pass_num*1.0)/(Global::test_profile.L_Test_Point[i].pass_num+Global::test_profile.L_Test_Point[i].fail_num)*100).toStdString().c_str());
    }



    sheet = report->getSheet(0);
    mainw->Performance_Summary_Report.log_data[0][mainw->Performance_Summary_Report.cols-1] = mainw->etest.Test_Result== true ?  "Pass":"Fail";
    for(int i = 0; i<mainw->Performance_Summary_Report.head_name.count();i++)
    {
        sheet->writeStr(2,i,mainw->Performance_Summary_Report.head_name[i].toStdString().c_str());
        sheet->writeStr(3,i,mainw->Performance_Summary_Report.log_data[0][i].toStdString().c_str());
    }



    sheet = report->getSheet(3);
    //由于log信息太长，暂时不写如excel
    sheet->writeStr(0,0,Global::Soft_Version.toLocal8Bit());

    report->save(name.toStdString().c_str());
    report->release();
}

int Global::Query_Test_Profile_Point(int session)
{
    for(int i =0; i<Global::test_profile.L_Test_Point.count(); i++ )
    {
        if(Global::test_profile.L_Test_Point[i].section == session  )
        {
            return i;
        }
    }

    return -1;
}

int Global::Query_Test_Profile_Max_Section()
{
    int sec=0;
    for(int i =0; i<Global::test_profile.L_Test_Point.count(); i++ )
    {
        if(Global::test_profile.L_Test_Point[i].section > sec  )
        {
            sec = Global::test_profile.L_Test_Point[i].section;
        }
    }

    return sec;
}

int Global::Find_QStringList_Index(QStringList &s,QString name)
{
    for(int i = 0; i <s.count();i++)
    {
        if(s[i] == name)
            return i;
    }
    return -1;
}
