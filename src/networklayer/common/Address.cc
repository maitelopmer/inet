//
// Copyright (C) 2005 Andras Varga
//
// This program is free software; you can redistribute it and/or
// modify it under the terms of the GNU Lesser General Public
// License as published by the Free Software Foundation; either
// version 2.1 of the License, or (at your option) any later version.
//
// This program is distributed in the hope that it will be useful,
// but WITHOUT ANY WARRANTY; without even the implied warranty of
// MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
// GNU Lesser General Public License for more details.
//
// You should have received a copy of the GNU Lesser General Public
// License along with this program; if not, see <http://www.gnu.org/licenses/>.
//

#include "Address.h"
#include "IPv4AddressPolicy.h"
#include "IPv6AddressPolicy.h"
#include "MACAddressPolicy.h"
#include "ModuleIdAddressPolicy.h"
#include "ModulePathAddressPolicy.h"

#define RESERVED_IPV6_ADDRESS_RANGE 0x8000

uint64 Address::get(AddressType type) const
{
    if (getType() == type)
        return raw.getLo();
    else
        throw cRuntimeError("Address is not of the given type");
}

void Address::set(AddressType type, uint64 lo)
{
    raw = Uint128(lo, ((uint64)RESERVED_IPV6_ADDRESS_RANGE << 48) + (uint64)type);
}

Address::AddressType Address::getType() const
{
    uint64 hi = raw.getHi();
    if (hi >> 48 == RESERVED_IPV6_ADDRESS_RANGE)
        return (AddressType)(hi & 0xFF);
    else
        return Address::IPv6;
}

IAddressPolicy * Address::getAddressPolicy() const
{
    switch (getType()) {
        case Address::NONE:
            throw cRuntimeError("Address contains no value");
        case Address::IPv4:
            return &IPv4AddressPolicy::INSTANCE;
        case Address::IPv6:
            return &IPv6AddressPolicy::INSTANCE;
        case Address::MAC:
            return &MACAddressPolicy::INSTANCE;
        case Address::MODULEID:
            return &ModuleIdAddressPolicy::INSTANCE;
        case Address::MODULEPATH:
            return &ModulePathAddressPolicy::INSTANCE;
        default:
            throw cRuntimeError("Unknown type");
    }
}

std::string Address::str() const
{
    switch (getType()) {
        case Address::NONE:
            return "<none>";
        case Address::IPv4:
            return toIPv4().str();
        case Address::IPv6:
            return toIPv6().str();
        case Address::MAC:
            return toMAC().str();
        case Address::MODULEID:
            return toModuleId().str();
        case Address::MODULEPATH:
            return toModulePath().str();
        default:
            throw cRuntimeError("Unknown type");
    }
}

bool Address::tryParse(const char *addr)
{
    IPv6Address ipv6;
    MACAddress mac;
    ModuleIdAddress moduleId;
    ModulePathAddress modulePath;
    if (IPv4Address::isWellFormed(addr))
        set(IPv4Address(addr));
    else if (ipv6.tryParse(addr))
        set(ipv6);
    else if (mac.tryParse(addr))
        set(mac);
    else if (moduleId.tryParse(addr))
        set(moduleId);
    else if (modulePath.tryParse(addr))
        set(modulePath);
    else
        return false;
    return true;
}

bool Address::isUnspecified() const
{
    switch (getType()) {
        case Address::NONE:
            return true;
        case Address::IPv4:
            return toIPv4().isUnspecified();
        case Address::IPv6:
            return toIPv6().isUnspecified();
        case Address::MAC:
            return toMAC().isUnspecified();
        case Address::MODULEID:
            return toModuleId().isUnspecified();
        case Address::MODULEPATH:
            return toModulePath().isUnspecified();
        default:
            throw cRuntimeError("Unknown type");
    }
}

