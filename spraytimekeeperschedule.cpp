/*
    Copyright (c) 2013, Morten S. Laursen <email>
    All rights reserved.

    Redistribution and use in source and binary forms, with or without
    modification, are permitted provided that the following conditions are met:
        * Redistributions of source code must retain the above copyright
        notice, this list of conditions and the following disclaimer.
        * Redistributions in binary form must reproduce the above copyright
        notice, this list of conditions and the following disclaimer in the
        documentation and/or other materials provided with the distribution.
        * Neither the name of the <organization> nor the
        names of its contributors may be used to endorse or promote products
        derived from this software without specific prior written permission.

    THIS SOFTWARE IS PROVIDED BY Morten S. Laursen <email> ''AS IS'' AND ANY
    EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE IMPLIED
    WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE
    DISCLAIMED. IN NO EVENT SHALL Morten S. Laursen <email> BE LIABLE FOR ANY
    DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES
    (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES;
    LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND
    ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT
    (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF THIS
    SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
*/


#include "spraytimekeeperschedule.h"
#include <QDateTime>
#include "spraytimekeeperschedule.moc"
#include <iostream>

SprayTimeKeeperSchedule::SprayTimeKeeperSchedule(QObject* parent): QThread(parent)
{
  this->start();
}

void SprayTimeKeeperSchedule::addSprayAction(qint64 time, bool action)
{
  this->actions.insert(time, action);
}

void SprayTimeKeeperSchedule::removeSprayAction(qint64 time)
{
  this->actions.remove(time);
}

void SprayTimeKeeperSchedule::setNozzleID(quint8 id)
{
  this->nozzleID = id;
}


void SprayTimeKeeperSchedule::run()
{
  while(true)
  {
    qint64 sleepTime = 500;
    this->lock.lock();
    if(this->actions.isEmpty())
    {
      this->lock.unlock();
      this->msleep(sleepTime);
      continue;
    } 
    qint64 time = QDateTime::currentMSecsSinceEpoch();
    qint64 nextTime = this->actions.begin().key();
    qint64 waitTime = (nextTime/1000) - time;
    std::cout << "time:		" << time << std::endl;
    std::cout << "nextTime:	" << nextTime/1000 << std::endl;
    std::cout << "waitTime:	" << waitTime << std::endl;
    if(waitTime < 10)
    {
      bool action = this->actions.take(nextTime);
      this->lock.unlock();
      emit(spray(nozzleID, action)); 
    }
    else
    {
      this->lock.unlock();
      if(waitTime < sleepTime)
	this->msleep(waitTime-5);
      else
	this->msleep(sleepTime);
    }
  }
  exec();
}


