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

#ifndef __INET_INTERFACESTATUS_H_
#define __INET_INTERFACESTATUS_H_

#include "INETDefs.h"
#include "IStatus.h"
#include "Lifecycle.h"
#include "LifecycleOperations.h"
#include "InterfaceStatus_sm.h"

class INET_API BringInterfaceUpOperation : public UpOperation {
  public:
    virtual int getNumStages() const { return 1; }
};

class INET_API BringInterfaceDownOperation : public DownOperation {
  public:
    virtual int getNumStages() const { return 1; }
};

class INET_API InterfaceStatusData : public InterfaceStatusContext, public IStatus
{
  protected:
    cModule * interface; // the network interface belonging to this status of NULL if there's no such interface

  public:
    InterfaceStatusData() : InterfaceStatusContext(*this), interface(NULL) { }

    static InterfaceStatusDataState & getStatusWithDefault(InterfaceStatusData * interfaceStatusData, InterfaceStatusDataState & defaultValue = InterfaceStatusMap::Up) {
        return interfaceStatusData ? interfaceStatusData->getStatus() : defaultValue;
    }

    InterfaceStatusDataState & getStatus() const { return getState(); }
    void setStatus(InterfaceStatusDataState & status);

    virtual const char * getStatusByName() const;
    virtual void setStatusByName(const char * name);

  protected:
    virtual void statusChanged();
    virtual const char * getStatusIcon();
    virtual void updateDisplayString(const char * icon);
};

class INET_API InterfaceStatus : public cSimpleModule, public InterfaceStatusData, public ILifecycle
{
  public:
    static simsignal_t interfaceStatusChangedSignal; // the signal used to notify listeners about status changes

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
