// Simulation code for RightHeartStarling generated by the OpenModelica Compiler.
#include "modelica.h"
#include "assert.h"
#include "string.h"
#include "simulation_runtime.h"

#if defined(_MSC_VER) && !defined(_SIMULATION_RUNTIME_H)
  #define DLLExport   __declspec( dllexport ) 
#else 
  #define DLLExport /* nothing */
#endif 

#include "RightHeartStarling_functions.cpp"

extern "C" {

}

#define NHELP 0
#define NG 1//number of zero crossing
#define NX 1
#define NY 8
#define NP 6 // number of parameters
#define NO 0 // number of outputvar on topmodel
#define NI 0 // number of inputvar on topmodel
#define NR 10 // number of residuals for initialialization function
#define NEXT 0 // number of external objects
#define MAXORD 5
#define NYSTR 0 // number of alg. string variables
#define NPSTR 0 // number of alg. string variables

static DATA* localData = 0;
#define time localData->timeValue
extern "C" { /* adrpo: this is needed for Visual C++ compilation to work! */
  char *model_name="RightHeartStarling";
  char *model_dir="";
}
char* state_names[1]={"volume"};
char* derivative_names[1]={"der(volume)"};
char* algvars_names[8]={"bloodFlow", "EDV", "ESV", "inflow", "outflow", "delta", "ventricleSteadyStateVolume", "Pi"};
char* input_names[1] = {""};
char* output_names[1] = {""};
char* param_names[6]={"K", "HR", "PericardiumPressure", "stiffnes", "contractility", "Po"};
char* string_alg_names[1] = {""};
char* string_param_names[1] = {""};

char* state_comments[1]={ "average ventricle volume"};
char* derivative_comments[1]={ "average ventricle volume"};
char* algvars_comments[8]={"",  "end diastolic volume",  "end systolic volume", "", "", "", "",  "atrium pressure"};
char* input_comments[1] = {""};
char* output_comments[1] = {""};
char* param_comments[6]={ "time adaptation coeficient of average ventricle blood volume",  "heart rate", "", "", "",  "artery pressure"};
char* string_param_comments[1] = {""};
char* string_alg_comments[1] = {""};

#define $Pi localData->algebraics[7]
#define $Po localData->parameters[5]
#define $contractility localData->parameters[4]
#define $stiffnes localData->parameters[3]
#define $PericardiumPressure localData->parameters[2]
#define $HR localData->parameters[1]
#define $K localData->parameters[0]
#define $volume localData->states[0]
#define $DER$volume localData->statesDerivatives[0]
#define $ventricleSteadyStateVolume localData->algebraics[6]
#define $delta localData->algebraics[5]
#define $outflow localData->algebraics[4]
#define $inflow localData->algebraics[3]
#define $ESV localData->algebraics[2]
#define $EDV localData->algebraics[1]
#define $bloodFlow localData->algebraics[0]

char* getName( double* ptr)
{
  if( &$Pi == ptr ) return algvars_names[7];
  if( &$Po == ptr ) return param_names[5];
  if( &$contractility == ptr ) return param_names[4];
  if( &$stiffnes == ptr ) return param_names[3];
  if( &$PericardiumPressure == ptr ) return param_names[2];
  if( &$HR == ptr ) return param_names[1];
  if( &$K == ptr ) return param_names[0];
  if( &$DER$volume == ptr ) return derivative_names[0];
  if( &$volume == ptr ) return state_names[0];
  if( &$ventricleSteadyStateVolume == ptr ) return algvars_names[6];
  if( &$delta == ptr ) return algvars_names[5];
  if( &$outflow == ptr ) return algvars_names[4];
  if( &$inflow == ptr ) return algvars_names[3];
  if( &$ESV == ptr ) return algvars_names[2];
  if( &$EDV == ptr ) return algvars_names[1];
  if( &$bloodFlow == ptr ) return algvars_names[0];
  return "";
}

static char init_fixed[NX+NX+NY+NP]={
	1/*volume*/, 1/*default*/, 0/*bloodFlow*/, 0/*EDV*/, 
	0/*ESV*/, 0/*inflow*/, 0/*outflow*/, 
	0/*delta*/, 0/*ventricleSteadyStateVolume*/, 0/*Pi*/, 
	1/*K*/, 1/*HR*/, 1/*PericardiumPressure*/, 
	1/*stiffnes*/, 1/*contractility*/, 1/*Po*/
};

