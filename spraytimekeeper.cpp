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

#include <iostream>
#include <QDateTime>
#include "spraytimekeeper.h"
#include "spraytimekeeper.moc"

SprayTimeKeeper::SprayTimeKeeper(QObject* parent, NozzleControl* nz): QThread(parent)
{
  this->nozzlecontrol = nz;
#ifdef USE_DATALOGGER
  this->log = new LoggerModule("../Logging", "SpraytimeCommands");
#endif
  schedule = new QVector<SprayTimeKeeperSchedule*>;
  for(int i = 0; i<numNozzles; i++)
  {
    //SprayTimeKeeperSchedule sched;
    schedule->append(new SprayTimeKeeperSchedule);
    (*schedule)[i]->setNozzleID(i);
    connect(((*schedule)[i]), SIGNAL(spray(quint8,bool)), this->nozzlecontrol, SLOT(spray(quint8,bool)));
  }
  requests = new QVector< QVector<sprayTimeKeeperRequest*> >(numNozzles);
  this->start();
}

void SprayTimeKeeper::run()
{
  while(true)
  {
    qint64 now = QDateTime::currentMSecsSinceEpoch()*1000;
    for(int j = 0; j< numNozzles;j++)
    {
      this->requestLock[j].lock();
      int cnt = (*requests)[j].count();
      for(int i=0; i<cnt;)
      {
	if((*requests)[j][i]->getEndTime() < now)
	{
	  (*requests)[j].remove(i);
	  --cnt;
	}
	else
	  i++;
      }
      this->requestLock[j].unlock();
    }
    this->msleep(1000);
    }
}


void SprayTimeKeeper::Spray(int NozzleID, qint64 startTime, qint64 endTime)
{
#ifdef USE_DATALOGGER
  log->log("startTime", QString::number(startTime).toLocal8Bit().constData());
  log->log("endTime", QString::number(endTime).toLocal8Bit().constData());
  log->log("NozzleID", NozzleID);
#endif
  //std::cout << "Received request with Starttime:" << startTime << "	endTime:" << endTime << std::endl;
  /* Check if nozzleID is valid*/
  if(NozzleID < 0 || NozzleID >= numNozzles)
  {
    std::cerr << "Invalid NozzleID !" << std::endl;
    return;
  }
  
  /* Check if endtime time is valid */
  if(endTime<= startTime)
  {
    std::cerr << "Invalid endTime !" << std::endl;
    return;
  }
  
  /* Check if starttime time is valid */
  if(startTime < QDateTime::currentMSecsSinceEpoch()*1000) 
  {
    std::cerr << "Start time lies in the past !" << std::endl;
    std::cerr << "current time:	" << QDateTime::currentMSecsSinceEpoch()*1000 << std::endl;
    std::cerr << "Start Time:	" << startTime << std::endl;
    return;
  }
  
  (*schedule)[NozzleID]->lock.lock();
  this->requestLock[NozzleID].lock();
  /* Check interference with earlier request, and remove them if this is the case */
  if(!(*requests)[NozzleID].empty())
  {
    int cnt = (*requests)[NozzleID].count();
    for(int i=0; i<cnt;)
    {
      if((*requests)[NozzleID][i]->interferes(startTime, endTime))
      {
	(*requests)[NozzleID].remove(i);
	--cnt;
      }
      else
	i++;
    }
  }
  
  
  /* create new request */
  (*requests)[NozzleID].push_back(new sprayTimeKeeperRequest(startTime, endTime, *((*schedule)[NozzleID])));
  this->requestLock[NozzleID].unlock();
  (*schedule)[NozzleID]->lock.unlock();
}
