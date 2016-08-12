/*!
* \file form_humidity.h
* \brief ʪ����ʾ����
*
*ʪ����ʾ����
*
* \author ningjian
* \version v1.00
* \date ����һ��������ʮ����������ʱʮ�ķ���ʮ���� ������
*/

#ifndef FORM_HUMIDITY_H
#define FORM_HUMIDITY_H

#include <QWidget>
#include <QPixmap>

#define ZERO_ROATE_H    51
#define STEP_10  22.3


namespace Ui {
class Form_Humidity;
}

class Form_Humidity : public QWidget
{
    Q_OBJECT

public:
    explicit Form_Humidity(QWidget *parent = 0);
    ~Form_Humidity();
    QPixmap back_pixmap;
    QPixmap needle_pixmap;
    QPixmap overlay_pixmap;
    double now_humidity;
    double goal_humidity;
private:
    Ui::Form_Humidity *ui;
    //ͨ��ʪ�Ȼ�ȡ��ת�Ƕ�
    double  getRoate(double humidity) ;
protected:
     void paintEvent(QPaintEvent *event);
};

#endif // FORM_HUMIDITY_H
