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

#include "LinkStatus.h"
#include "LifecycleOperations.h"

Register_Class(PlugInLinkOperation);
Register_Class(UnplugLinkOperation);

simsignal_t LinkStatus::linkStatusChangedSignal = SIMSIGNAL_NULL;

void LinkStatusData::setStatus(LinkStatusDataState & status)
{
    setState(status);
    statusChanged();
}

const char * LinkStatusData::getStatusByName() const
{
    LinkStatusDataState & state = getState();
    if (state == LinkStatusMap::PluggedIn)
        return "plugged in";
    else if (state == LinkStatusMap::Unplugged)
        return "unplugged";
    else if (state == LinkStatusMap::PluggingIn)
        return "plugging in";
    else if (state == LinkStatusMap::Unplugging)
        return "unplugging";
    else
        throw cRuntimeError("Unknown status");
}

void LinkStatusData::setStatusByName(const char * name)
{
    if (!strcmp(name, "plugged in"))
        setStatus(LinkStatusMap::PluggedIn);
    else if (!strcmp(name, "unplugged"))
        setStatus(LinkStatusMap::Unplugged);
    else if (!strcmp(name, "plugging in"))
        setStatus(LinkStatusMap::PluggingIn);
    else if (!strcmp(name, "unplugging"))
        setStatus(LinkStatusMap::Unplugging);
    else
        throw cRuntimeError("Unknown status");
}

void LinkStatusData::statusChanged()
{
    updateDisplayString(getStatusColor());
}

const char * LinkStatusData::getStatusColor()
{
    LinkStatusDataState & state = getState();
    if (state == LinkStatusMap::PluggedIn)
        return "black";
    else if (state == LinkStatusMap::Unplugged)
        return "gray";
    else if (state == LinkStatusMap::PluggingIn)
        return "darkgray";
    else if (state == LinkStatusMap::Unplugging)
        return "darkgray";
    else
        throw cRuntimeError("Unknown status");
}

void LinkStatusData::updateDisplayString(const char * color)
{
    // TODO: having an icon over the connection would be much better
    if (link)
        link->getDisplayString().setTagArg("ls", 0, color);
}

void LinkStatus::initialize(int stage)
{
    if (stage == 0) {
        link = this; // TODO: par("updateLink") ? NULL : NULL;
        linkStatusChangedSignal = registerSignal("linkStatusChanged");
        setStatusByName(par("initialStatus"));
    }
}

void LinkStatus::processMessage(cMessage * message, simtime_t t, result_t& result)
{
    if (getStatus() != LinkStatusMap::PluggedIn)
        throw cRuntimeError("Cannot send message through channel when not plugged in");
    else
        cDatarateChannel::processMessage(message, t, result);
}

void LinkStatus::statusChanged()
{
    LinkStatusData::statusChanged();
    signalStatus();
}

void LinkStatus::signalStatus()
{
    emit(linkStatusChangedSignal, this);
}

bool LinkStatus::initiateStateChange(LifecycleOperation *operation, int stage, IDoneCallback *doneCallback)
{
    if (dynamic_cast<PlugInLinkOperation *>(operation)) {
        if (stage == 0) {
            StartPluggingIn();
            EV << link->getFullPath() << " plugging in" << endl;
        }
        // NOTE: this is not an 'else if' so that it works if there's only 1 stage
        if (stage == operation->getNumStages() - 1) {
            EndPluggingIn();
            EV << link->getFullPath() << " plugged in" << endl;
        }
        statusChanged();
    }
    else if (dynamic_cast<UnplugLinkOperation *>(operation)) {
        if (stage == 0) {
            StartUnplugging();
            EV << link->getFullPath() << " unplugging" << endl;
        }
        // NOTE: this is not an 'else if' so that it works if there's only 1 stage
        if (stage == operation->getNumStages() - 1) {
            EndUnplugging();
            EV << link->getFullPath() << " unplugged" << endl;
        }
        statusChanged();
    }
    return true;
}
