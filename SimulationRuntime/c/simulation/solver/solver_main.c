/*
 * This file is part of OpenModelica.
 *
 * Copyright (c) 1998-CurrentYear, Link?ping University,
 * Department of Computer and Information Science,
 * SE-58183 Link?ping, Sweden.
 *
 * All rights reserved.
 *
 * THIS PROGRAM IS PROVIDED UNDER THE TERMS OF THIS OSMC PUBLIC
 * LICENSE (OSMC-PL). ANY USE, REPRODUCTION OR DISTRIBUTION OF
 * THIS PROGRAM CONSTITUTES RECIPIENT'S ACCEPTANCE OF THE OSMC
 * PUBLIC LICENSE.
 *
 * The OpenModelica software and the Open Source Modelica
 * Consortium (OSMC) Public License (OSMC-PL) are obtained
 * from Link?ping University, either from the above address,
 * from the URL: http://www.ida.liu.se/projects/OpenModelica
 * and in the OpenModelica distribution.
 *
 * This program is distributed  WITHOUT ANY WARRANTY; without
 * even the implied warranty of  MERCHANTABILITY or FITNESS
 * FOR A PARTICULAR PURPOSE, EXCEPT AS EXPRESSLY SET FORTH
 * IN THE BY RECIPIENT SELECTED SUBSIDIARY LICENSE CONDITIONS
 * OF OSMC-PL.
 *
 * See the full OSMC Public License conditions for more details.
 *
 */

#include "solver_main.h"
#include "simulation_runtime.h"
#include "initialization.h"
#include "events.h"
#include "dassl.h"
/*
 * #include "dopri45.h"
 */
#include "rtclock.h"
#include "error.h"
#include <math.h>
#include <string.h>
#include <errno.h>


double** work_states;

const int rungekutta_s = 4;
const double rungekutta_b[4] = { 1.0 / 6.0, 1.0 / 3.0, 1.0 / 3.0, 1.0 / 6.0 };
const double rungekutta_c[4] = { 0.0, 0.5, 0.5, 1.0 };


typedef struct RK4
{
  double** work_states;
  int work_states_ndims;
}RK4;


double dlamch_(char*,int);

int
euler_ex_step(_X_DATA* simData, SOLVER_INFO* solverInfo);

int
rungekutta_step(_X_DATA* simData, SOLVER_INFO* solverInfo);


/*! \fn void printAllVars(_X_DATA *data, int )
 *
 *  prints all values as arguments it need data
 *  and which part of the ring should printed.
 *
 *  author: wbraun
 */
void printAllVars(_X_DATA *data, int ringSegment)
{
  long i;
  MODEL_DATA      *mData = &(data->modelData);
  INFO("all real variables");
  for(i=0; i<mData->nVariablesReal; ++i){
    INFO3("localData->realVars[%ld] = %s = %g",i,mData->realVarsData[i].info.name,data->localData[ringSegment]->realVars[i]);
  }
  INFO("all integer variables");
  for(i=0; i<mData->nVariablesInteger; ++i){
    INFO3("localData->integerVars[%ld] = %s = %ld",i,mData->integerVarsData[i].info.name,data->localData[ringSegment]->integerVars[i]);
  }
  INFO("all boolean variables");
  for(i=0; i<mData->nVariablesBoolean; ++i){
    INFO3("localData->booleanVars[%ld] = %s = %s",i,mData->booleanVarsData[i].info.name,data->localData[ringSegment]->booleanVars[i]?"true":"false");
  }
  INFO("all string variables");
  for(i=0; i<mData->nVariablesString; ++i){
    INFO3("localData->stringVars[%ld] = %s = %s",i,mData->stringVarsData[i].info.name,data->localData[ringSegment]->stringVars[i]);
  }
  INFO("all real parameters");
  for(i=0; i<mData->nParametersReal; ++i){
    INFO3("mData->realParameterData[%ld] = %s = %g",i,mData->realParameterData[i].info.name,mData->realParameterData[i].attribute.initial);
  }
  INFO("all integer parameters");
  for(i=0; i<mData->nParametersInteger; ++i){
    INFO3("mData->integerParameterData[%ld] = %s = %ld",i,mData->integerParameterData[i].info.name,mData->integerParameterData[i].attribute.initial);
  }
  INFO("all boolean parameters");
  for(i=0; i<mData->nParametersBoolean; ++i){
    INFO3("mData->booleanParameterData[%ld] = %s = %s",i,mData->booleanParameterData[i].info.name,mData->booleanParameterData[i].attribute.initial?"true":"false");
  }
  INFO("all string parameters");
  for(i=0; i<mData->nParametersString; ++i){
    INFO3("mData->stringParameterData[%ld] = %s = %s",i,mData->stringParameterData[i].info.name,mData->stringParameterData[i].attribute.initial);
  }

}

