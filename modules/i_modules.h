/*******************************************************************
 *  Copyright (C) 2014, All right reserved.
 *
 *  @filename        i_modules.h
 *
 *  @version         1.0
 *
 *  @autho           NingJian
 *
 *  @description
 *                  模块接口类
 *
 *  @history        2014-7-8 14:14:02 created by NingJian
 *
 *  @note
 *
 *  TODO:
 *
 *******************************************************************/


#ifndef I_MODULES_H
#define I_MODULES_H

#include <QObject>

class I_Modules : public QObject
{
    Q_OBJECT
public:
    explicit I_Modules(QObject *parent = 0);
    virtual bool init() = 0;
signals:

public slots:

};

#endif // I_MODULES_H
