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


package inet.linklayer.radio;

//
// This is an ideal radio module for fullduplex wireless communication.
//
// All radio receives packets inside the transmissionRange of sender,
// All other radio hear nothing outside the transmissionRange of sender.
// Radio receives overlapped incoming frames (no collision).
// Radio also receives incoming frames when transmit a frame.
// Radio can send only one frame at same time.
//
// See ~IdealChannelModel, ~IdealWirelessMac, ~IdealWirelessNic
//
simple IdealRadio like IRadio
{
    parameters:
        double bitrate @unit("bps");            // transmission speed [bit/sec]
        double transmissionRange @unit("m");    // max. distance from sender where reception is possible
        bool drawCoverage = default(true);      // draw the coverage area in Tkenv
        int refreshCoverageInterval @unit("s") = default(0s);  // update period in simulation time for coverage redraw, 0 means never update
    gates:
        input upperLayerIn; // from higher layer protocol (MAC)
        output upperLayerOut; // to higher layer protocol (MAC)
        input radioIn @labels(IdealAirFrame); // to receive frames (IdealAirFrame) on the radio channel
}

