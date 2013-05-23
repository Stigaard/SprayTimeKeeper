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


#include "spraytimekeeperrequest.h"
#include "spraytimekeeperrequest.moc"

bool sprayTimeKeeperRequest::interferes(qint64 startTime, qint64 endTime)
{
  bool startInterference = (this->startTime >= startTime) && (this->startTime <= endTime);
  bool endInterference = (this->endTime >= startTime) && (this->endTime <= endTime);
  return startInterference || endInterference;
}

sprayTimeKeeperRequest::sprayTimeKeeperRequest(qint64 startTime, qint64 endTime, SprayTimeKeeperSchedule& schedule)
{
  this->startTime = startTime;
  this->endTime = endTime;
  this->schedule = &schedule;
  
  this->schedule->addSprayAction(startTime, true);
  this->schedule->addSprayAction(endTime, false);
}

sprayTimeKeeperRequest::~sprayTimeKeeperRequest()
{
  this->schedule->removeSprayAction(startTime);
  this->schedule->removeSprayAction(endTime);
}

