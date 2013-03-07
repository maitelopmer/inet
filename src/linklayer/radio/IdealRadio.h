//
// Copyright (C) 2013 OpenSim Ltd
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
// author: Zoltan Bojthe
//


#ifndef __INET_IDEALRADIO_H
#define __INET_IDEALRADIO_H


#include "INETDefs.h"

#include "IdealAirFrame_m.h"
#include "IdealChannelModelAccess.h"
#include "IPowerControl.h"
#include "RadioState.h"


/**
 * This module implements a fullduplex, collision-free and interference-free radio.
 *
 * You should define transmit range of radio. All other radios can receive my sent
 * frame if distance <= transmitRange. All other radios hear nothing if distance
 * larger than transmitRange.
 * The radio can receive all overlapped incoming frames (no collision).
 * The radio also can receive incoming frames during transmission.
 * The radio can be transmit only one frame at same time.
 */
class INET_API IdealRadio : public IdealChannelModelAccess, public IPowerControl
{
  public:
    IdealRadio();
    virtual ~IdealRadio();

    /** Returns the current transmission range */
    virtual int getTransmissionRange() const { return transmissionRange; }

  protected:
    virtual void initialize(int stage);
    virtual void finish();

    virtual void handleMessage(cMessage *msg);

    virtual void handleUpperMsg(cMessage *msg);

    virtual void handleSelfMsg(cMessage *msg);

    virtual void handleCommand(cMessage *msg);

    virtual void handleLowerMsgStart(IdealAirFrame *airframe);

    virtual void handleLowerMsgEnd(IdealAirFrame *airframe);

    /** Sends a message to the upper layer */
    virtual void sendUp(IdealAirFrame *airframe);

    /** Sends a message to the channel */
    virtual void sendDown(IdealAirFrame *airframe);

    /** Encapsulates a MAC frame into an Air Frame */
    virtual IdealAirFrame *encapsulatePacket(cPacket *msg);

    /** Sets the radio state, and also fires change notification */
    virtual void setRadioState();

    /** Returns the current channel the radio is tuned to */
    virtual int getChannelNumber() const { return 0; }

    /** Create a new IdealAirFrame */
    virtual IdealAirFrame *createAirFrame() { return new IdealAirFrame(); }

    virtual void updateDisplayString();

    virtual void receiveSignal(cComponent *source, simsignal_t signalID, cObject *obj);

    // IPowerControl:
    virtual void enablingInitialization();
    virtual void disablingInitialization();

  protected:
    cMessage *updateDisplayStringTimer;

    typedef std::list<cMessage *> RecvBuff;
    RecvBuff recvBuff;

    /** Parameters */
    //@{
    double transmissionRange;           // [meter]
    double bitrate;                     // [bps]
    simtime_t refreshCoverageInterval;  // [s]
    bool drawCoverage;                  // if true, draw coverage circles
    //@}

    /** @name Gate Ids */
    //@{
    int upperLayerOutGateId;
    int upperLayerInGateId;
    int radioInGateId;
    //@}

    /** Radio State and helper variables */
    //@{
    int numReceives;    // number of current receives
    bool onTransmit;    // is in transmit mode
    RadioState::State rs;
    //@}

    // signals:
    static simsignal_t radioStateSignal; // signaling RadioState::State enum when state changed
};

#endif