char var_attr[NX+NY+NP]={/*volume:*/1+0, /*bloodFlow:*/1+0, /*EDV:*/1+0, /*ESV:*/1+0, 
/*inflow:*/1+0, /*outflow:*/1+0, /*delta:*/1+0, 
/*ventricleSteadyStateVolume:*/1+0, /*Pi:*/1+0, /*K:*/1+0, 
/*HR:*/1+0, /*PericardiumPressure:*/1+0, /*stiffnes:*/1+0, 
/*contractility:*/1+0, /*Po:*/1+0};

#define DIVISION(a,b,c) ((b != 0) ? a / b : a / division_error(b,c))


int encounteredDivisionByZero = 0;
double division_error(double b,const char* division_str)
{
  if(!encounteredDivisionByZero){
    fprintf(stderr,"ERROR: Division by zero in partial equation: %s.\n",division_str);
    encounteredDivisionByZero = 1;
   }
   return b;
}

void setLocalData(DATA* data)
{
   localData = data;
}

DATA* initializeDataStruc(DATA_FLAGS flags)
{
  DATA* returnData = (DATA*)malloc(sizeof(DATA));

  if(!returnData) //error check
    return 0;

  memset(returnData,0,sizeof(DATA));
  returnData->nStates = NX;
  returnData->nAlgebraic = NY;
  returnData->nParameters = NP;
  returnData->nInputVars = NI;
  returnData->nOutputVars = NO;
  returnData->nZeroCrossing = NG;
  returnData->nInitialResiduals = NR;
  returnData->nHelpVars = NHELP;
  returnData->stringVariables.nParameters = NPSTR;
  returnData->stringVariables.nAlgebraic = NYSTR;

  if(flags & STATES && returnData->nStates) {
    returnData->states = (double*) malloc(sizeof(double)*returnData->nStates);
    returnData->oldStates = (double*) malloc(sizeof(double)*returnData->nStates);
    returnData->oldStates2 = (double*) malloc(sizeof(double)*returnData->nStates);
    assert(returnData->states&&returnData->oldStates&&returnData->oldStates2);
    memset(returnData->states,0,sizeof(double)*returnData->nStates);
    memset(returnData->oldStates,0,sizeof(double)*returnData->nStates);
    memset(returnData->oldStates2,0,sizeof(double)*returnData->nStates);
  } else {
    returnData->states = 0;
    returnData->oldStates = 0;
    returnData->oldStates2 = 0;
  }

  if(flags & STATESDERIVATIVES && returnData->nStates) {
    returnData->statesDerivatives = (double*) malloc(sizeof(double)*returnData->nStates);
    returnData->oldStatesDerivatives = (double*) malloc(sizeof(double)*returnData->nStates);
    returnData->oldStatesDerivatives2 = (double*) malloc(sizeof(double)*returnData->nStates);
    assert(returnData->statesDerivatives&&returnData->oldStatesDerivatives&&returnData->oldStatesDerivatives2);
    memset(returnData->statesDerivatives,0,sizeof(double)*returnData->nStates);
    memset(returnData->oldStatesDerivatives,0,sizeof(double)*returnData->nStates);
    memset(returnData->oldStatesDerivatives2,0,sizeof(double)*returnData->nStates);
  } else {
    returnData->statesDerivatives = 0;
    returnData->oldStatesDerivatives = 0;
    returnData->oldStatesDerivatives2 = 0;
  }

  if(flags & HELPVARS && returnData->nHelpVars) {
    returnData->helpVars = (double*) malloc(sizeof(double)*returnData->nHelpVars);
    assert(returnData->helpVars);
    memset(returnData->helpVars,0,sizeof(double)*returnData->nHelpVars);
  } else {
    returnData->helpVars = 0;
  }

  if(flags & ALGEBRAICS && returnData->nAlgebraic) {
    returnData->algebraics = (double*) malloc(sizeof(double)*returnData->nAlgebraic);
    returnData->oldAlgebraics = (double*) malloc(sizeof(double)*returnData->nAlgebraic);
    returnData->oldAlgebraics2 = (double*) malloc(sizeof(double)*returnData->nAlgebraic);
    assert(returnData->algebraics&&returnData->oldAlgebraics&&returnData->oldAlgebraics2);
    memset(returnData->algebraics,0,sizeof(double)*returnData->nAlgebraic);
    memset(returnData->oldAlgebraics,0,sizeof(double)*returnData->nAlgebraic);
    memset(returnData->oldAlgebraics2,0,sizeof(double)*returnData->nAlgebraic);
  } else {
    returnData->algebraics = 0;
    returnData->oldAlgebraics = 0;
    returnData->oldAlgebraics2 = 0;
    returnData->stringVariables.algebraics = 0;
  }

  if (flags & ALGEBRAICS && returnData->stringVariables.nAlgebraic) {
    returnData->stringVariables.algebraics = (char**)malloc(sizeof(char*)*returnData->stringVariables.nAlgebraic);
    assert(returnData->stringVariables.algebraics);
    memset(returnData->stringVariables.algebraics,0,sizeof(char*)*returnData->stringVariables.nAlgebraic);
  } else {
    returnData->stringVariables.algebraics=0;
  }

  if(flags & PARAMETERS && returnData->nParameters) {
    returnData->parameters = (double*) malloc(sizeof(double)*returnData->nParameters);
    assert(returnData->parameters);
    memset(returnData->parameters,0,sizeof(double)*returnData->nParameters);
  } else {
    returnData->parameters = 0;
  }

  if (flags & PARAMETERS && returnData->stringVariables.nParameters) {
  	  returnData->stringVariables.parameters = (char**)malloc(sizeof(char*)*returnData->stringVariables.nParameters);
      assert(returnData->stringVariables.parameters);
      memset(returnData->stringVariables.parameters,0,sizeof(char*)*returnData->stringVariables.nParameters);
  } else {
      returnData->stringVariables.parameters=0;
  }

  if(flags & OUTPUTVARS && returnData->nOutputVars) {
    returnData->outputVars = (double*) malloc(sizeof(double)*returnData->nOutputVars);
    assert(returnData->outputVars);
    memset(returnData->outputVars,0,sizeof(double)*returnData->nOutputVars);
  } else {
    returnData->outputVars = 0;
  }

  if(flags & INPUTVARS && returnData->nInputVars) {
    returnData->inputVars = (double*) malloc(sizeof(double)*returnData->nInputVars);
    assert(returnData->inputVars);
    memset(returnData->inputVars,0,sizeof(double)*returnData->nInputVars);
  } else {
    returnData->inputVars = 0;
  }

  if(flags & INITIALRESIDUALS && returnData->nInitialResiduals) {
    returnData->initialResiduals = (double*) malloc(sizeof(double)*returnData->nInitialResiduals);
    assert(returnData->initialResiduals);
    memset(returnData->initialResiduals,0,sizeof(double)*returnData->nInitialResiduals);
  } else {
    returnData->initialResiduals = 0;
  }

  if(flags & INITFIXED) {
    returnData->initFixed = init_fixed;
  } else {
    returnData->initFixed = 0;
  }

  /*   names   */
  if(flags & MODELNAME) {
    returnData->modelName = model_name;
  } else {
    returnData->modelName = 0;
  }
  
  if(flags & STATESNAMES) {
    returnData->statesNames = state_names;
  } else {
    returnData->statesNames = 0;
  }

  if(flags & STATESDERIVATIVESNAMES) {
    returnData->stateDerivativesNames = derivative_names;
  } else {
    returnData->stateDerivativesNames = 0;
  }

  if(flags & ALGEBRAICSNAMES) {
    returnData->algebraicsNames = algvars_names;
  } else {
    returnData->algebraicsNames = 0;
  }

  if(flags & PARAMETERSNAMES) {
    returnData->parametersNames = param_names;
  } else {
    returnData->parametersNames = 0;
  }

  if(flags & INPUTNAMES) {
    returnData->inputNames = input_names;
  } else {
    returnData->inputNames = 0;
  }

  if(flags & OUTPUTNAMES) {
    returnData->outputNames = output_names;
  } else {
    returnData->outputNames = 0;
  }

  /*   comments  */
  if(flags & STATESCOMMENTS) {
    returnData->statesComments = state_comments;
  } else {
    returnData->statesComments = 0;
  }

  if(flags & STATESDERIVATIVESCOMMENTS) {
    returnData->stateDerivativesComments = derivative_comments;
  } else {
    returnData->stateDerivativesComments = 0;
  }

  if(flags & ALGEBRAICSCOMMENTS) {
    returnData->algebraicsComments = algvars_comments;
  } else {
    returnData->algebraicsComments = 0;
  }

  if(flags & PARAMETERSCOMMENTS) {
    returnData->parametersComments = param_comments;
  } else {
    returnData->parametersComments = 0;
  }

  if(flags & INPUTCOMMENTS) {
    returnData->inputComments = input_comments;
  } else {
    returnData->inputComments = 0;
  }

  if(flags & OUTPUTCOMMENTS) {
    returnData->outputComments = output_comments;
  } else {
    returnData->outputComments = 0;
  }

  if (flags & EXTERNALVARS) {
    returnData->extObjs = (void**)malloc(sizeof(void*)*NEXT);
    if (!returnData->extObjs) {
      printf("error allocating external objects\n");
      exit(-2);
    }
    memset(returnData->extObjs,0,sizeof(void*)*NEXT);
    setLocalData(returnData); /* must be set since used by constructors*/
  }
  return returnData;
}

