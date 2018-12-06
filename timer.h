#ifndef TIMER_H
#define TIMER_H
#include <QTime>

/*
*******************************************************************************
Class Timer
Contém um elemento QTime responsável pela temporização das jogadas
-------------------------------------------------------------------------------
*******************************************************************************
*/
class Timer
{
public:
    Timer();
    QTime time; //atributo responsável pela temporização.
    void startTimer();
    QTime getQTimer();
    int getSeconds();
};

#endif // TIMER_H
