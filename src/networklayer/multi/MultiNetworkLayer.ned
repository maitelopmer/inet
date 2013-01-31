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

package inet.networklayer.multi;

import inet.networklayer.contract.INetworkLayer;
import inet.networklayer.ipv4.IPv4NetworkLayer;
import inet.nodes.ipv6.IPv6NetworkLayer;
import inet.networklayer.generic.GenericNetworkLayer;

simple MultiNetworkLayerUpperMultiplexer
{
    parameters:
        @display("i=block/fork");

    gates:
        input manetUpperIn;
        output manetUpperOut;
        input manetLowerIn[];
        output manetLowerOut[];
        input pingUpperIn[];
        output pingUpperOut[];
        input pingLowerIn[];
        output pingLowerOut[];
}

simple MultiNetworkLayerLowerMultiplexer
{
    parameters:
        @display("i=block/fork");

    gates:
        input ifUpperIn[] @labels(INetworkDatagram);
        output ifUpperOut[] @labels(INetworkDatagram);
        input ifLowerIn[] @labels(INetworkDatagram);
        output ifLowerOut[] @labels(INetworkDatagram);
}

simple NetworkDatagramMultiplexer
{
    parameters:
        @display("i=block/fork");

    gates:
        input upperIn;
        output upperOut;
        input lowerIn[];
        output lowerOut[];
}

//
// TODO
//
module MultiNetworkLayer like INetworkLayer
{
    parameters:
        @display("i=block/fork");
        bool enableIPv4 = default(true);
        bool enableIPv6 = default(true);
        bool enableGeneric = default(true);
        ipv4NetworkLayer.*.routingTableModuleName = default("ipv4RoutingTable");
        ipv6NetworkLayer.*.routingTableModuleName = default("ipv6RoutingTable");
        genericNetworkLayer.*.routingTableModuleName = default("genericRoutingTable");

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
        // TODO: simplify names to ipv4, ipv6, generic once looking up modules support paths
        ipv4NetworkLayer: IPv4NetworkLayer if enableIPv4 {
            @display("p=100,300");
        }
        ipv6NetworkLayer: IPv6NetworkLayer if enableIPv6 {
            @display("p=300,300");
        }
        genericNetworkLayer: GenericNetworkLayer if enableGeneric {
            @display("p=500,300");
        }
        upperMultiplexer: MultiNetworkLayerUpperMultiplexer {
            @display("p=300,100");
        }
        lowerMultiplexer: MultiNetworkLayerLowerMultiplexer {
            @display("p=300,500");
        }

    connections allowunconnected:
        // manet
        manetIn --> upperMultiplexer.manetUpperIn;
        upperMultiplexer.manetUpperOut --> manetOut;
        ipv4NetworkLayer.manetOut --> upperMultiplexer.manetLowerIn++ if enableIPv4;
        upperMultiplexer.manetLowerOut++ --> ipv4NetworkLayer.manetIn if enableIPv4;
        ipv6NetworkLayer.manetOut --> upperMultiplexer.manetLowerIn++ if enableIPv6;
        upperMultiplexer.manetLowerOut++ --> ipv6NetworkLayer.manetIn if enableIPv6;
        genericNetworkLayer.manetOut --> upperMultiplexer.manetLowerIn++ if enableGeneric;
        upperMultiplexer.manetLowerOut++ --> genericNetworkLayer.manetIn if enableGeneric;

        // ipv4
        for i=0..sizeof(pingIn)-1 {
            upperMultiplexer.pingLowerOut++ --> ipv4NetworkLayer.pingIn++ if enableIPv4;
        }
        for i=0..sizeof(pingOut)-1 {
            ipv4NetworkLayer.pingOut++ --> upperMultiplexer.pingLowerIn++ if enableIPv4;
        }
        // ipv6
        for i=0..sizeof(pingIn)-1 {
            upperMultiplexer.pingLowerOut++ --> ipv6NetworkLayer.pingIn++ if enableIPv6;
        }
        for i=0..sizeof(pingOut)-1 {
            ipv6NetworkLayer.pingOut++ --> upperMultiplexer.pingLowerIn++ if enableIPv6;
        }
        // generic
        for i=0..sizeof(pingIn)-1 {
            upperMultiplexer.pingLowerOut++ --> genericNetworkLayer.pingIn++ if enableGeneric;
        }
        for i=0..sizeof(pingOut)-1 {
            genericNetworkLayer.pingOut++ --> upperMultiplexer.pingLowerIn++ if enableGeneric;
        }

		// xxx
        for i=0..sizeof(pingIn)-1 {
            pingIn[i] --> { @display("m=n"); } --> upperMultiplexer.pingUpperIn++;
        }
        for i=0..sizeof(pingOut)-1 {
            upperMultiplexer.pingUpperOut++ --> { @display("m=n"); } --> pingOut[i];
        }

        // ipv4
        for i=0..sizeof(ifIn)-1 {
            lowerMultiplexer.ifUpperOut++ --> ipv4NetworkLayer.ifIn++ if enableIPv4;
        }
        for i=0..sizeof(ifOut)-1 {
            ipv4NetworkLayer.ifOut++ --> lowerMultiplexer.ifUpperIn++ if enableIPv4;
        }
        // ipv6
        for i=0..sizeof(ifIn)-1 {
            lowerMultiplexer.ifUpperOut++ --> ipv6NetworkLayer.ifIn++ if enableIPv6;
        }
        for i=0..sizeof(ifOut)-1 {
            ipv6NetworkLayer.ifOut++ --> lowerMultiplexer.ifUpperIn++ if enableIPv6;
        }
        // generic
        for i=0..sizeof(ifIn)-1 {
            lowerMultiplexer.ifUpperOut++ --> genericNetworkLayer.ifIn++ if enableGeneric;
        }
        for i=0..sizeof(ifOut)-1 {
            genericNetworkLayer.ifOut++ --> lowerMultiplexer.ifUpperIn++ if enableGeneric;
        }

		// yyy
        for i=0..sizeof(ifIn)-1 {
            ifIn[i] --> { @display("m=s"); } --> lowerMultiplexer.ifLowerIn++;
        }
        for i=0..sizeof(ifOut)-1 {
            lowerMultiplexer.ifLowerOut++ --> { @display("m=s"); } --> ifOut[i];
        }
}