void deInitializeDataStruc(DATA* data, DATA_FLAGS flags)
{
  if(!data)
    return;

  if(flags & STATES && data->states) {
    free(data->states);
    data->states = 0;
  }

  if(flags & STATESDERIVATIVES && data->statesDerivatives) {
    free(data->statesDerivatives);
    data->statesDerivatives = 0;
  }

  if(flags & ALGEBRAICS && data->algebraics) {
    free(data->algebraics);
    data->algebraics = 0;
  }

  if(flags & PARAMETERS && data->parameters) {
    free(data->parameters);
    data->parameters = 0;
  }

  if(flags & OUTPUTVARS && data->inputVars) {
    free(data->inputVars);
    data->inputVars = 0;
  }

  if(flags & INPUTVARS && data->outputVars) {
    free(data->outputVars);
    data->outputVars = 0;
  }
  
  if(flags & INITIALRESIDUALS && data->initialResiduals){
    free(data->initialResiduals);
    data->initialResiduals = 0;
  }
  if (flags & EXTERNALVARS && data->extObjs) {
    free(data->extObjs);
    data->extObjs = 0;
  }
}

int functionDAE_output()
{
  state mem_state;
  modelica_boolean tmp0;
  modelica_boolean tmp1;
  modelica_real tmp2;
  mem_state = get_memory_state();
  $bloodFlow = ($HR * ($EDV - $ESV));
  RELATIONLESS(tmp0,$delta,0.0);
  tmp1 = tmp0;
  if (tmp1) {
    tmp2 = $bloodFlow;
  }
  else {
    tmp2 = ($bloodFlow + $delta);
  }
  $inflow = tmp2;
  $outflow = ($delta - $inflow);
  $Pi = time;
  restore_memory_state(mem_state);
  return 0;
}

