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

#include "InterfaceStatus.h"
#include "LifecycleOperations.h"

Register_Class(BringInterfaceUpOperation);
Register_Class(BringInterfaceDownOperation);
Define_Module(InterfaceStatus)

simsignal_t InterfaceStatus::interfaceStatusChangedSignal = SIMSIGNAL_NULL;

void InterfaceStatusData::setStatus(InterfaceStatusDataState & status)
{
    setState(status);
    statusChanged();
}

const char * InterfaceStatusData::getStatusByName() const
{
    InterfaceStatusDataState & state = getState();
    if (state == InterfaceStatusMap::Up)
        return "up";
    else if (state == InterfaceStatusMap::Down)
        return "down";
    else if (state == InterfaceStatusMap::BringingUp)
        return "bringing up";
    else if (state == InterfaceStatusMap::BringingDown)
        return "bringing down";
    else
        throw cRuntimeError("Unknown status");
}

void InterfaceStatusData::setStatusByName(const char * name)
{
    if (!strcmp(name, "up"))
        setStatus(InterfaceStatusMap::Up);
    else if (!strcmp(name, "down"))
        setStatus(InterfaceStatusMap::Down);
    else if (!strcmp(name, "bringing up"))
        setStatus(InterfaceStatusMap::BringingUp);
    else if (!strcmp(name, "bringing down"))
        setStatus(InterfaceStatusMap::BringingDown);
    else
        throw cRuntimeError("Unknown status");
}

void InterfaceStatusData::statusChanged()
{
    updateDisplayString(getStatusIcon());
}

const char * InterfaceStatusData::getStatusIcon()
{
    InterfaceStatusDataState & state = getState();
    if (state == InterfaceStatusMap::Up)
        return "status/up";
    else if (state == InterfaceStatusMap::Down)
        return "status/down";
    else if (state == InterfaceStatusMap::BringingUp)
        return "status/execute";
    else if (state == InterfaceStatusMap::BringingDown)
        return "status/execute";
    else
        throw cRuntimeError("Unknown status");
}

void InterfaceStatusData::updateDisplayString(const char * icon)
{
    if (interface)
        interface->getDisplayString().setTagArg("i2", 0, icon);
}

void InterfaceStatus::initialize(int stage)
{
    if (stage == 0) {
        interface = par("updateInterface") ? getParentModule() : NULL;
        interfaceStatusChangedSignal = registerSignal("interfaceStatusChanged");
        setStatusByName(par("initialStatus"));
    }
}

void InterfaceStatus::statusChanged()
{
    InterfaceStatusData::statusChanged();
    signalStatus();
}

void InterfaceStatus::updateDisplayString(const char * icon)
{
    InterfaceStatusData::updateDisplayString(icon);
    getDisplayString().setTagArg("i", 0, icon);
}

void InterfaceStatus::signalStatus()
{
    emit(interfaceStatusChangedSignal, this);
}

bool InterfaceStatus::initiateStateChange(LifecycleOperation *operation, int stage, IDoneCallback *doneCallback)
{
    if (dynamic_cast<BringInterfaceUpOperation *>(operation)) {
        if (stage == 0) {
            StartBringingUp();
            EV << interface->getFullPath() << " bringing up" << endl;
        }
        // NOTE: this is not an 'else if' so that it works if there's only 1 stage
        if (stage == operation->getNumStages() - 1) {
            EndBringingUp();
            EV << interface->getFullPath() << " brought up" << endl;
        }
        statusChanged();
    }
    else if (dynamic_cast<BringInterfaceDownOperation *>(operation)) {
        if (stage == 0) {
            StartBringingDown();
            EV << interface->getFullPath() << " bringing down" << endl;
        }
        // NOTE: this is not an 'else if' so that it works if there's only 1 stage
        if (stage == operation->getNumStages() - 1) {
            EndBringingDown();
            EV << interface->getFullPath() << " brought down" << endl;
        }
        statusChanged();
    }
    return true;
}