int
solver_main_step(int flag, _X_DATA* simData, SOLVER_INFO* solverInfo) {
  switch (flag) {
  case 2:
    return rungekutta_step(simData, solverInfo);
  case 3:
    return dasrt_step(simData, solverInfo);
  case 4:
    functionODE_inline(simData, solverInfo->currentStepSize);
    solverInfo->currentTime = simData->localData[0]->timeValue;
    return 0;
/*case 6:
    return stepsize_control(start, stop, fixStep, functionODE, reinit_step,
                            useInterpolation, tolarence, reject);
*/
  default:
  case 1:
    return euler_ex_step(simData, solverInfo);
  }
  return 1;
}

/*	function: update_DAEsystem
 *
 * 	! Function to update the whole system with EventIteration.
 * 	Evaluate the functionDAE()
 */
void update_DAEsystem(_X_DATA *data)
{
  int needToIterate = 0;
  int IterationNum = 0;

  functionDAE(data, &needToIterate);
  while(checkForDiscreteChanges(data) || needToIterate)
  {
    if(needToIterate)
    {
      DEBUG_INFO(LOG_EVENTS, "reinit() call. Iteration needed!");
    }
    else
    {
    	DEBUG_INFO(LOG_EVENTS, "discrete Variable changed. Iteration needed!");
    }
    storePreValues(data);
    functionDAE(data, &needToIterate);
    IterationNum++;
    if(IterationNum > IterationMax)
    {
      THROW("ERROR: Too many event iterations. System is inconsistent!");
    }
  }
}

/* function: copyStartValuestoInitValues
 *
 * ! Function to copy all start values to initial values
 *
 */
void copyStartValuestoInitValues(_X_DATA *data)
{

  /* just copy all start values to initial */
  storeStartValuesParam(data);
  /* bound_parameters(data); */
  storeStartValues(data);
  storePreValues(data);
  overwriteOldSimulationData(data);

}

/* The main function for a solver with synchronous event handling
 * flag 1=explicit euler
 * 2=rungekutta
 * 3=dassl
 * 4=inline
 * 5=free
 * 6=dopri5 with stepsize control & dense output */