bool Address::isUnicast() const
{
    switch (getType()) {
        case Address::NONE:
            throw cRuntimeError("Address contains no value");
        case Address::IPv4:
            return !toIPv4().isMulticast() && !toIPv4().isLimitedBroadcastAddress();  // TODO: move to IPv4Address
        case Address::IPv6:
            return toIPv6().isUnicast();
        case Address::MAC:
            return !toMAC().isBroadcast() && !toMAC().isMulticast(); // TODO: move to MACAddress
        case Address::MODULEID:
            return toModuleId().isUnicast();
        case Address::MODULEPATH:
            return toModulePath().isUnicast();
        default:
            throw cRuntimeError("Unknown type");
    }
}

bool Address::isMulticast() const
{
    switch (getType()) {
        case Address::NONE:
            throw cRuntimeError("Address contains no value");
        case Address::IPv4:
            return toIPv4().isMulticast();
        case Address::IPv6:
            return toIPv6().isMulticast();
        case Address::MAC:
            return toMAC().isMulticast();
        case Address::MODULEID:
            return toModuleId().isMulticast();
        case Address::MODULEPATH:
            return toModulePath().isMulticast();
        default:
            throw cRuntimeError("Unknown type");
    }
}

bool Address::isBroadcast() const
{
    switch (getType()) {
        case Address::NONE:
            throw cRuntimeError("Address contains no value");
        case Address::IPv4:
            return toIPv4().isLimitedBroadcastAddress();
        case Address::IPv6:
            return false;
            //throw cRuntimeError("IPv6 isBroadcast() unimplemented");
        case Address::MAC:
            return toMAC().isBroadcast();
        case Address::MODULEID:
            return toModuleId().isBroadcast();
        case Address::MODULEPATH:
            return toModulePath().isBroadcast();
        default:
            throw cRuntimeError("Unknown type");
    }
}

bool Address::operator<(const Address& other) const
{
    AddressType type = getType();
    AddressType otherType = other.getType();
    if (type != otherType)
        return type < otherType;
    else {
        switch (type) {
            case Address::NONE:
                throw cRuntimeError("Address contains no value");
            case Address::IPv4:
                return toIPv4() < other.toIPv4();
            case Address::IPv6:
                return toIPv6() < other.toIPv6();
            case Address::MAC:
                return toMAC() < other.toMAC();
            case Address::MODULEID:
                return toModuleId() < other.toModuleId();
            case Address::MODULEPATH:
                return toModulePath() < other.toModulePath();
            default:
                throw cRuntimeError("Unknown type");
        }
    }
}

bool Address::operator==(const Address& other) const
{
    AddressType type = getType();
    if (type != other.getType())
        return false;
    else {
        switch (type) {
            case Address::NONE:
                return true;
            case Address::IPv4:
                return toIPv4() == other.toIPv4();
            case Address::IPv6:
                return toIPv6() == other.toIPv6();
            case Address::MAC:
                return toMAC() == other.toMAC();
            case Address::MODULEID:
                return toModuleId() == other.toModuleId();
            case Address::MODULEPATH:
                return toModulePath() == other.toModulePath();
            default:
                throw cRuntimeError("Unknown type");
        }
    }
}

bool Address::operator!=(const Address& other) const
{
    return !operator==(other);
}

bool Address::matches(const Address& other, int prefixLength) const
{
    switch (getType()) {
        case Address::NONE:
            throw cRuntimeError("Address contains no value");
        case Address::IPv4:
            return IPv4Address::maskedAddrAreEqual(toIPv4(), other.toIPv4(), IPv4Address::makeNetmask(prefixLength)); //FIXME !!!!!
        case Address::IPv6:
            return toIPv6().matches(other.toIPv6(), prefixLength);
        case Address::MAC:
            return toMAC() == other.toMAC();
        case Address::MODULEID:
            return toModuleId() == other.toModuleId();
        case Address::MODULEPATH:
            return ModulePathAddress::maskedAddrAreEqual(toModulePath(), other.toModulePath(), prefixLength);
        default:
            throw cRuntimeError("Unknown type");
    }
}
