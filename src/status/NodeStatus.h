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

#ifndef __INET_NODESTATUS_H_
#define __INET_NODESTATUS_H_

#include "INETDefs.h"
#include "IStatus.h"
#include "Lifecycle.h"
#include "LifecycleOperations.h"
#include "NodeStatus_sm.h"

class INET_API TurnNodeOnOperation : public UpOperation {
  public:
    enum STAGE {
      STAGE_LOCAL,
      STAGE_PHYSICAL_LAYER,
      STAGE_LINK_LAYER,
      STAGE_NETWORK_LAYER,
      STAGE_TRANSPORT_LAYER,
      STAGE_APPLICATION_LAYER,
      STAGE_COUNT
    };
  public:
    virtual int getNumStages() const { return STAGE_COUNT; }
};

class INET_API TurnNodeOffOperation : public DownOperation {
  public:
    enum STAGE {
      STAGE_LOCAL,
      STAGE_APPLICATION_LAYER,
      STAGE_TRANSPORT_LAYER,
      STAGE_NETWORK_LAYER,
      STAGE_LINK_LAYER,
      STAGE_PHYSICAL_LAYER,
      STAGE_COUNT
    };

  public:
    virtual int getNumStages() const { return STAGE_COUNT; }
};

class INET_API NodeStatusData : public NodeStatusContext, public IStatus
{
  protected:
    cModule * node; // the network node belonging to this status of NULL if there's no such node

  public:
    NodeStatusData() : NodeStatusContext(*this), node(NULL) { }

    static NodeStatusDataState & getStatusWithDefault(NodeStatusData * nodeStatusData, NodeStatusDataState & defaultValue = NodeStatusMap::On) {
        return nodeStatusData ? nodeStatusData->getStatus() : defaultValue;
    }

    virtual NodeStatusDataState & getStatus() const { return getState(); }
    virtual void setStatus(NodeStatusDataState & status);

    virtual const char * getStatusByName() const;
    virtual void setStatusByName(const char * name);

  protected:
    virtual void statusChanged();
    virtual const char * getStatusIcon();
    virtual void updateDisplayString(const char * icon);
};

class INET_API NodeStatus : public cSimpleModule, public NodeStatusData, public ILifecycle
{
  public:
    static simsignal_t nodeStatusChangedSignal; // the signal used to notify handlers about status changes

  public:
    virtual bool initiateStateChange(LifecycleOperation *operation, int stage, IDoneCallback *doneCallback);

  protected:
    virtual void initialize(int stage);
    virtual void handleMessage(cMessage * message) { throw cRuntimeError("This module doesn't handle messages"); }

    virtual void statusChanged();
    virtual void signalStatus();
    virtual void updateDisplayString(const char * icon);
};

#endif
