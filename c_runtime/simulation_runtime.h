/*
 * This file is part of OpenModelica.
 *
 * Copyright (c) 1998-CurrentYear, Link�ping University,
 * Department of Computer and Information Science,
 * SE-58183 Link�ping, Sweden.
 *
 * All rights reserved.
 *
 * THIS PROGRAM IS PROVIDED UNDER THE TERMS OF GPL VERSION 3 
 * AND THIS OSMC PUBLIC LICENSE (OSMC-PL). 
 * ANY USE, REPRODUCTION OR DISTRIBUTION OF THIS PROGRAM CONSTITUTES RECIPIENT'S  
 * ACCEPTANCE OF THE OSMC PUBLIC LICENSE.
 *
 * The OpenModelica software and the Open Source Modelica
 * Consortium (OSMC) Public License (OSMC-PL) are obtained
 * from Link�ping University, either from the above address,
 * from the URLs: http://www.ida.liu.se/projects/OpenModelica or  
 * http://www.openmodelica.org, and in the OpenModelica distribution. 
 * GNU version 3 is obtained from: http://www.gnu.org/copyleft/gpl.html.
 *
 * This program is distributed WITHOUT ANY WARRANTY; without
 * even the implied warranty of  MERCHANTABILITY or FITNESS
 * FOR A PARTICULAR PURPOSE, EXCEPT AS EXPRESSLY SET FORTH
 * IN THE BY RECIPIENT SELECTED SUBSIDIARY LICENSE CONDITIONS
 * OF OSMC-PL.
 *
 * See the full OSMC Public License conditions for more details.
 *
 */

/*
 * File: simulation_runtime.h
 *
 * Description: This file is a C++ header file for the simulation runtime.
 * It contains solver functions and other simulation runtime specific functions
 *
 */

#ifndef _SIMULATION_RUNTIME_H
#define _SIMULATION_RUNTIME_H

#include "compat.h"
#include "fortran_types.h"
#include "simulation_varinfo.h"
#include "real_array.h"
#include "integer_array.h"
#include "boolean_array.h"
#include "rtclock.h"
#include <stdlib.h>

#ifdef __cplusplus

#include "linearize.h"
#include "simulation_events.h"
#include "simulation_delay.h"
#include "simulation_result.h"
#include "simulation_inline_solver.h"

#include <fstream>
#include <iostream>
#include <string>

using namespace std;


/* \brief This class is used for throwing an exception when simulation code should be terminated.
 * For instance, when a terminate call occurse or if an assert becomes active
 */

class TerminateSimulationException {
public:
  TerminateSimulationException(const std::string& msg) : currentTime(0.0), errorMessage(msg) {}
  TerminateSimulationException(double time) : currentTime(time), errorMessage("") {}
  TerminateSimulationException(double time, const std::string& msg) : currentTime(time), errorMessage(msg) {}
  TerminateSimulationException() : currentTime(0.0) {}
  virtual ~TerminateSimulationException() {}
  const std::string& getMessage() const { return errorMessage; }
  double getTime() const { return currentTime; }
protected:
  double currentTime;
  std::string errorMessage;
};

extern string TermMsg; /* message for termination. */
extern simulation_result *sim_result;
/* function with template for linear model */
int linear_model_frame(string& out, string A, string B, string C, string D, string x_startvalues, string u_startvalues);
int callSolver(int, char**, string, string, string, double, double, double, long, double);

#endif /* cplusplus */

extern const int measure_time_flag;
extern int sim_verbose; /* control debug output during simulation. */
extern int sim_noemit; /* control emitting result data to file */
extern int acceptedStep; /* !=0 when accepted step is calculated, 0 otherwise. */
extern int modelTermination; /* Becomes non-zero when user terminates simulation. */
extern int terminationTerminate; /* Becomes non-zero when user terminates simulation. */
extern int terminationAssert; /* Becomes non-zero when model call assert simulation. */
extern int warningLevelAssert; /* Becomes non-zero when model call assert with warning level. */
extern omc_fileInfo TermInfo; /* message for termination. */


/* Flags for controlling logging to stdout */
extern const int LOG_EVENTS;
extern const int LOG_NONLIN_SYS;
extern const int LOG_DEBUG;

/* Flags for modelErrorCodes */
extern const int ERROR_NONLINSYS;
extern const int ERROR_LINSYS;

typedef struct sim_DATA_REAL_ALIAS {
  modelica_real* alias;
  modelica_boolean negate;
  int nameID;
} DATA_REAL_ALIAS;

