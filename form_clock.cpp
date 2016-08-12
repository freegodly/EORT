#include "form_clock.h"
#include "ui_form_clock.h"
#include <QTime>
#include <QTimer>
#include <QPainter>
Form_Clock::Form_Clock(QWidget *parent) :
    QWidget(parent),
    ui(new Ui::Form_Clock)
{
    ui->setupUi(this);


    QTimer *timer = new QTimer(this);

    connect(timer, SIGNAL(timeout()), this, SLOT(update()));


    now_temperature =0.0;
    goal_temperature =0.0;
    //设置背景

    QPixmap pixmap(":/res/degree_meter.png");
    back_pixmap = pixmap;

    timer->start(1000);

}

Form_Clock::~Form_Clock()
{
    delete ui;
}

double Form_Clock::getRoate(double temperature)
{
    return  temperature>45? temperature/20.0 * STEP_20 + ZERO_ROATE+(temperature-45)/10.0:temperature/20.0 * STEP_20 + ZERO_ROATE;
}

void Form_Clock::paintEvent(QPaintEvent *event)
{

    //ui->lcdNumber->display(QString("%1").arg(now_temperature));
    static const QPoint hourHand[3] = {
        QPoint(7, 8),
        QPoint(-7, 8),
        QPoint(0, -40)
    };
    static const QPoint minuteHand[3] = {
        QPoint(7, 8),
        QPoint(-7, 8),
        QPoint(0, -70)
    };

    QColor hourColor(127, 0, 127);
    QColor minuteColor(0, 127, 127, 191);

    int side = qMin(width(), height());
    QTime time = QTime::currentTime();

    QPainter painter(this);

    //设置背景
    painter.drawPixmap(0,0,this->width(),this->height(),back_pixmap);

    // 103 220
    //画数字 当前温度
   // painter.restore();
    QFont font("宋体",12); //使用wenquanyi字体
    painter.setFont(font);
    painter.setPen(minuteColor);
    painter.drawText(QRectF(107,215,90,20),QString("R:%1").arg(now_temperature));
    painter.setPen(hourColor);
    painter.drawText(QRectF(107,230,90,20),QString("G:%1").arg(goal_temperature));
    painter.setRenderHint(QPainter::Antialiasing);

    painter.translate(width() / 2, height() / 2);

    painter.scale(side / 200.0, side / 200.0);

    painter.setPen(Qt::NoPen);

    painter.setBrush(hourColor);

    painter.save();

    // painter.rotate(30.0 * ((time.hour() + time.minute() / 60.0)));
    painter.rotate(getRoate(goal_temperature) );
    painter.drawConvexPolygon(hourHand, 3);
    painter.restore();


    painter.setPen(Qt::NoPen);

    painter.setBrush(minuteColor);


    painter.save();
    //painter.rotate(6.0 * (time.minute() + time.second() / 60.0));

    painter.rotate(getRoate(now_temperature) );
    // -6  40   -35   20
    painter.drawConvexPolygon(minuteHand, 3);
    painter.restore();


}
