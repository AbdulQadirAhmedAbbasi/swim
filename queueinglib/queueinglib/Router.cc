//
// This file is part of an OMNeT++/OMNEST simulation example.
//
// Copyright (C) 2006-2015 OpenSim Ltd.
//
// This file is distributed WITHOUT ANY WARRANTY. See the file
// `license' for details on this and other legal matters.
//

#include "Router.h"

namespace queueing {

Define_Module(Router);

void Router::initialize()
{
    const char *algName = par("routingAlgorithm");
    if (strcmp(algName, "random") == 0) {
        routingAlgorithm = ALG_RANDOM;
    }
    else if (strcmp(algName, "roundRobin") == 0) {
        routingAlgorithm = ALG_ROUND_ROBIN;
    }
    else if (strcmp(algName, "weightedRoundRobin") == 0) {
        routingAlgorithm = ALG_WEIGHTED_ROUND_ROBIN; ///////// Added by Abdul
    }
    else if (strcmp(algName, "minQueueLength") == 0) {
        routingAlgorithm = ALG_MIN_QUEUE_LENGTH;
    }
    else if (strcmp(algName, "minDelay") == 0) {
        routingAlgorithm = ALG_MIN_DELAY;
    }
    else if (strcmp(algName, "minServiceTime") == 0) {
        routingAlgorithm = ALG_MIN_SERVICE_TIME;
    }
    rrCounter = -1;

    //Added by Abdul
    maxWeight = 3;         //maximum weight that can be assigned to any server
    minWeight = 1;         //minimum weight that can be assigned to any server
    requestCounter = 0;    //currently assigned total requests to the server according to weight
    wrrCounter = 0;        //current server Index 
}

void Router::handleMessage(cMessage *msg)
{
    int outGateIndex = -1;  // by default we drop the message

    switch (routingAlgorithm) {
        case ALG_RANDOM:
            outGateIndex = par("randomGateIndex");
            break;

        case ALG_ROUND_ROBIN:
            rrCounter = (rrCounter + 1) % gateSize("out");
            outGateIndex = rrCounter;
            break;
        
        case ALG_WEIGHTED_ROUND_ROBIN:
            wrrCounter = wrrCounter %  gateSize("out");
            outGateIndex = wrrCounter;
            
            requestCounter = requestCounter + 1;
            if(requestCounter == serverWeights[outGateIndex])
            {
                wrrCounter = wrrCounter + 1;
                requestCounter = 0;
            }
            break;

        case ALG_MIN_QUEUE_LENGTH:
            // TODO implementation missing
            outGateIndex = -1;
            break;

        case ALG_MIN_DELAY:
            // TODO implementation missing
            outGateIndex = -1;
            break;

        case ALG_MIN_SERVICE_TIME:
            // TODO implementation missing
            outGateIndex = -1;
            break;

        default:
            outGateIndex = -1;
            break;
    }

    // send out if the index is legal
    if (outGateIndex < 0 || outGateIndex >= gateSize("out"))
        throw cRuntimeError("Invalid output gate selected during routing");

    send(msg, "out", outGateIndex);
}

}; //namespace

