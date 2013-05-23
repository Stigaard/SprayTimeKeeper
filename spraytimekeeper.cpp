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

SprayTimeKeeper::SprayTimeKeeper(QObject* parent, NozzleControl* nz): QObject(parent)
{
  this->nozzlecontrol = nz;
  schedule = new QVector<SprayTimeKeeperSchedule*>;
  for(int i = 0; i<numNozzles; i++)
  {
    //SprayTimeKeeperSchedule sched;
    schedule->append(new SprayTimeKeeperSchedule);
    schedule->at(i)->setNozzleID(i);
    connect((this->schedule->at(i)), SIGNAL(spray(quint8,bool)), this->nozzlecontrol, SLOT(spray(quint8,bool)));
  }
  requests = new QVector< QVector<sprayTimeKeeperRequest*> >(numNozzles);
}

void SprayTimeKeeper::Spray(int NozzleID, qint64 startTime, qint64 endTime)
{
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
    return;
  }
  
  schedule->at(NozzleID)->lock.lock();
  /* Check interference with earlier request, and remove them if this is the case */
  if(!requests[NozzleID].empty())
  {
    int cnt = requests->at(NozzleID).count();
    for(int i=0; i<cnt;)
    {
      if(requests->at(NozzleID).at(i)->interferes(startTime, endTime))
      {
	(*requests)[NozzleID].remove(i);
	--cnt;
      }
      else
	i++;
    }
  }
  
  /* create new request */
  (*requests)[NozzleID].push_back(new sprayTimeKeeperRequest(startTime, endTime, *schedule->at(NozzleID)));
  schedule->at(NozzleID)->lock.unlock();
}