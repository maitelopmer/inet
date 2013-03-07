//
// Copyright (C) 2012 Opensim Ltd.
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

#ifndef __GENERICNETWORKPROTOCOLINTERFACEDATA_H
#define __GENERICNETWORKPROTOCOLINTERFACEDATA_H

#include <vector>

#include "INETDefs.h"

#include "NotificationBoard.h"
#include "InterfaceEntry.h"
#include "Address.h"

/**
 * Generic network protocol specific data for an InterfaceEntry, stores generic network address.
 *
 * @see InterfaceEntry
 */
class INET_API GenericNetworkProtocolInterfaceData : public InterfaceProtocolData
{
  protected:
    Address inetAddr;  ///< address of interface
    int metric;        ///< link "cost"; see e.g. MS KB article Q299540  //TODO needed???

    NotificationBoard *nb; // cached pointer

  protected:
    void changed1() {changed(NF_INTERFACE_GENERICNETWORKPROTOCOLCONFIG_CHANGED);}

  private:
    // copying not supported: following are private and also left undefined
    GenericNetworkProtocolInterfaceData(const GenericNetworkProtocolInterfaceData& obj);
    GenericNetworkProtocolInterfaceData& operator=(const GenericNetworkProtocolInterfaceData& obj);

  public:
    GenericNetworkProtocolInterfaceData();
    virtual ~GenericNetworkProtocolInterfaceData();
    virtual std::string info() const;
    virtual std::string detailedInfo() const;

    /** @name Getters */
    //@{
    Address getAddress() const {return inetAddr;}
    int getMetric() const  {return metric;}
    //@}

    /** @name Setters */
    //@{
    virtual void setAddress(Address a) {inetAddr = a; changed1();}
    virtual void setMetric(int m) {metric = m; changed1();}
    //@}

    virtual void joinMulticastGroup(Address address) { }
};

#endif