typedef struct sim_DATA_INT_ALIAS {
  modelica_integer* alias;
  modelica_boolean negate;
  int nameID;
} DATA_INT_ALIAS;

typedef struct sim_DATA_BOOL_ALIAS {
  modelica_boolean* alias;
  modelica_boolean negate;
  int nameID;
} DATA_BOOL_ALIAS;

typedef struct sim_DATA_STRING_ALIAS {
  char** alias;
  modelica_boolean negate;
  int nameID;
} DATA_STRING_ALIAS;

typedef struct sim_DATA_STRING {
  const char** algebraics;
  const char** parameters;
  const char** inputVars;
  const char** outputVars;
  DATA_STRING_ALIAS* alias;

  long nAlgebraic,nParameters;
  long nInputVars,nOutputVars;
  long nAlias;
} DATA_STRING;

typedef struct sim_DATA_INT {
  modelica_integer* algebraics;
  modelica_integer* parameters;
  modelica_integer* inputVars;
  modelica_integer* outputVars;
  modelica_integer*  algebraics_old, *algebraics_old2;
  DATA_INT_ALIAS* alias;
  modelica_boolean* algebraicsFilterOutput; /* True if this variable should be filtered */
  modelica_boolean* aliasFilterOutput; /* True if this variable should be filtered */

  long nAlgebraic,nParameters;
  long nInputVars,nOutputVars;
  long nAlias;
} DATA_INT;

typedef struct sim_DATA_BOOL {
  modelica_boolean* algebraics;
  modelica_boolean* parameters;
  modelica_boolean* inputVars;
  modelica_boolean* outputVars;
  modelica_boolean* algebraics_old, *algebraics_old2;
  DATA_BOOL_ALIAS* alias;
  modelica_boolean* algebraicsFilterOutput; /* True if this variable should be filtered */
  modelica_boolean* aliasFilterOutput; /* True if this variable should be filtered */

  long nAlgebraic,nParameters;
  long nInputVars,nOutputVars;
  long nAlias;
} DATA_BOOL;

typedef struct sample_raw_time_st {
  double start;
  double interval;
  int zc_index;
} sample_raw_time;

typedef struct sample_time_st {
  double events;
  int zc_index;
  int activated;
} sample_time;

typedef struct sim_DATA {
  /* this is the data structure for saving important data for this simulation. */
  /* Each generated function have a DATA* parameter wich contain the data. */
  /* A object for the data can be created using */
  /* initializeDataStruc() function*/
  double* states;
  double* statesDerivatives;
  double* algebraics;
  double* parameters;
  double* inputVars;
  double* outputVars;
  double* helpVars;
  double* initialResiduals;
  double* jacobianVars;

  /* True if the variable should be filtered */
  modelica_boolean* statesFilterOutput;
  modelica_boolean* statesDerivativesFilterOutput;
  modelica_boolean* algebraicsFilterOutput;
  modelica_boolean* aliasFilterOutput;

  /* Old values used for extrapolation */
  double* states_old,*states_old2;
  double* statesDerivatives_old,*statesDerivatives_old2;
  double* algebraics_old,*algebraics_old2;
  double oldTime,oldTime2;
  double current_stepsize;

  /* Backup derivative for dassl */
  double* statesDerivativesBackup;
  double* statesBackup;

  char* initFixed; /* Fixed attribute for all variables and parameters */
  int init; /* =1 during initialization, 0 otherwise. */
  void** extObjs; /* External objects */
  /* nStatesDerivatives == states */
  fortran_integer nStates,nAlgebraic,nParameters;
  long nInputVars,nOutputVars,nFunctions,nEquations,nProfileBlocks;
  fortran_integer nZeroCrossing/*NG*/;
  long nJacobianvars;
  long nRelations/*NREL*/;
  long nInitialResiduals/*NR*/;
  long nHelpVars/* NHELP */;
  /* extern char init_fixed[]; */
  DATA_STRING stringVariables;
  DATA_INT intVariables;
  DATA_BOOL boolVariables;

  DATA_REAL_ALIAS* realAlias;
  long nAlias;

  const char* modelName; /* For error messages */
  const char* modelFilePrefix; /* For filenames, input/output */
  const struct omc_varInfo* statesNames;
  const struct omc_varInfo* stateDerivativesNames;
  const struct omc_varInfo* algebraicsNames;
  const struct omc_varInfo* parametersNames;
  const struct omc_varInfo* alias_names;
  const struct omc_varInfo* int_alg_names;
  const struct omc_varInfo* int_param_names;
  const struct omc_varInfo* int_alias_names;
  const struct omc_varInfo* bool_alg_names;
  const struct omc_varInfo* bool_param_names;
  const struct omc_varInfo* bool_alias_names;
  const struct omc_varInfo* string_alg_names;
  const struct omc_varInfo* string_param_names;
  const struct omc_varInfo* string_alias_names;
  const struct omc_varInfo* inputNames;
  const struct omc_varInfo* outputNames;
  const struct omc_varInfo* jacobian_names;
  const struct omc_functionInfo* functionNames;
  const struct omc_equationInfo* equationInfo;
  const int* equationInfo_reverse_prof_index;

  double startTime; /* the start time of the simulation */
  double timeValue; /* the time for the simulation */
  /* used in some generated function */
  /* this is not changed by initializeDataStruc */
  double lastEmittedTime; /* The last time value that has been emitted. */
  int forceEmit; /* when != 0 force emit, set e.g. by newTime for equidistant output signal. */

  /* An array containing the initial data of samples used in the sim */
  sample_raw_time* rawSampleExps;
  long nRawSamples;
  /* The queue of sample time events to be processed. */
  sample_time* sampleTimes; /* Warning: Not implemented yet!? */
  long curSampleTimeIx;
  long nSampleTimes;
} DATA;