int functionDAE_output2()
{
  state mem_state;
  mem_state = get_memory_state();
  $Pi = time;
  restore_memory_state(mem_state);
  return 0;
}

/*
*/
int input_function()
{
  return 0;
}

/*
*/
int output_function()
{
  return 0;
}

int functionDAE_res(double *t, double *x, double *xd, double *delta, long int *ires, double *rpar, long int* ipar)
{
  int i;
  double temp_xd[NX];
  double* statesBackup;
  double* statesDerivativesBackup;
  double timeBackup;
  statesBackup = localData->states;
  statesDerivativesBackup = localData->statesDerivatives;
  timeBackup = localData->timeValue;
  localData->states = x;
  for (i=0; i<localData->nStates; i++) temp_xd[i]=localData->statesDerivatives[i];
  
  localData->statesDerivatives = temp_xd;
  localData->timeValue = *t;
  
  functionODE();
  /* get the difference between the temp_xd(=localData->statesDerivatives) and xd(=statesDerivativesBackup) */
  for (i=0; i < localData->nStates; i++) delta[i]=localData->statesDerivatives[i]-statesDerivativesBackup[i];
  
  localData->states = statesBackup;
  localData->statesDerivatives = statesDerivativesBackup;
  localData->timeValue = timeBackup;
  if (modelErrorCode) {
      if (ires) *ires = -1;
      modelErrorCode =0;
  }
  return 0;
}

int function_zeroCrossing(long *neqm, double *t, double *x, long *ng, double *gout, double *rpar, long* ipar)
{
  double timeBackup;
  state mem_state;
  mem_state = get_memory_state();
  timeBackup = localData->timeValue;
  localData->timeValue = *t;
  functionODE();
  functionDAE_output();
  ZEROCROSSING(0,Less($delta,0.0));
  restore_memory_state(mem_state);
  localData->timeValue = timeBackup;
  return 0;
}

int handleZeroCrossing(long index)
{
  state mem_state;
  mem_state = get_memory_state();
  switch(index) {
    case 0:
      save($inflow);
      break;

    default: break;
  }
  restore_memory_state(mem_state);
  return 0;
}

