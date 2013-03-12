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

#include <algorithm>
#include "LifecycleController.h"
#include "LifecycleOperations.h"

Define_Module(LifecycleController);


void LifecycleController::Callback::init(LifecycleController *controller, Process *process, cModule *module)
{
    this->controller = controller;
    this->process = process;
    this->module = module;
}

void LifecycleController::Callback::invoke()
{
    if (!controller)
        throw cRuntimeError("Usage error: callback may only be invoked if (and after) you returned 'false' from initiateStateChange()");
    controller->stateChangeCompleted(this);
}

//----

template<typename T>
void vector_delete_element(std::vector<T*>& v, T* p)
{
    typename std::vector<T*>::iterator it = std::find(v.begin(), v.end(), p);
    ASSERT(it != v.end());
    v.erase(it);
    delete p;
}

void LifecycleController::initialize()
{
}

void LifecycleController::handleMessage(cMessage *msg)
{
    throw cRuntimeError("This module does not process messages");
}

void LifecycleController::processCommand(const cXMLElement& node)
{
    //FIXME temporary code -- need something more sophisticated here
    const char *target = node.getAttribute("target");
    cModule *module = getModuleByPath(target);
    if (!module)
        throw cRuntimeError("Module '%s' not found", target);

    const char *operationName = node.getAttribute("operation");
    LifecycleOperation *operation = check_and_cast<LifecycleOperation *>(createOne(operationName));
    initiateStateChange(module, operation);
}

bool LifecycleController::initiateStateChange(cModule *module, LifecycleOperation *operation)
{
    Process *process = new Process();
    process->module = module;
    process->operation = operation;
    process->stage = 0;
    processes.push_back(process);
    return resumeStateChange(process);
}

bool LifecycleController::resumeStateChange(Process *process)
{
    int numStages = process->operation->getNumStages();
    while (process->stage < numStages)
    {
        doOneStage(process, process->module);
        if (process->pendingList.empty())
            process->stage++;
        else
            return false; // pending
    }

    delete process->operation;
    vector_delete_element(processes, process);
    return true;  // done
}

void LifecycleController::doOneStage(Process *process, cModule *submodule)
{
    ILifecycle *restartable = dynamic_cast<ILifecycle*>(submodule);
    if (restartable)
    {
        Callback *callback = spareCallback ? spareCallback : new Callback();
        bool done = restartable->initiateStateChange(process->operation, process->stage, callback);  //FIXME change retval to "need more stages" ???
        if (!done)
        {
            callback->init(this, process, submodule);
            process->pendingList.push_back(callback);
            spareCallback = NULL;
        }
        else
            spareCallback = callback;
    }

    for (cModule::SubmoduleIterator i(submodule); !i.end(); i++)
    {
         cModule *child = i();
         doOneStage(process, child);
    }
}

void LifecycleController::stateChangeCompleted(Callback *callback)
{
    Process *process = callback->process;
    vector_delete_element(process->pendingList, (IDoneCallback*)callback);

    if (process->pendingList.empty())
    {
        process->stage++;
        resumeStateChange(process);
    }
}
