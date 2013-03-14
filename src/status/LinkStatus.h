//
// Copyright (C) 2013 Opensim Ltd.
//
// This program is free software; you can redistribute it and/or
// modify it under the terms of the GNU Lesser General Public License
// as published by the Free Software Foundation; either version 2
// of the License, or (at your option) any later version.
//
// This program is distributed in the hope that it will be useful,
// but WITHOUT ANY WARRANTY; without even the implied warranty of
// MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
// GNU Lesser General Public License for more details.
//
// You should have received a copy of the GNU Lesser General Public License
// along with this program; if not, see <http://www.gnu.org/licenses/>.
//
// Author: Levente Meszaros (levy@omnetpp.org)
//

#ifndef __INET_LINKSTATUS_H_
#define __INET_LINKSTATUS_H_

#include "INETDefs.h"
#include "IStatus.h"
#include "Lifecycle.h"
#include "LifecycleOperations.h"
#include "LinkStatus_sm.h"

class INET_API PlugInLinkOperation : public UpOperation {
  public:
    enum STAGE {
      STAGE_LOCAL,
      STAGE_COUNT
    };

  public:
    virtual int getNumStages() const { return STAGE_COUNT; }
};

class INET_API UnplugLinkOperation : public DownOperation {
  public:
    enum STAGE {
      STAGE_LOCAL,
      STAGE_COUNT
    };

  public:
    virtual int getNumStages() const { return STAGE_COUNT; }
};

class INET_API LinkStatusData : public LinkStatusContext, public IStatus
{
  protected:
    cChannel * link;  // the network link belonging to this status of NULL if there's no such link
    cOutVector statusVector;

  public:
    LinkStatusData();

    static LinkStatusDataState & getStatusWithDefault(LinkStatusData * linkStatusData, LinkStatusDataState & defaultValue = LinkStatusMap::PluggedIn) {
        return linkStatusData ? linkStatusData->getStatus() : defaultValue;
    }

    LinkStatusDataState & getStatus() const { return getState(); }
    void setStatus(LinkStatusDataState & status);

    virtual const char * getStatusByName() const;
    virtual void setStatusByName(const char * name);

  protected:
    virtual void statusChanged();
    virtual const char * getStatusColor();
    virtual void updateDisplayString(const char * color);
};

class INET_API LinkStatus : public cDatarateChannel, public LinkStatusData, public ILifecycle
{
  public:
    static simsignal_t linkStatusChangedSignal; // the signal used to notify listeners about status changes

  public:
    virtual bool initiateStateChange(LifecycleOperation *operation, int stage, IDoneCallback *doneCallback);

  protected:
    virtual void initialize(int stage);
    virtual void processMessage(cMessage * message, simtime_t t, result_t& result);

    virtual void statusChanged();
    virtual void signalStatus();
};

#endif
