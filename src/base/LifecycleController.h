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

#ifndef __INET_LIFECYCLECONTROLLER_H_
#define __INET_LIFECYCLECONTROLLER_H_

#include <INETDefs.h>
#include "Lifecycle.h"
#include "IScriptable.h"

/**
 * Manages operations like node, interface, daemon, etc failure/recovery
 * suspend/resume, shutdown/restart, and similar operations.
 *
 * TODO describe what it does and how it works: scope (module tree), stages,
 * callback, etc.
 *
 * This module will typically have one instance in the whole network.
 */
class INET_API LifecycleController : public cSimpleModule, public IScriptable
{
    protected:
        struct Process {
                cModule *module; // the operation's root module
                LifecycleOperation *operation;
                int stage; // current stage
                std::vector<IDoneCallback*> pendingList;
        };
        std::vector<Process*> processes;

        class INET_API Callback : public IDoneCallback
        {
            public:
                LifecycleController *controller;
                Process *process;
                cModule *module;

                void init(LifecycleController *controller, Process *process, cModule *module);
                virtual void invoke();
        };

        Callback *spareCallback;

    protected:
        virtual bool resumeStateChange(Process *process);
        virtual void doOneStage(Process *process, cModule *submodule);
        virtual void stateChangeCompleted(Callback *callback);  // invoked from the callback
    public:
        LifecycleController() : spareCallback(NULL) {}
        ~LifecycleController() { delete spareCallback; }
        virtual void initialize();
        virtual void handleMessage(cMessage *msg);
        virtual void processCommand(const cXMLElement& node);
        virtual bool initiateStateChange(cModule *module, LifecycleOperation *operation); //TODO retval: Process*?
};

#endif

