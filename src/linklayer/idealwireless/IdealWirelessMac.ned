//
// Copyright (C) 2013 OpenSim Ltd.
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

package inet.linklayer.idealwireless;

// Ideal Wireless MAC module for ~IdealRadio, ~IdealWirelessNic, and ~IdealChannelModel.
// 
simple IdealWirelessMac
{
    parameters:
        string address = default("auto");   // MAC address as hex string (12 hex digits), or
                                            // "auto". "auto" values will be replaced by
                                            // a generated MAC address in init stage 0.
        double bitrate @unit("bps");
        int mtu @unit("B") = default(4470B);
        int headerLength @unit("B");        // IdealWirelessFrame.byteLength = headerLength + packetFromUpperLayer.byteLength
        bool promiscuous = default(false);
        @display("i=block/rxtx");

    gates:
        input upperLayerIn @labels(Ieee802Ctrl);        // should connect to output gate of an IPassiveQueue module
        output upperLayerOut @labels(Ieee802Ctrl);
        input lowerLayerIn @labels(IdealWirelessFrame);
        output lowerLayerOut @labels(IdealWirelessFrame);
}

