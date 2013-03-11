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

#ifndef __INET_TESTOPERATION_H_
#define __INET_TESTOPERATION_H_

#include "INETDefs.h"
#include "NodeStatus.h"

class INET_API TestTurnNodeOnOperation : public TurnNodeOnOperation {
  public:
    virtual int getNumStages() const { return 4; }
};

class INET_API TestTurnNodeOffOperation : public TurnNodeOffOperation {
  public:
    virtual int getNumStages() const { return 4; }
};

#endif
