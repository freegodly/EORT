#include "mainwindow.h"
#include <QApplication>
#include <QTextCodec>
#include "flow_result.h"
#include "global.h"
#include "QIcon"
int main(int argc, char *argv[])
{
    QApplication a(argc, argv);

    // ���²��ֽ����������
//    QTextCodec::setCodecForTr(QTextCodec::codecForName("GB2312"));
      QTextCodec::setCodecForLocale(QTextCodec::codecForName("GBK"));
//    QTextCodec::setCodecForCStrings(QTextCodec::codecForName("GB2312"));
    // ���ϲ��ֽ����������


    mainw = new MainWindow;
    mainw->show();

    //a.setWindowIcon(QIcon("eort.ico"));
    return a.exec();
}