/* Global data */
extern DATA *globalData;


extern int modelErrorCode;

extern double *gout;
extern double *gout_old;
extern modelica_boolean *gout_res;
extern modelica_boolean *backuprelations;

/*
 * this is used for initialize the DATA structure that is used in
 * all the generated functions.
 * The parameter controls what vectors should be initilized in
 * in the structure. Usually you can use the "ALL" flag which
 * initilizes all the vectors. This is needed for example in those ocasions
 * when another process have allocated the needed vectors.
 * Make sure that you call this function first because it sets the non-initialize
 * pointer to 0.
 *
 * This flag should be the same for second argument in deInitializeDataStruc
 * to avoid memory leak.
 */
DATA* initializeDataStruc();

/* this frees the memory that is allocated in the data-structure.
 * The second argument must have the same value as the argument in initializeDataStruc
 */
void deInitializeDataStruc(DATA* data);
/* this is used to set the localData in the generated code
 * that is used in the diferrent generated functions
 *
 */
void setLocalData(DATA* data);

/* defined in model code. Used to get name of variable by investigating its pointer in the state or alg vectors. */
const char* getNameReal(double* ptr);
const char* getNameInt(modelica_integer* ptr);
const char* getNameBool(modelica_boolean* ptr);
const char* getNameString(const char** ptr);

void storeExtrapolationData();

/* function for calculating ouput values */
/*used in DDASRT fortran function*/
int
functionODE();
int
functionAlgebraics();
int
functionAliasEquations();

/* function for calculating state values on residual form */
/*used in DDASRT fortran function*/
int
functionODE_residual(double *t, double *x, double *xprime, double *delta, fortran_integer *ires, double *rpar, fortran_integer* ipar);

/* Function for calling external object constructors */
void
callExternalObjectConstructors(DATA*);

/*   function for calculating all equation sorting order 
  uses in EventHandle  */
int
functionDAE(int *needToIterate);

/* function for storing value histories of delayed expressions
 * called from functionDAE_output()
 */
int
function_storeDelayed();

/* function for calculating states on explicit ODE form */
/*used in functionDAE_res function*/
int functionODE_inline();

/* function for calculate initial values from initial equations and fixed start attibutes */
int initial_function();

/* function for calculating bound parameters that depend on other parameters, e.g. parameter Real n=1/m; */
int bound_parameters();

/* function for checking for asserts and terminate */
int checkForAsserts();

/* function for calculate residual values for the initial equations and fixed start attibutes */
int initial_residual();

/* function for initializing time instants when sample() is activated */
void function_sampleInit();

/* function for calculation Jacobian */
extern int jac_flag;  /* Flag for DASSL to work with analytical Jacobian */
extern int num_jac_flag;  /* Flag for DASSL to work with selfmade numerical Jacobian */
int functionJacA(double* jac);
int functionJacB(double* jac);
int functionJacC(double* jac);
int functionJacD(double* jac);

int isInteractiveSimulation();

double newTime(double t, double step,double stop);

#define MODELICA_ASSERT(info,msg) { terminationAssert = 1; TermMsg = msg; TermInfo = info;}

#define MODELICA_TERMINATE(msg)  { modelTermination=1; \
terminationTerminate = 1; TermMsg = msg; TermInfo = omc_dummyFileInfo; }

#define initial() localData->init

#endif

