#include "dialog_setting.h"
#include "ui_dialog_setting.h"
#include "global.h"
#include <QComboBox>
#include <QTableWidget>
#include <QFile>
#include <QLineEdit>
#include <QComboBox>
#include <QFileDialog>
#include <QMenu>
#include <QCheckBox>

Dialog_Setting::Dialog_Setting(QWidget *parent) :
    QDialog(parent),
    ui(new Ui::Dialog_Setting)
{
    ui->setupUi(this);

    //    QFile styleSheet(":/files/transparent.qss");

    //    if (!styleSheet.open(QIODevice::ReadOnly)) {
    //        qWarning("Unable to open :/files/transparent.qss");
    //        return;
    //    }
    //    this->setStyleSheet(styleSheet.readAll());
//    QList<QAction*> qa;
//    qa.append(ui->Insert_Row);
//    qa.append(ui->Delete_Row);
  //  ui->tableWidget_profile->setContextMenuPolicy(Qt::CustomContextMenu);
    LoadConfig(this->children());
    Load_Profile();
}

Dialog_Setting::~Dialog_Setting()
{
    delete ui;
}

void Dialog_Setting::SaveConfig()
{
    Global::config_doc.clear();
    QDomElement  root = Global::config_doc.createElement("Config");
    Global::config_doc.appendChild(root);

    QObjectList list =this->children();
    traversalControl(list,&root);

    QFile f("./config/config.xml");
    if(f.open(QFile::WriteOnly))
    {
        QTextStream out(&f);
        out.setCodec("UFT-8"); //中文编码
        //out.setCodec("GB18030-0"); //中文编码

        Global::config_doc.save(out, QDomNode::EncodingFromTextStream);
        //           QByteArray by= Global::config_doc.toByteArray() ;
        //           f.write(by.toBase64());
        f.close();
    }
}

void Dialog_Setting::LoadConfig(const QObjectList &q)
{
    for(int i=0;i<q.length();i++)
    {

        if(!q.at(i)->children().empty())
        {
            LoadConfig(q.at(i)->children());
        }

        QObject* obj = q.at(i);
        QLineEdit *b = qobject_cast<QLineEdit*>(obj);
        if(b)
        {
            QString Name = obj->objectName();
            QDomNode node = Global::FindXml(Global::config_doc,obj->objectName());
            if(!node.isNull() ) b->setText(node.attributes().namedItem("Value").nodeValue() );

        }
        QComboBox *c = qobject_cast<QComboBox*>(obj);
        if(c)
        {
            QDomNode node =  Global::FindXml(Global::config_doc,obj->objectName());
            if(!node.isNull() )
            {
                // int index = c->findText(node.attributes().namedItem("Value").nodeValue().toInt());
                //改为配置文件保存索引而不是值
                int index = node.attributes().namedItem("Value").nodeValue().toInt();
                c->setCurrentIndex(index-1);
            }

        }

         QCheckBox *qc = qobject_cast<QCheckBox*>(obj);
         if(qc)
         {
             QDomNode node =  Global::FindXml(Global::config_doc,obj->objectName());
             if(!node.isNull() )
             {
                 // int index = c->findText(node.attributes().namedItem("Value").nodeValue().toInt());
                 //改为配置文件保存索引而不是值
                 QString v = node.attributes().namedItem("Value").nodeValue();
                 if(v == "True")
                 {
                     qc->setChecked(true);
                 }else{
                     qc->setChecked(false);
                 }
             }

         }



        //            CodeEditor *p = qobject_cast<CodeEditor*>(obj);
        //            if(p)
        //            {
        //                QDomNode node = FindXml(Global::config_doc,obj->objectName());
        //                if(!node.isNull() ) p->setPlainText(node.attributes().namedItem("Value").nodeValue() );
        //            }

        QTableWidget *tw = qobject_cast<QTableWidget*>(obj);
        if(tw)
        {

        }
    }
}

void Dialog_Setting::traversalControl(const QObjectList &q, QDomElement *root)
{
    for(int i=0;i<q.length();i++)
    {

        if(!q.at(i)->children().empty())
        {
            traversalControl(q.at(i)->children(),root);
        }

        QObject* obj = q.at(i);
        QLineEdit *b = qobject_cast<QLineEdit*>(obj);
        if(b)
        {
            QDomElement  element = Global::config_doc.createElement(obj->objectName() );
            element.setAttribute("Value",b->text());
            element.setAttribute("Type","QLineEdit");
            root->appendChild(element);
        }
        QComboBox *c = qobject_cast<QComboBox*>(obj);
        if(c)
        {
            QDomElement  element = Global::config_doc.createElement(obj->objectName() );
            element.setAttribute("Type","QComboBox");
            // element.setAttribute("Value",c->currentText());
            //改为配置文件保存索引而不是值
            element.setAttribute("Value",c->currentIndex()+1);
            root->appendChild(element);
        }


        QCheckBox *qc = qobject_cast<QCheckBox*>(obj);
        if(qc)
        {


            QDomElement  element = Global::config_doc.createElement(obj->objectName() );
            element.setAttribute("Type","QCheckBox");
            // element.setAttribute("Value",c->currentText());
            //改为配置文件保存索引而不是值
            if(qc->checkState() == Qt::Checked)
                 element.setAttribute("Value","True");
            else
                element.setAttribute("Value","False");


            root->appendChild(element);

        }


        //           CodeEditor *p = qobject_cast<CodeEditor*>(obj);
        //           if(p)
        //           {
        //               QDomElement  element = Global::config_doc.createElement(obj->objectName() );
        //               element.setAttribute("Value",p->toPlainText());
        //               root->appendChild(element);
        //           }
        QTableWidget *tw = qobject_cast<QTableWidget*>(obj);
        if(tw)
        {

        }
    }
}

void Dialog_Setting::Load_Profile()
{
    // Global::config_doc
    // ui->tableWidget_profile
}

void Dialog_Setting::closeEvent(QCloseEvent *event)
{
    SaveConfig();
}





void Dialog_Setting::on_tabWidget_destroyed()
{

}

void Dialog_Setting::on_pushButton_SelectFile_clicked()
{
    QString file = QFileDialog::getOpenFileName(this, QString::fromLocal8Bit("选择程序"), " ",  QString::fromLocal8Bit("测试程序(*.exe)"));
    ui->Flow_Path->setText(file);
}

void Dialog_Setting::on_pushButton_SelectFile_File_clicked()
{
    QString file = QFileDialog::getOpenFileName(this, QString::fromLocal8Bit("选择交互文件"), " ",  QString::fromLocal8Bit("测试交互文件(*.txt *.ini)"));
    ui->Flow_File->setText(file);
}

void Dialog_Setting::contextMenuRequest(QPoint pos)
{
//        QMenu *menu = new QMenu(ui->tableWidget_profile);
//       // menu->setAttribute(Qt::WA_DeleteOnClose);

//      //  if (ui->tableWidget_profile->      ->selectTest(pos, true) >= 0) // context menu on legend requested
//        {
//            //menu->addAction("Move to top left", this, SLOT(on_pushButton_SelectFile_clicked()));
//            QList<QAction*> qa;
//            qa.append(ui->Insert_Row);
//             qa.append(ui->Delete_Row);
//            menu->addActions(qa);

//        }

//         menu->popup(QCursor::pos());
}
