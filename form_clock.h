/*!
* \file form_clock.h
* \brief 温度显示框类
*
*温度显示框类
*
* \author ningjian
* \version v1.00
* \date 二〇一四年七月十六日下午三时十四分四十八秒 星期三
*/


#ifndef FORM_CLOCK_H
#define FORM_CLOCK_H

#include <QWidget>
#include <QPixmap>

#define ZERO_ROATE    -64
#define STEP_20  29


namespace Ui {
class Form_Clock;
}

class Form_Clock : public QWidget
{
    Q_OBJECT

public:
    explicit Form_Clock(QWidget *parent = 0);
    ~Form_Clock();
     QPixmap back_pixmap;


     //现在温度
     double now_temperature;
     //目标温度
     double goal_temperature;

private:
    Ui::Form_Clock *ui;
     //通过温度获取旋转角度
     double  getRoate(double temperature) ;
protected:
    void paintEvent(QPaintEvent *event);
};

#endif // FORM_CLOCK_H