int
solver_main(_X_DATA* simData, double start, double stop, double step, long outputSteps, double tolerance, int flag) {

	int i;

	SOLVER_INFO solverInfo;

	SIMULATION_INFO *simInfo = &(simData->simulationInfo);

	SIMULATION_DATA *sData = simData->localData[0];

	double uround = dlamch_((char*) "P", 1);

	int retValIntegrator = 0;

	FILE *fmt = NULL;
	unsigned int stepNo = 0;

	/*
	if (simInfo->numSteps > 0) {
		simInfo->stepSize = (simInfo->stopTime - simInfo->startTime) / simInfo->numSteps;
		DEBUG_INFO1(LOG_SOLVER,"Step size calculated = %g", simInfo->stepSize);
	} else {
		if (simInfo->stepSize == 0) {
			simInfo->stepSize = 2e-3;
			DEBUG_INFO1(LOG_SOLVER,"Set step size on default value = %g", simInfo->stepSize);
		}
		DEBUG_INFO1(LOG_SOLVER,"Step size is set = %g", simInfo->stepSize);
	}
	*/

	/* initial solverInfo */
	solverInfo.currentTime = simInfo->startTime;
	solverInfo.currentStepSize = simInfo->stepSize;
	solverInfo.laststep = 0;
	solverInfo.offset = 0;
	solverInfo.didEventStep = 0;
	solverInfo.sampleEventActivated = 0;
	solverInfo.stateEvents = 0;
	solverInfo.sampleEvents = 0;
	solverInfo.stepNo = 0;
	solverInfo.callsODE = 0;
	solverInfo.callsDAE = 0;

	copyStartValuestoInitValues(simData);
  /* read input vars */
  input_function(simData);
	/* initial sample and delay before initial the system */
	callExternalObjectConstructors(simData);
	initSample(simData, simInfo->startTime, simInfo->stopTime);
	initDelay(simInfo->startTime);

	/* will be removed -> DOPRI45 */
	/* first interpolation point is the value of the fixed external stepsize */
	/* interpolationStep = step;*/
	sData->timeValue = simInfo->startTime;


	if (flag == 2) {
	/* Allocate RK work arrays */
	  RK4 rungeData;
	  rungeData.work_states_ndims = rungekutta_s;
		rungeData.work_states = (double**) malloc((rungeData.work_states_ndims + 1) * sizeof(double*));
		for (i = 0; i < rungeData.work_states_ndims + 1; i++)
			rungeData.work_states[i] = (double*) calloc(simData->modelData.nStates, sizeof(double));
		solverInfo.solverData = &rungeData;
	}else if (flag == 3){
	  /* Initial DASSL solver */
	  DASSL_DATA dasslData;
		DEBUG_INFO(LOG_SOLVER, "*** Initializing DASSL");
		dasrt_initial(simData, &solverInfo, &dasslData);
	  solverInfo.solverData = &dasslData;
	} else if (flag == 4){
	/* Enable inlining solvers */
		work_states = (double**) malloc(inline_work_states_ndims * sizeof(double*));
		for (i = 0; i < inline_work_states_ndims; i++)
			work_states[i] = (double*) calloc(simData->modelData.nVariablesReal, sizeof(double));
	} else {
		/* Allocate DOPRI5(4) derivative array and activate dense output */
		/*
	case 6:
		useInterpolation = 1;
		work_states = (double**) malloc((9 + 1) * sizeof(double*));
		for (i = 0; i < 9 + 1; i++)
			work_states[i] = (double*) calloc(globalData->nStates, sizeof(double));
		break;
		*/
	}

	/* Is done while the initialization!
	if (bound_parameters(simData)) {
		INFO("Error calculating bound parameters");
		return -1;
	}
	DEBUG_INFO(LOG_SOLVER, "Calculated bound parameters");
	*/

	/* Evaluate all constant equations during initialization */
	/* functionAliasEquations(simData); */

	/* Calculate initial values from initial_function()
	 * saveall() value as pre values */
	if (measure_time_flag) {
		rt_accumulate(SIM_TIMER_PREINIT);
		rt_tick(SIM_TIMER_INIT);
	}


	if(initialization(simData, "state", "nelder_mead_ex")) {
			THROW("Error in initialization. Storing results and exiting.");
	}
  /* adrpo: write the parameter data in the file once again after bound parameters and initialization! */
	sim_result_writeParameterData(&(simData->modelData));
  DEBUG_INFO(LOG_SOLVER, "Wrote parameters to the file after initialization (for output formats that support this)");

	/* debug print */
	if (DEBUG_FLAG(LOG_DEBUG)){
	  for (i=0; i<3;i++){
	    INFO1("Print values for buffer segment = %d",i);
	    printAllVars(simData,i);
	  }
	}

	/* initial sample and delay again, due to maybe change
	 * parameters during Initialization */
  callExternalObjectConstructors(simData);
  initSample(simData, simInfo->startTime, simInfo->stopTime);
  initDelay(simInfo->startTime);


	SaveZeroCrossings(simData);
	storePreValues(simData);

	/*
	 * if (sim_verbose >= LOG_SOLVER) {
   *   sim_result_emit(simData);
   * }
	 */

	/* Activate sample and evaluate again */
	if (simData->simulationInfo.curSampleTimeIx < simData->simulationInfo.nSampleTimes) {
		solverInfo.sampleEventActivated = checkForSampleEvent(simData, &solverInfo);
		activateSampleEvents(simData);
	}
	update_DAEsystem(simData);
	if (solverInfo.sampleEventActivated) {
		deactivateSampleEventsandEquations(simData);
		solverInfo.sampleEventActivated = 0;
	}
	storePreValues(simData);
	CheckForNewEvent(simData, 0, &(solverInfo.currentTime));
	SaveZeroCrossings(simData);
	storePreValues(simData);
	sim_result_emit(simData);
	overwriteOldSimulationData(simData);

  /* Initialization complete */
  if (measure_time_flag)
    rt_accumulate( SIM_TIMER_INIT);

  if (simData->localData[0]->timeValue >= simInfo->stopTime) {
    if (sim_verbose >= LOG_SOLVER) {
      INFO("Simulation done!");
    }
    simData->simulationInfo.terminal = 1;
    update_DAEsystem(simData);

    sim_result_emit(simData);

    simData->simulationInfo.terminal = 0;
    return 0;
  }

  DEBUG_INFO(LOG_SOLVER, "Performed initial value calculation.");
  DEBUG_INFO2(LOG_SOLVER, "Start numerical solver from %g to %g", simInfo->startTime, simInfo->stopTime);

  if (measure_time_flag) {
    char* filename = (char*) calloc(((size_t)strlen(simData->modelData.modelFilePrefix)+1+11),sizeof(char));
	  filename = strncpy(filename,simData->modelData.modelFilePrefix,strlen(simData->modelData.modelFilePrefix));
    filename = strncat(filename,"_prof.data",10);
    fmt = fopen(filename, "wb");
    if (!fmt) {
      WARNING2("Warning: Time measurements output file %s could not be opened: %s", filename, strerror(errno));
      fclose(fmt);
      fmt = NULL;
    }
  }

  /*
   * Start main simulation loop
   */
  while (solverInfo.currentTime < simInfo->stopTime) {

    /* rotate RingBuffer before step is calculated */
    rotateRingBuffer(simData->simulationData, 1, (void**) simData->localData);

	  if (measure_time_flag) {
		  for (i = 0; i < simData->modelData.nFunctions + simData->modelData.nProfileBlocks; i++)
			  rt_clear(i + SIM_TIMER_FIRST_FUNCTION);
		  rt_clear(SIM_TIMER_STEP);
		  rt_tick(SIM_TIMER_STEP);
	  }

	  /* Calculate new step size after an event */
	  if (solverInfo.didEventStep == 1) {
		  solverInfo.offset = solverInfo.currentTime - solverInfo.laststep;
		  if (solverInfo.offset + uround > solverInfo.currentStepSize)
			  solverInfo.offset = 0;
		  DEBUG_INFO1(LOG_SOLVER, "Offset value for the next step: %g", solverInfo.offset);
	  } else {
		  solverInfo.offset = 0;
	  }

	  if (flag != 6) {
		  /*!!!!! not for DOPRI5 with stepsize control */
		  solverInfo.currentStepSize = simInfo->stepSize - solverInfo.offset;

		  if (solverInfo.currentTime + solverInfo.currentStepSize > simInfo->stopTime) {
			  solverInfo.currentStepSize = simInfo->stopTime - solverInfo.currentTime;
		  }
	  }

	  if (simData->simulationInfo.curSampleTimeIx < simData->simulationInfo.nSampleTimes) {
		  solverInfo.sampleEventActivated = checkForSampleEvent(simData, &solverInfo);
	  }

	  /* read input vars */
	  input_function(simData);

	  DEBUG_INFO2(LOG_SOLVER, "Call Solver from %g to %g", solverInfo.currentTime,
			  solverInfo.currentTime + solverInfo.currentStepSize);

	  /* do one integration step
	   *
	   * one step means:
	   * determine all states by Integration-Method
	   * update continuous part with
	   * functionODE() and functionAlgebraics(); */
	  communicateStatus("Running", (solverInfo.currentTime-simInfo->startTime)/(simInfo->stopTime-simInfo->startTime));
	  retValIntegrator = solver_main_step(flag, simData, &solverInfo);

    functionAlgebraics(simData);
    functionAliasEquations(simData);
    function_storeDelayed(simData);
    output_function(simData);
    SaveZeroCrossings(simData);
    storePreValues(simData);

	  /* Check for Events */
	  if (measure_time_flag)
		  rt_tick(SIM_TIMER_EVENT);
	  if (CheckForNewEvent(simData, &(solverInfo.sampleEventActivated), &(solverInfo.currentTime))) {
	    DEBUG_INFO(LOG_SOLVER,"###### STATE EVENT DONE ########");
		  solverInfo.stateEvents++;
		  solverInfo.didEventStep = 1;
		  /* due to an event overwrite old values */
		  overwriteOldSimulationData(simData);
	  } else if (solverInfo.sampleEventActivated) {
	    DEBUG_INFO(LOG_SOLVER,"###### TIME EVENT DONE ########");
		  EventHandle(simData,1, NULL);
		  solverInfo.sampleEvents++;
		  solverInfo.didEventStep = 1;
		  solverInfo.sampleEventActivated = 0;
		  /* due to an event overwrite old values */
		  overwriteOldSimulationData(simData);
	  } else {
		  solverInfo.laststep = solverInfo.currentTime;
      solverInfo.didEventStep = 0;
	  }

	  if (measure_time_flag)
		  rt_accumulate(SIM_TIMER_EVENT);

	  /******** Emit this time step ********/
	  storePreValues(simData);
	  /*if (useInterpolation)
		  interpolation_control(dideventstep, interpolationStep, step, stop);
	  */
	  if (fmt) {
		  int flag = 1;
		  double tmpdbl;
		  unsigned int tmpint;
		  rt_tick(SIM_TIMER_OVERHEAD);
		  rt_accumulate(SIM_TIMER_STEP);
		  /* Disable time measurements if we have trouble writing to the file... */
		  flag = flag && 1 == fwrite(&stepNo, sizeof(unsigned int), 1, fmt);
		  stepNo++;
		  flag = flag && 1 == fwrite(&(simData->localData[0]->timeValue), sizeof(double), 1,
				  fmt);
		  tmpdbl = rt_accumulated(SIM_TIMER_STEP);
		  flag = flag && 1 == fwrite(&tmpdbl, sizeof(double), 1, fmt);
		  for (i = 0; i < simData->modelData.nFunctions + simData->modelData.nProfileBlocks; i++) {
			  tmpint = rt_ncall(i + SIM_TIMER_FIRST_FUNCTION);
			  flag = flag && 1 == fwrite(&tmpint, sizeof(unsigned int), 1, fmt);
		  }
		  for (i = 0; i < simData->modelData.nFunctions + simData->modelData.nProfileBlocks; i++) {
			  tmpdbl = rt_accumulated(i + SIM_TIMER_FIRST_FUNCTION);
			  flag = flag && 1 == fwrite(&tmpdbl, sizeof(double), 1, fmt);
		  }
		  rt_accumulate(SIM_TIMER_OVERHEAD);
		  if (!flag) {
			  WARNING1("Disabled time measurements because the output file could not be generated: %s", strerror(errno));
			  fclose(fmt);
			  fmt = NULL;
		  }
	  }

	  SaveZeroCrossings(simData);
	  /*if (!useInterpolation)*/
	  sim_result_emit(simData);
	  /********* end of Emit this time step *********/

	  /* save dassl stats before reset */
	  /*
	  if (reset == 1) {
		  for (i = 0; i < DASSLSTATS; i++)
			  dasslStats[i] += dasslStatsTmp[i];
	  }
	  */
	  /* Check for termination of terminate() or assert() */
	  if (terminationAssert || terminationTerminate || modelErrorCode) {
		  terminationAssert = 0;
		  terminationTerminate = 0;
		  checkForAsserts(simData);
		  checkTermination(simData);
		  if (modelErrorCode)
			  retValIntegrator = 1;
	  }

	  if (retValIntegrator == 1) {
	    simData->simulationInfo.terminal = 1;
		  update_DAEsystem(simData);
		  simData->simulationInfo.terminal = 0;
		  INFO1("model terminate | Simulation terminated at time %g",solverInfo.currentTime);
		  break;
	  }

	  DEBUG_INFO1(LOG_SOLVER, "** Step to  %g Done!", solverInfo.currentTime);
	  /* debug print */
    if (DEBUG_FLAG(LOG_DEBUG)){
	    for (i=0; i<3;i++){
	      INFO1("Print values for buffer segment = %d",i);
	      printAllVars(simData,i);
	    }
	  }
  }


  /* Last step with terminal()=true */
  if (solverInfo.currentTime >= simInfo->stopTime) {
	  simData->simulationInfo.terminal = 1;
	  update_DAEsystem(simData);
	  sim_result_emit(simData);
	  simData->simulationInfo.terminal = 0;
  }
  communicateStatus("Finished", 1);

  /* save dassl stats before print */

  if (DEBUG_FLAG(LOG_STATS)) {
    /*
    int i;
    for (i = 0; i < DASSLSTATS; i++)
      dasslStats[i] += dasslStatsTmp[i];
     */
    rt_accumulate(SIM_TIMER_TOTAL);

    INFO("##### Statistics #####");
    INFO_AL1("simulation time: %g", rt_accumulated(SIM_TIMER_TOTAL));
    INFO_AL1("Events: %d", solverInfo.stateEvents + solverInfo.sampleEvents);
    INFO_AL1("State Events: %d", solverInfo.stateEvents);
    INFO_AL1("Sample Events: %d", solverInfo.sampleEvents);
    /*
    INFO_AL("##### Solver Statistics #####");
    INFO_AL1("The number of steps taken: %d", dasslStats[0]);
    INFO_AL1("The number of calls to functionODE: %d", dasslStats[1]);
    INFO_AL1("The evaluations of Jacobian: %d", dasslStats[2]);
    INFO_AL1("The number of error test failures: %d", dasslStats[3]);
    INFO_AL1("The number of convergence test failures: %d", dasslStats[4]);
    */
    /*
    if (flag == 6)
    {
        INFO1("DOPRI5: total number of steps rejected: %d", reject);
    }
    */
  }


  if (fmt)
    fclose(fmt);

  return 0;
}

