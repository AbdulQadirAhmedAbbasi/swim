/*******************************************************************************
 * Simulator of Web Infrastructure and Management
 * Copyright (c) 2016 Carnegie Mellon University.
 * All Rights Reserved.
 *  
 * THIS SOFTWARE IS PROVIDED "AS IS," WITH NO WARRANTIES WHATSOEVER. CARNEGIE
 * MELLON UNIVERSITY EXPRESSLY DISCLAIMS TO THE FULLEST EXTENT PERMITTED BY LAW
 * ALL EXPRESS, IMPLIED, AND STATUTORY WARRANTIES, INCLUDING, WITHOUT
 * LIMITATION, THE WARRANTIES OF MERCHANTABILITY, FITNESS FOR A PARTICULAR
 * PURPOSE, AND NON-INFRINGEMENT OF PROPRIETARY RIGHTS.
 *  
 * Released under a BSD license, please see license.txt for full terms.
 * DM-0003883
 *******************************************************************************/

#include "ReactiveAdaptationManager4.h"
#include "managers/adaptation/UtilityScorer.h"
#include "managers/execution/AllTactics.h"

using namespace std;

Define_Module(ReactiveAdaptationManager4);

/**
 * Reactive adaptation
 *
 * RT = response time
 * RTT = response time threshold
 *
 * - if RT > RTT, add a server if possible, if not decrease dimmer if possible
 * - if RT < RTT and spare utilization > 1
 *      -if dimmer < 1, increase dimmer else if servers > 1 and no server booting remove server
 */
Tactic* ReactiveAdaptationManager4::evaluate() {
    MacroTactic* pMacroTactic = new MacroTactic;
    Model* pModel = getModel();
    const double dimmerStep = 1.0 / (pModel->getNumberOfDimmerLevels() - 1);
    double dimmer = pModel->getDimmerFactor();
    double spareUtilization =  pModel->getConfiguration().getActiveServers() - pModel->getObservations().utilization;
    bool isServerBooting = pModel->getServers() > pModel->getActiveServers();
    double powerConsumption = pModel->getConfiguration().getPeakPowerConsumption() * (pModel->getObservations().utilization/ 100);
    double PC_THRESHOLD = pModel->getConfiguration().getPeakPowerConsumption() * 70/100;
    double MIN_PC_THRESHOLD = pModel->getConfiguration().getPeakPowerConsumption() * 30/100;
    if ( powerConsumption > PC_THRESHOLD) {
        if (!isServerBooting
                && pModel->getServers() < pModel->getMaxServers()) {
            pMacroTactic->addTactic(new AddServerTactic);
        } else if (dimmer > 0.0) {
            dimmer = max(0.0, dimmer - dimmerStep);
            pMacroTactic->addTactic(new SetDimmerTactic(dimmer));
        }
    } else if (powerConsumption < MIN_PC_THRESHOLD) { // can we increase dimmer or remove servers?
            if (dimmer < 1.0) {
                dimmer = min(1.0, dimmer + dimmerStep);
                pMacroTactic->addTactic(new SetDimmerTactic(dimmer));
            } else if (!isServerBooting
                    && pModel->getServers() > 1) {
                pMacroTactic->addTactic(new RemoveServerTactic);
            }   
    }

    return pMacroTactic;
}
