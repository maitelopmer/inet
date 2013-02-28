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


cplusplus {{
#include "INETDefs.h"

#include "Coord.h"
}}


class noncobject Coord;

//
// Format of the messages that are sent to the channel ~IdealChannelModel
//
// This message format is used to send a packet from a radio module
// to the channel ~IdealChannelModel.
//
// If you need more fields for whatever reason, please do NOT create
// your own message! Just extend (subclass) this message format (see Omnet
// manual,chapter Messages - Inheritance among message classes).
//
packet IdealAirFrame
{
    simtime_t transmissionDuration;     // Time it takes to transmit the packet, in seconds
    Coord transmissionStartPosition;    // position of sender when transmission starts

/////////////////////////
    double transmissionRange;           // max. distance from sender where reception is possible
}