/***************************************		EULER_EXP     *********************************/
int
euler_ex_step(_X_DATA* simData, SOLVER_INFO* solverInfo) {
  int i;
  SIMULATION_DATA *sData = (SIMULATION_DATA*)simData->localData[0];
  SIMULATION_DATA *sDataOld = (SIMULATION_DATA*)simData->localData[1];
  modelica_real* stateDer = sDataOld->realVars + simData->modelData.nStates;

  for (i = 0; i < simData->modelData.nStates; i++) {
    sData->realVars[i] = sDataOld->realVars[i] + stateDer[i] * solverInfo->currentStepSize;
  }
  sData->timeValue = sDataOld->timeValue + solverInfo->currentStepSize;
  functionODE(simData);
  solverInfo->currentTime += solverInfo->currentStepSize;
  return 0;
}

/***************************************		RK4  		***********************************/
int
rungekutta_step(_X_DATA* simData, SOLVER_INFO* solverInfo) {
  double** k = ((RK4*)(solverInfo->solverData))->work_states;
  double sum;
  int i,j;

  SIMULATION_DATA *sData = (SIMULATION_DATA*)simData->localData[0];
  SIMULATION_DATA *sDataOld = (SIMULATION_DATA*)simData->localData[1];
  modelica_real* stateDer = sData->realVars + simData->modelData.nStates;
  modelica_real* stateDerOld = sDataOld->realVars + simData->modelData.nStates;


  /* We calculate k[0] before returning from this function.
   * We only want to calculate f() 4 times per call */
  for (i = 0; i < simData->modelData.nStates; i++) {
    k[0][i] = stateDerOld[i];
  }

  for (j = 1; j < rungekutta_s; j++) {
    for (i = 0; i < simData->modelData.nStates; i++) {
      sData->realVars[i] = sDataOld->realVars[i] + solverInfo->currentStepSize * rungekutta_c[j] * k[j - 1][i];
    }
    sData->timeValue = sDataOld->timeValue + rungekutta_c[j] * solverInfo->currentStepSize;
    functionODE(simData);
    for (i = 0; i < simData->modelData.nStates; i++) {
      k[j][i] = stateDer[i];
    }
  }

  for (i = 0; i < simData->modelData.nStates; i++) {
    sum = 0;
    for (j = 0; j < rungekutta_s; j++) {
      sum = sum + rungekutta_b[j] * k[j][i];
    }
    sData->realVars[i] = sDataOld->realVars[i] + solverInfo->currentStepSize * sum;
  }
  sData->timeValue = sDataOld->timeValue + solverInfo->currentStepSize;
  functionODE(simData);
  solverInfo->currentTime += solverInfo->currentStepSize;
  return 0;
}
