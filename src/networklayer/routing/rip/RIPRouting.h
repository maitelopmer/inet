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

#ifndef __INET_RIPROUTING_H_
#define __INET_RIPROUTING_H_

#include "INETDefs.h"
#include "INotifiable.h"
#include "IRoute.h"
#include "IRoutingTable.h"
#include "IInterfaceTable.h"
#include "UDPSocket.h"

#define RIP_INFINITE_METRIC 16

struct RIPRoute : public cObject
{
    enum RouteType {
      RIP_ROUTE_RTE,            // route learned from a RIPEntry
      RIP_ROUTE_STATIC,         // static route
      RIP_ROUTE_DEFAULT,        // default route
      RIP_ROUTE_REDISTRIBUTE,   // route imported from another routing protocol
      RIP_ROUTE_INTERFACE       // route belongs to a local interface
    };

    IRoute *route;         // the route in the host routing table that is associated with this route, may be NULL
    RouteType type;        // the type of the route
    InterfaceEntry *ie;    // only for INTERFACE routes
    Address from;          // only for RTE routes
    int metric;            // the metric of this route, or infinite (16) if invalid
    uint16 tag;            // route tag, only for REDISTRIBUTE routes
    bool changed;          // true if the route has changed since the update
    simtime_t lastUpdateTime; // time of the last change

    RIPRoute(IRoute *route, RouteType type, int metric)
        : route(route), type(type), ie(NULL), metric(metric), tag(0), changed(false), lastUpdateTime(0) {}
    virtual std::string info() const;
};

/**
 * Enumerated parameter to control how the RIPRouting module
 * advertises the routes to its neighbors.
 */
enum SplitHorizonMode
{
    NO_SPLIT_HORIZON,     // every route is sent to the neighbor
    SPLIT_HORIZON,        // do not send routes to the neighbor it was learnt from
    SPLIT_HORIZON_POISONED_REVERSE // send the route to the neighbor it was learnt from with infinite metric (16)
};

/**
 * Holds the RIP configuration of the interfaces.
 * We could store this data in the InterfaceEntry* itself,
 * but it contains only 5 holes for protocol data, and they
 * are used by network layer protocols only. Therefore
 * RIPRouting manages its own table of these entries.
 */
struct RIPInterfaceEntry
{
    const InterfaceEntry *ie;           // the associated interface entry
    int metric;                         // metric of this interface
    SplitHorizonMode splitHorizonMode;  // RIP mode of this interface

    RIPInterfaceEntry(const InterfaceEntry *ie);
    void configure(cXMLElement *config);
};

/**
 * Implementation of the Routing Information Protocol v2 (RFC 2453).
 *
 * Outside a subnetted network only the network routes are advertised (merging subnet routes) (RFC 2453 3.7)
 *
 * Default routes (with 0.0.0.0 address) are added to BGP routers and are propagated by RIP.
 * Routes involving 0.0.0.0 should not leave the boundary of an AS. (RFC 2453 3.7)
 *
 */
class INET_API RIPRouting : public cSimpleModule, protected INotifiable
{
    typedef std::vector<RIPInterfaceEntry> InterfaceVector;
    typedef std::vector<RIPRoute*> RouteVector;
    // environment
    cModule *host;                  // the host module that owns this module
    IInterfaceTable *ift;           // interface table of the host
    IRoutingTable *rt;              // routing table from which routes are imported and to which learned routes are added
    IAddressPolicy *addressPolicy;  // address type of the routing table
    // state
    InterfaceVector ripInterfaces;  // interfaces on which RIP is used
    RouteVector ripRoutes;          // all advertised routes (imported or learned)
    UDPSocket socket;               // bound to the RIP port (see udpPort parameter)
    cMessage *updateTimer;          // for sending unsolicited Response messages in every ~30 seconds.
    cMessage *triggeredUpdateTimer; // scheduled when there are pending changes
    // parameters
    int ripUdpPort;                 // UDP port RIP routers (usually 520)
    simtime_t updateInterval;       // time between regular updates
    simtime_t routeExpiryTime;      // learned routes becomes invalid if no update received in this period of time
    simtime_t routePurgeTime;       // invalid routes are deleted after this period of time is elapsed
    // signals
    static simsignal_t sentRequestSignal;
    static simsignal_t sentUpdateSignal;
    static simsignal_t rcvdResponseSignal;
    static simsignal_t badResponseSignal;
    static simsignal_t numRoutesSignal;
  public:
    RIPRouting();
    ~RIPRouting();
  private:
    RIPInterfaceEntry *findInterfaceById(int interfaceId);
    RIPRoute *findRoute(const Address &destAddress, int prefixLength);
    RIPRoute *findRoute(const Address &destination, int prefixLength, RIPRoute::RouteType type);
    RIPRoute *findRoute(const InterfaceEntry *ie, RIPRoute::RouteType type);
    void addInterface(const InterfaceEntry *ie, cXMLElement *config);
    void deleteInterface(const InterfaceEntry *ie);
    void invalidateRoutes(const InterfaceEntry *ie);
    bool isLoopbackInterfaceRoute(const IRoute *route);
    bool isLocalInterfaceRoute(const IRoute *route);
    bool isDefaultRoute(const IRoute *route) { return route->getPrefixLength() == 0; }
    std::string getHostName() {return host->getFullName(); }
  protected:
    virtual int numInitStages() const  {return 5;}
    virtual void initialize(int stage);
    virtual void handleMessage(cMessage *msg);
    virtual void receiveChangeNotification(int category, const cObject *details);

    virtual void configureInterfaces(cXMLElement *config);
    virtual void configureInitialRoutes();
    virtual RIPRoute* importRoute(IRoute *route, RIPRoute::RouteType type, int metric = 1);
    virtual void sendInitialRequests();

    virtual void processRequest(RIPPacket *packet);
    virtual void processUpdate(bool triggered);
    virtual void sendRoutes(const Address &address, int port, const RIPInterfaceEntry &ripInterface, bool changedOnly);

    virtual void processResponse(RIPPacket *packet);
    virtual bool isValidResponse(RIPPacket *packet);
    virtual void addRoute(const Address &dest, int prefixLength, const InterfaceEntry *ie, const Address &nextHop, int metric, const Address &from);
    virtual void updateRoute(RIPRoute *route, const InterfaceEntry *ie, const Address &nextHop, int metric, const Address &from);

    virtual void triggerUpdate();
    virtual RIPRoute *checkRouteIsExpired(RIPRoute *route);
    virtual void invalidateRoute(RIPRoute *route);
    virtual void purgeRoute(RIPRoute *route);

    virtual void sendPacket(RIPPacket *packet, const Address &destAddr, int destPort, const InterfaceEntry *destInterface);
};

#endif
