//
// Copyright (C) 2012 Andras Varga
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

#include "NodeStatus.h"
#include "ModuleAccess.h"
#include "LifecycleOperations.h"

Register_Class(TurnNodeOnOperation);
Register_Class(TurnNodeOffOperation);
Define_Module(NodeStatus)

simsignal_t NodeStatus::nodeStatusChangedSignal = SIMSIGNAL_NULL;

void NodeStatusData::setStatus(NodeStatusDataState & status)
{
    setState(status);
    statusChanged();
}

const char * NodeStatusData::getStatusByName() const
{
    NodeStatusDataState & state = getState();
    if (state == NodeStatusMap::On)
        return "on";
    else if (state == NodeStatusMap::Off)
        return "off";
    else if (state == NodeStatusMap::TurningOn)
        return "turning on";
    else if (state == NodeStatusMap::TurningOff)
        return "turning off";
    else
        throw cRuntimeError("Unknown status");
}

void NodeStatusData::setStatusByName(const char * name)
{
    if (!strcmp(name, "on"))
        setStatus(NodeStatusMap::On);
    else if (!strcmp(name, "off"))
        setStatus(NodeStatusMap::Off);
    else if (!strcmp(name, "turning on"))
        setStatus(NodeStatusMap::TurningOn);
    else if (!strcmp(name, "turning off"))
        setStatus(NodeStatusMap::TurningOff);
    else
        throw cRuntimeError("Unknown status");
}

void NodeStatusData::statusChanged()
{
    updateDisplayString(getStatusIcon());
}

const char * NodeStatusData::getStatusIcon()
{
    NodeStatusDataState & state = getState();
    if (state == NodeStatusMap::On)
        return "status/check";
    else if (state == NodeStatusMap::Off)
        return "status/cross";
    else if (state == NodeStatusMap::TurningOn)
        return "status/execute";
    else if (state == NodeStatusMap::TurningOff)
        return "status/execute";
    else
        throw cRuntimeError("Unknown status");
}

void NodeStatusData::updateDisplayString(const char * icon)
{
    if (node)
        node->getDisplayString().setTagArg("i2", 0, icon);
}

void NodeStatus::initialize(int stage)
{
    if (stage == 0) {
        node = par("updateNode") ? findContainingNode(this) : NULL;
        nodeStatusChangedSignal = registerSignal("nodeStatusChanged");
        setStatusByName(par("initialStatus"));
    }
}

void NodeStatus::statusChanged()
{
    NodeStatusData::statusChanged();
    signalStatus();
}

void NodeStatus::signalStatus()
{
    emit(nodeStatusChangedSignal, this);
}

void NodeStatus::updateDisplayString(const char * icon)
{
    NodeStatusData::updateDisplayString(icon);
    getDisplayString().setTagArg("i", 0, icon);
}

bool NodeStatus::initiateStateChange(LifecycleOperation *operation, int stage, IDoneCallback *doneCallback)
{
    if (dynamic_cast<TurnNodeOnOperation *>(operation)) {
        if (stage == 0) {
            StartTurningOn();
            EV << node->getFullPath() << " turning on" << endl;
        }
        // NOTE: this is not an 'else if' so that it works if there's only 1 stage
        if (stage == operation->getNumStages() - 1) {
            EndTurningOn();
            EV << node->getFullPath() << " turned on" << endl;
        }
        statusChanged();
    }
    else if (dynamic_cast<TurnNodeOffOperation *>(operation)) {
        if (stage == 0) {
            StartTurningOff();
            EV << node->getFullPath() << " turning off" << endl;
        }
        // NOTE: this is not an 'else if' so that it works if there's only 1 stage
        if (stage == operation->getNumStages() - 1) {
            EndTurningOff();
            EV << node->getFullPath() << " turned off" << endl;
        }
        statusChanged();
    }
    return true;
}
