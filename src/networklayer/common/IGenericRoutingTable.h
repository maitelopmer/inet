//
// Copyright (C) 2012 Andras Varga
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

#ifndef __INET_IGENERICROUTINGTABLE_H
#define __INET_IGENERICROUTINGTABLE_H

#include "INETDefs.h"
#include "Address.h"
#include "IGenericRoute.h"


/**
 * A C++ interface to abstract the functionality of a routing table, regardless of address type.
 */
//TODO the "Generic" can be dropped from the name, once RoutingTable[6] is renamed to IPv[4|6]RoutingTable
class INET_API IGenericRoutingTable
{
    public:
        virtual ~IGenericRoutingTable() {};

        /** @name Miscellaneous functions */
        //@{
        /**
         * Forwarding on/off
         */
        virtual bool isForwardingEnabled() const = 0;  //XXX IP modulba?

        /**
         * Multicast forwarding on/off
         */
        virtual bool isMulticastForwardingEnabled() const = 0; //XXX IP modulba?

        /**
         * Returns routerId.
         */
        virtual Address getRouterId() const = 0;

        /**
         * Checks if the address is a local one, i.e. one of the host's.
         */
        virtual bool isLocalAddress(const Address& dest) const = 0;  //XXX maybe into InterfaceTable?

        /**
         * Checks if the address is a local network broadcast address, i.e. one of the
         * broadcast addresses derived from the interface addresses and netmasks.
         */
        virtual bool isLocalBroadcastAddress(const Address& dest) const = 0; //XXX maybe into InterfaceTable?

        /**
         * Returns an interface given by its address. Returns NULL if not found.
         */
        virtual InterfaceEntry *getInterfaceByAddress(const Address& address) const = 0;   //XXX should be find..., see next one

        /**
         * Returns the interface entry having the specified address
         * as its local broadcast address.
         */
        virtual InterfaceEntry *findInterfaceByLocalBroadcastAddress(const Address& dest) const = 0;
        //@}

        /** @name Routing functions (query the route table) */
        //@{
        /**
         * The routing function. Performs longest prefix match for the given
         * destination address, and returns the resulting route. Returns NULL
         * if there is no matching route.
         */
        virtual IGenericRoute *findBestMatchingRoute(const Address& dest) const = 0;

        /**
         * Convenience function based on findBestMatchingRoute().
         *
         * Returns the output interface for the packets with dest as destination
         * address, or NULL if the destination is not in routing table.
         */
        virtual InterfaceEntry *getOutputInterfaceForDestination(const Address& dest) const = 0;  //XXX redundant

        /**
         * Convenience function based on findBestMatchingRoute().
         *
         * Returns the gateway for the destination address. Returns the unspecified
         * address if the destination is not in routing table or the gateway field
         * is not filled in in the route.
         */
        virtual Address getNextHopForDestination(const Address& dest) const = 0; //XXX redundant AND unused
        //@}

        /** @name Multicast routing functions */
        //@{

        /**
         * Checks if the address is in one of the local multicast group
         * address list.
         */
        virtual bool isLocalMulticastAddress(const Address& dest) const = 0;

        /**
         * Returns route for a multicast origin and group.
         */
        virtual IGenericMulticastRoute *findBestMatchingMulticastRoute(const Address &origin, const Address& group) const = 0;
        //@}

        /** @name Route table manipulation */
        //@{

        /**
         * Returns the total number of unicast routes.
         */
        virtual int getNumRoutes() const = 0;

        /**
         * Returns the kth route.
         */
        virtual IGenericRoute *getRoute(int k) const = 0;

        /**
         * Finds and returns the default route, or NULL if it doesn't exist
         */
        virtual IGenericRoute *getDefaultRoute() const = 0;  //XXX is this a universal concept?

        /**
         * Adds a route to the routing table. Routes are allowed to be modified
         * while in the routing table. (There is a notification mechanism that
         * allows routing table internals to be updated on a routing entry change.)
         */
        virtual void addRoute(IGenericRoute *entry) = 0;

        /**
         * Removes the given route from the routing table, and returns it.
         * NULL is returned if the route was not in the routing table.
         */
        virtual IGenericRoute *removeRoute(IGenericRoute *entry) = 0;

        /**
         * Deletes the given route from the routing table.
         * Returns true if the route was deleted, and false if it was
         * not in the routing table.
         */
        virtual bool deleteRoute(IGenericRoute *entry) = 0;

        /**
         * Returns the total number of multicast routes.
         */
        virtual int getNumMulticastRoutes() const = 0;

        /**
         * Returns the kth multicast route.
         */
        virtual IGenericMulticastRoute *getMulticastRoute(int k) const = 0;

        /**
         * Adds a multicast route to the routing table. Routes are allowed to be modified
         * while in the routing table. (There is a notification mechanism that
         * allows routing table internals to be updated on a routing entry change.)
         */
        virtual void addMulticastRoute(IGenericMulticastRoute *entry) = 0;

        /**
         * Removes the given route from the routing table, and returns it.
         * NULL is returned of the route was not in the routing table.
         */
        virtual IGenericMulticastRoute *removeMulticastRoute(IGenericMulticastRoute *entry) = 0;

        /**
         * Deletes the given multicast route from the routing table.
         * Returns true if the route was deleted, and false if it was
         * not in the routing table.
         */
        virtual bool deleteMulticastRoute(IGenericMulticastRoute *entry) = 0;

        /**
         * Deletes invalid routes from the routing table. Invalid routes are those
         * where the isValid() method returns false.
         */
        virtual void purgeExpiredRoutes() = 0;
        //@}

        virtual IGenericRoute *createRoute() = 0;
};

#endif

