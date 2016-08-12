/*!
* \file form_clock.h
* \brief �¶���ʾ����
*
*�¶���ʾ����
*
* \author ningjian
* \version v1.00
* \date ����һ��������ʮ����������ʱʮ�ķ���ʮ���� ������
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


     //�����¶�
     double now_temperature;
     //Ŀ���¶�
     double goal_temperature;

private:
    Ui::Form_Clock *ui;
     //ͨ���¶Ȼ�ȡ��ת�Ƕ�
     double  getRoate(double temperature) ;
protected:
    void paintEvent(QPaintEvent *event);
};

#endif // FORM_CLOCK_H
