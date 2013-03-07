//
// Copyright (C) 2000 Institut fuer Telematik, Universitaet Karlsruhe
// Copyright (C) 2004-2005 Andras Varga
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


#include "IPvXTrafGen.h"

#include "AddressResolver.h"
#include "IPSocket.h"
#include "IPv4ControlInfo.h"
#include "IPv6ControlInfo.h"


Define_Module(IPvXTrafGen);

int IPvXTrafGen::counter;
simsignal_t IPvXTrafGen::sentPkSignal = SIMSIGNAL_NULL;

void IPvXTrafGen::initialize(int stage)
{
    // because of AddressResolver, we need to wait until interfaces are registered,
    // address auto-assignment takes place etc.
    if (stage != 3)
        return;

    IPvXTrafSink::initialize();
    sentPkSignal = registerSignal("sentPk");

    protocol = par("protocol");
    numPackets = par("numPackets");
    simtime_t startTime = par("startTime");
    stopTime = par("stopTime");
    if (stopTime != 0 && stopTime <= startTime)
        error("Invalid startTime/stopTime parameters");

    const char *destAddrs = par("destAddresses");
    cStringTokenizer tokenizer(destAddrs);
    const char *token;
    while ((token = tokenizer.nextToken()) != NULL)
        destAddresses.push_back(AddressResolver().resolve(token));

    packetLengthPar = &par("packetLength");

    counter = 0;

    numSent = 0;
    WATCH(numSent);

    IPSocket ipSocket(gate("ipOut"));
    ipSocket.registerProtocol(protocol);

    if (destAddresses.empty())
        return;

    if (numPackets > 0)
    {
        cMessage *timer = new cMessage("sendTimer");
        scheduleAt(startTime, timer);
    }
}

Address IPvXTrafGen::chooseDestAddr()
{
    int k = intrand(destAddresses.size());
    return destAddresses[k];
}

void IPvXTrafGen::sendPacket()
{
    char msgName[32];
    sprintf(msgName, "appData-%d", counter++);

    cPacket *payload = new cPacket(msgName);
    payload->setByteLength(packetLengthPar->longValue());

    Address destAddr = chooseDestAddr();

    if (destAddr.getType() == Address::IPv4)
    {
        // send to IPv4
        IPv4ControlInfo *controlInfo = new IPv4ControlInfo();
        controlInfo->setDestAddr(destAddr.toIPv4());
        controlInfo->setProtocol(protocol);
        payload->setControlInfo(controlInfo);
    }
    else if (destAddr.getType() == Address::IPv6)
    {
        // send to IPv6
        IPv6ControlInfo *controlInfo = new IPv6ControlInfo();
        controlInfo->setDestAddr(destAddr.toIPv6());
        controlInfo->setProtocol(protocol);
        payload->setControlInfo(controlInfo);
    }
    else
        throw cRuntimeError("Unknown address type");
    EV << "Sending packet: ";
    printPacket(payload);
    emit(sentPkSignal, payload);
    send(payload, "ipOut");
    numSent++;
}

void IPvXTrafGen::handleMessage(cMessage *msg)
{
    if (msg->isSelfMessage())
    {
        // send, then reschedule next sending
        sendPacket();

        simtime_t d = simTime() + par("sendInterval").doubleValue();
        if ((!numPackets || numSent<numPackets) && (stopTime == 0 || stopTime > d))
            scheduleAt(d, msg);
        else
            delete msg;
    }
    else
    {
        // process incoming packet
        processPacket(PK(msg));
    }

    if (ev.isGUI())
    {
        char buf[40];
        sprintf(buf, "rcvd: %d pks\nsent: %d pks", numReceived, numSent);
        getDisplayString().setTagArg("t", 0, buf);
    }
}

