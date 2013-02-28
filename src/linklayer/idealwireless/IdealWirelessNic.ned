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


package inet.linklayer.idealwireless;

import inet.base.IHook;
import inet.linklayer.IOutputQueue;
import inet.linklayer.ITrafficConditioner;
import inet.linklayer.IWirelessNic;
import inet.linklayer.radio.IdealRadio;


//
// Ideal Wireless interface. Complements the ~IdealWirelessMac and ~IdealRadio
// modules with an output queue for QoS and RED support.
// Use it with ~IdealChannelModel (as channelControl)
//
module IdealWirelessNic like IWirelessNic
{
    parameters:
        @display("i=block/ifcard;bgb=214,335;bgl=53");
        double bitrate @unit("bps");
        string queueType = default("DropTailQueue");    // DropTailQueue, a Diffserv queue, etc.
    gates:
        input upperLayerIn;
        output upperLayerOut;
        input radioIn @labels(IdealAirFrame);
    submodules:
        queue: <queueType> like IOutputQueue {
            parameters:
                @display("p=23,125;q=l2queue");
        }
        mac: IdealWirelessMac {
            parameters:
                bitrate = bitrate;
                @display("p=98,207");
        }
        radio: IdealRadio {
            parameters:
                bitrate = bitrate;
                @display("p=98,278");
        }
    connections:
        upperLayerIn --> { @display("m=n"); } --> queue.in;
        mac.upperLayerOut --> { @display("m=n"); } --> upperLayerOut;
        queue.out --> mac.upperLayerIn;
        radioIn --> { @display("m=s"); } --> radio.radioIn;
        radio.upperLayerIn <-- mac.lowerLayerOut;
        radio.upperLayerOut --> mac.lowerLayerIn;
}

