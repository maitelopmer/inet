//
// Copyright (C) 2004 Andras Varga
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

package inet.networklayer.generic;

import inet.networklayer.arp.GlobalARP;
import inet.networklayer.contract.INetworkLayer;


//
// TODO
//
module GenericNetworkLayer like INetworkLayer
{
    parameters:
        @display("i=block/fork");
    gates:
        input ifIn[] @labels(INetworkDatagram);
        input tcpIn @labels(TCPSegment,INetworkControlInfo/down);
        input udpIn @labels(UDPPacket,INetworkControlInfo/down);
        input sctpIn @labels(INetworkControlInfo/down,SCTPPacket);
        input rsvpIn @labels(INetworkControlInfo/down);
        input ospfIn @labels(INetworkControlInfo/down);
        input pingIn[];
        input manetIn;
        input igmpIn;
        output ifOut[] @labels(INetworkDatagram);
        output tcpOut @labels(TCPSegment,INetworkControlInfo/up);
        output udpOut @labels(UDPPacket,INetworkControlInfo/up);
        output sctpOut @labels(INetworkControlInfo/up,SCTPPacket);
        output rsvpOut @labels(INetworkControlInfo/up);
        output ospfOut @labels(INetworkControlInfo/up);
        output pingOut[];
        output manetOut;
        output igmpOut;

    submodules:
        gnp: GenericNetworkProtocol {
            parameters:
                protocolMapping = "6:0,17:1,1:2,2:3,46:4,89:5,132:6,48:7,138:7"; // see: http://www.iana.org/assignments/protocol-numbers
                @display("p=85,95;q=queue");
            gates:
                transportIn[8];
                transportOut[8];
                queueIn[sizeof(ifIn)];
        }
        echo: EchoProtocol {
            parameters:
                @display("p=160,63");
        }
        arp: GlobalARP {
            parameters:
                @display("p=202,142;q=pendingQueue");
            gates:
                nicOut[sizeof(ifOut)];
        }
    connections allowunconnected:
        // transport Layer
        gnp.transportOut[0] --> { @display("m=n"); } --> tcpOut;
        gnp.transportIn[0] <-- { @display("m=n"); } <-- tcpIn;

        gnp.transportOut[1] --> { @display("m=n"); } --> udpOut;
        gnp.transportIn[1] <-- { @display("m=n"); } <-- udpIn;

        gnp.transportOut[2] --> echo.localIn;
        gnp.transportIn[2] <-- echo.sendOut;

        gnp.transportOut[4] --> { @display("m=n"); } --> rsvpOut;
        gnp.transportIn[4] <-- { @display("m=n"); } <-- rsvpIn;

        gnp.transportOut[5] --> { @display("m=n"); } --> ospfOut;
        gnp.transportIn[5] <-- { @display("m=n"); } <-- ospfIn;

        gnp.transportOut[6] --> { @display("m=n"); } --> sctpOut;
        gnp.transportIn[6] <-- { @display("m=n"); } <-- sctpIn;

        gnp.transportOut[7] --> { @display("m=n"); } --> manetOut;
        gnp.transportIn[7] <-- { @display("m=n"); } <-- manetIn;

        for i=0..sizeof(pingOut)-1 {
            echo.pingOut++ --> { @display("m=n"); } --> pingOut[i];
        }
        for i=0..sizeof(pingIn)-1 {
            echo.pingIn++ <-- { @display("m=n"); } <-- pingIn[i];
        }

        gnp.queueOut --> arp.ipIn;

        for i=0..sizeof(ifIn)-1 {
            ifIn[i] --> { @display("m=s"); } --> gnp.queueIn[i];
        }
        for i=0..sizeof(ifOut)-1 {
            arp.nicOut[i] --> { @display("m=s"); } --> ifOut[i];
        }
}
