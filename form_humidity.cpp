#include "form_humidity.h"
#include "ui_form_humidity.h"
#include <QTime>
#include <QTimer>
#include <QPainter>
#include <random>
#include <QTime>
Form_Humidity::Form_Humidity(QWidget *parent) :
    QWidget(parent),
    ui(new Ui::Form_Humidity)
{
    QTime  time= QTime::currentTime();
    qsrand(time.msec()+time.second()*1000);
    ui->setupUi(this);

    //ÉèÖÃ±³¾°

    QPixmap pixmap(":/res/humidity.png");
    back_pixmap = pixmap;

    QPixmap needle_pixmap_t(":/res/needle.png");
    needle_pixmap= needle_pixmap_t;

    QPixmap overlay_pixmap_t(":/res/overlay.png");
    overlay_pixmap= overlay_pixmap_t;


    QTimer *timer = new QTimer(this);

    connect(timer, SIGNAL(timeout()), this, SLOT(update()));

    timer->start(1000);
   now_humidity = 0.0;
   goal_humidity =0.0;
}

Form_Humidity::~Form_Humidity()
{
    delete ui;
}

double Form_Humidity::getRoate(double humidity)
{
    return humidity/10.0 * STEP_10 + ZERO_ROATE_H;
}

void Form_Humidity::paintEvent(QPaintEvent *event)
{
    QPainter painter(this);

    //ÉèÖÃ±³¾°
    painter.drawPixmap(0,0,this->width(),this->height(),back_pixmap);


    QColor hourColor(127, 0, 127);
    QColor minuteColor(0, 127, 127, 191);
    QFont font("ËÎÌå",12);
    painter.setFont(font);
    painter.setPen(minuteColor);
    painter.drawText(QRectF(107,215,90,20),QString("R:%1%").arg(now_humidity));
    painter.setPen(hourColor);
    painter.drawText(QRectF(107,230,90,20),QString("G:%1%").arg(goal_humidity));

    QFont font1("¿¬Ìå",15);
    painter.setFont(font1);
    painter.setPen(Qt::blue);
    painter.drawText(QPoint(90,200),"Hygrometer  ");
    painter.save();

     int side = qMin(width(), height());
     painter.setRenderHint(QPainter::Antialiasing);

     painter.translate(width() / 2, height() / 2);

     painter.scale(side / 200.0,side / 200.0);

     painter.setPen(Qt::green);

     painter.setBrush(Qt::green);



     painter.rotate(getRoate(now_humidity) );
     painter.drawPixmap(0,0,8,63,needle_pixmap);
     painter.restore();
     painter.drawPixmap(50,60,overlay_pixmap.width(),overlay_pixmap.height(),overlay_pixmap);
}