int function_updateDependents()
{
  state mem_state;
  modelica_boolean tmp0;
  modelica_boolean tmp1;
  modelica_real tmp2;
  inUpdate=initial()?0:1;
  mem_state = get_memory_state();
  $ESV = (0.080251025206847 * (pow((modelica_real)((9.0 + $Po) - $PericardiumPressure), (modelica_real)2.0) * pow((modelica_real)(1.0 / $contractility), (modelica_real)2.0)));
  $EDV = (62.0173672946042 * (pow((modelica_real)(time - $PericardiumPressure), (modelica_real)0.5) * pow((modelica_real)(1.0 / $stiffnes), (modelica_real)0.5)));
  $ventricleSteadyStateVolume = (($EDV / 2.0) + ($ESV / 2.0));
  $delta = (($ventricleSteadyStateVolume - $volume) * $K);
  $DER$volume = ($delta / 60.0);
  $bloodFlow = ($HR * ($EDV - $ESV));
  RELATIONLESS(tmp0,$delta,0.0);
  tmp1 = tmp0;
  if (tmp1) {
    tmp2 = $bloodFlow;
  }
  else {
    tmp2 = ($bloodFlow + $delta);
  }
  $inflow = tmp2;
  $outflow = ($delta - $inflow);
  restore_memory_state(mem_state);
  inUpdate=0;
  return 0;
}

int function_onlyZeroCrossings(double *gout,double *t)
{
  state mem_state;
  mem_state = get_memory_state();
  ZEROCROSSING(0,Less($delta,0.0));
  restore_memory_state(mem_state);
  return 0;
}

int function_when(int i)
{
  state mem_state;
  mem_state = get_memory_state();
  switch(i) {
    default: break;
  }
  restore_memory_state(mem_state);
  return 0;
}

int functionODE()
{
  state mem_state;
  mem_state = get_memory_state();
  $ESV = (0.080251025206847 * (pow((modelica_real)((9.0 + $Po) - $PericardiumPressure), (modelica_real)2.0) * pow((modelica_real)(1.0 / $contractility), (modelica_real)2.0)));
  $EDV = (62.0173672946042 * (pow((modelica_real)(time - $PericardiumPressure), (modelica_real)0.5) * pow((modelica_real)(1.0 / $stiffnes), (modelica_real)0.5)));
  $ventricleSteadyStateVolume = (($EDV / 2.0) + ($ESV / 2.0));
  $delta = (($ventricleSteadyStateVolume - $volume) * $K);
  $DER$volume = ($delta / 60.0);
  restore_memory_state(mem_state);
  return 0;
}

int initial_function()
{
  return 0;
}

int initial_residual()
{
  modelica_boolean tmp0;
  modelica_boolean tmp1;
  modelica_real tmp2;
  int i=0;
  state mem_state;
  mem_state = get_memory_state();
  localData->initialResiduals[i++] = ($bloodFlow - ($HR * ($EDV - $ESV)));
  localData->initialResiduals[i++] = ($EDV - (62.0173672946042 * (pow((modelica_real)(time - $PericardiumPressure), (modelica_real)0.5) * pow((modelica_real)(1.0 / $stiffnes), (modelica_real)0.5))));
  localData->initialResiduals[i++] = ($ESV - (0.080251025206847 * (pow((modelica_real)((9.0 + $Po) - $PericardiumPressure), (modelica_real)2.0) * pow((modelica_real)(1.0 / $contractility), (modelica_real)2.0))));
  localData->initialResiduals[i++] = ($ventricleSteadyStateVolume - (($EDV / 2.0) + ($ESV / 2.0)));
  localData->initialResiduals[i++] = ($delta - (($ventricleSteadyStateVolume - $volume) * $K));
  localData->initialResiduals[i++] = (($inflow + $outflow) - $delta);
  RELATIONLESS(tmp0,$delta,0.0);
  tmp1 = tmp0;
  if (tmp1) {
    tmp2 = $bloodFlow;
  }
  else {
    tmp2 = ($bloodFlow + $delta);
  }
  localData->initialResiduals[i++] = ($inflow - tmp2);
  localData->initialResiduals[i++] = ($DER$volume - ($delta / 60.0));
  localData->initialResiduals[i++] = ($Pi - time);
  localData->initialResiduals[i++] = ($volume - 95.7);
  restore_memory_state(mem_state);
  return 0;
}

int bound_parameters()
{
  state mem_state;
  mem_state = get_memory_state();
  restore_memory_state(mem_state);
  return 0;
}

int checkForDiscreteVarChanges()
{
  int needToIterate=0;
  
  for (long i = 0; i < localData->nHelpVars; i++) {
    if (change(localData->helpVars[i])) { needToIterate=1; }
  }
  return needToIterate;
}
