//
// (C) 2013 Opensim Ltd.
//
// This library is free software, you can redistribute it
// and/or modify
// it under  the terms of the GNU Lesser General Public License
// as published by the Free Software Foundation;
// either version 2 of the License, or any later version.
// The library is distributed in the hope that it will be useful,
// but WITHOUT ANY WARRANTY; without even the implied warranty of
// MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.
// See the GNU Lesser General Public License for more details.
//
// Author: Andras Varga (andras@omnetpp.org)
//

#ifndef __INET_LIFECYCLEOPERATIONS_H_
#define __INET_LIFECYCLEOPERATIONS_H_

#include "Lifecycle.h"

class INET_API DownOperation : public LifecycleOperation {};
class INET_API UpOperation : public LifecycleOperation {};
class INET_API IncidentOperation : public LifecycleOperation {};

#endif

