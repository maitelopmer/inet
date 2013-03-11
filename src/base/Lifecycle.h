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

#ifndef __INET_LIFECYCLE_H_
#define __INET_LIFECYCLE_H_

#include <INETDefs.h>

/**
 * Base class for operations used by the ILifecycle interface. Subclasses
 * represent "operations" like shutdown, suspend, failure, restart, etc.
 */
class INET_API LifecycleOperation : public cObject, public noncopyable
{
    public:
        virtual int getNumStages() const {return 1;}  //TODO = 0;

};

/**
 * Callback object used by the ILifecycle interface.
 */
class INET_API IDoneCallback
{
    public:
        virtual ~IDoneCallback() {}
        virtual void invoke() = 0;
};

/**
 * Interface to be implemented by modules that want to support fail/recover,
 * suspend/resume, shutdown/restart, and similar scenarios.
 */
class INET_API ILifecycle
{
    public:
        virtual ~ILifecycle() {}

        /**
         * Return: true = "done"; false = "not yet done, will invoke doneCallback when done"
         */
        virtual bool initiateStateChange(LifecycleOperation *operation, int stage, IDoneCallback *doneCallback) = 0;
};

#endif

