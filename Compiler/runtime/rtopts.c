/*
 * This file is part of OpenModelica.
 *
 * Copyright (c) 1998-2008, Link�pings University,
 * Department of Computer and Information Science,
 * SE-58183 Link�ping, Sweden.
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
 * from Link�pings University, either from the above address,
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

#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <errno.h>
#include "rml.h"

static int type_info;
static int split_arrays;
static int modelica_output;
static int debug_flag_info;
static int params_struct;
static int version_request;

/* Level of eliminations of equations.
 * 0 - None
 * 1 - Only aliases (a=b) (default)
 * 2 - Full (a=-b, a=b, a=constant)
 * 3 - Only constants (a = constant)
 * */
static int elimination_level=2;

static char **debug_flags;
static char *debug_flagstr;
static int debug_flagc;
static int debug_all;
static int debug_none;
int nproc;
double latency=0.0;
double bandwidth=0.0;
int simulation_cg;
int silent;
char* simulation_code_target = "gcc";

/*
 * adrpo 2007-06-11
 * flag for accepting only Modelica grammar or MetaModelica grammar
 */
#define GRAMMAR_MODELICA 0
#define GRAMMAR_METAMODELICA 1
int acceptedGrammar = GRAMMAR_MODELICA;

/*
 * @author adrpo
 * @date 2007-02-08
 * This variable is defined in corbaimpl.cpp and set
 * here by function setCorbaSessionName(char* name);
 */
extern char* corbaSessionName;

/*
 * adrpo 2008-11-28
 * flag for accepting different version of Modelica annotations
 */
char* annotation_version = "2.x";

/*
 * adrpo 2008-12-15
 * flag +showErrorMessages for printing all messages comming to the error buffer
 */
int showErrorMessages = 0;

void RTOpts_5finit(void)
{
  type_info = 0;
  split_arrays = 1;
  modelica_output = 0;
  debug_flag_info = 0;
  params_struct = 0;
  debug_all = 0;
  debug_none = 1;
  nproc = 0;
  simulation_cg = 0;
  simulation_code_target = "gcc";
  silent = 0;
  version_request = 0;
  corbaSessionName = 0;
  acceptedGrammar = GRAMMAR_MODELICA;
  annotation_version = "2.x";
  showErrorMessages = 0;
}

/*
 * @author adrpo
 * this fuctions sets the name that should be appended to the Corba IOR file dumped by omc
 * by default the file has the name:
 * - on Windows: /tmp/openmodelica.objid
 * - on Linux  : /tmp/openmodelica.user.objid
 * To this filename a ".$corba_session_name" is appended where
 * $corba_session_name is set in this function if omc is called:
 * ./omc +c=corba_session_name +d=interactiveCorba
 * By default the corba_session_name is set to "".
 * see more into corbaimpl.cpp function Corba__initialize
 */
static int setCorbaSessionName(char *name)
{
  int i;
  int len=strlen(name);
  if (len==0) return -1;

  corbaSessionName = strdup(name);
  return 0;
}


static int set_debug_flags(char *flagstr)
{
  int i;
  int len=strlen(flagstr);
  int flagc=1;
  int flag;

  debug_none = 0; /* -d was given, hence turn off the virtual flag "none". */

  if (len==0) {
    debug_flagc = 0;
    debug_flagstr = (char*)malloc(sizeof(char));
    debug_flagstr = '\0';
    debug_all = 0;
    debug_flags = 0;
    return 0;
  }

  debug_flagstr=(char*)malloc((len+1)*sizeof(char));
  strcpy(debug_flagstr, flagstr);

  for (i=0;i<len;i++)
    if (debug_flagstr[i]==',')
      flagc++;
  debug_flags = (char**)malloc(flagc * sizeof(char*));
  debug_flags[0]=debug_flagstr;
  flag=1;
  for (i=1; i<len; i++) {
    if (debug_flagstr[i-1]==',') {
      debug_flags[flag]=&(debug_flagstr[i]);
      debug_flagstr[i-1]=0;
      if (strcmp("all", debug_flags[flag-1])==0) {
	debug_all=1;
      }
      flag++;
    }
  }
  if (strcmp("all", debug_flags[flag-1])==0) {
    debug_all=1;
  }
  if (flag!=flagc) {
    fprintf(stderr, "Error in setting flags.\n",flag,flagc);
    return -1;
  }

  debug_flagc=flagc;

  /*
  for (i=0; i<debug_flagc; i++) {
    printf("\n%d=%s\n",i,debug_flags[i]);
  }
  */
  return 0;
}
static int set_debug_flag(char const* strdata, int value)
{
	int length=strlen(strdata),i;
	for (i=0; i<debug_flagc; i++)
	{
		if (strcmp(debug_flags[i], strdata)==0)
		{
			if(value == 0 )
			{
				debug_flags[i] = "";
				// TODO: realloc memory when count(empty_entries) > _const
				return 0;
			}
			return 1;
		}
		length += strlen( debug_flags[i]);
	}
	if(value == 0)
		return 0;
	debug_flagc+=1;
	debug_flags = (char**)realloc(debug_flags, debug_flagc * sizeof(char*));
	debug_flags[debug_flagc-1] = (char*)strdata;
	return 1;
}

int check_debug_flag(char const* strdata)
{
  int flg=0;
  int i;
  int containFailtrace = 0;
  for (i=0; i<debug_flagc; i++) {
    if (strcmp(debug_flags[i], "failtrace")==0) {
      containFailtrace=1;
      break;
  	}
  }
  if (strcmp(strdata,"none")==0 && (debug_none == 1 || containFailtrace==0 ) ) {
    flg=1;
  }
  if (debug_all==1) {
    flg=1;
  }
  for (i=0; i<debug_flagc; i++) {
    if (strcmp(debug_flags[i], strdata)==0) {
      flg=1;
      break;
    }
    else if (debug_flags[i][0]=='-' &&
	     strcmp(debug_flags[i]+1, strdata)==0) {
      flg=0;
      break;
    }
  }
  if (debug_flag_info == 1) {
    if (flg==1)
      fprintf(stdout, "--------- %s = 1 ---------\n", strdata);
    else
      fprintf(stdout, "--------- %s = 0 ---------\n", strdata);
  }

  return flg;
}

#define VERSION_OPT1        "++v"
#define VERSION_OPT2        "+version"
#define ANNOTATION_VERSION  "+annotationVersion"
#define TARGET              "+target"
#define METAMODELICA        "+g"
#define SHOW_ERROR_MESSAGES "+showErrorMessages"

RML_BEGIN_LABEL(RTOpts__args)
{
  void *args = rmlA0;
  void *res = (void*)mk_nil();
  int strLen_TARGET = strlen(TARGET);
  int strLen_METAMODELICA = strlen(METAMODELICA);
  int strLen_ANNNOTATION_VERSION = strlen(ANNOTATION_VERSION);
  int strLen_SHOW_ERROR_MESSAGES = strlen(SHOW_ERROR_MESSAGES);

  debug_none = 1;

  while (RML_GETHDR(args) != RML_NILHDR)
  {
    char *arg = RML_STRINGDATA(RML_CAR(args));
    if(strcmp(arg,VERSION_OPT1) == 0 ||
       strcmp(arg,VERSION_OPT2) == 0)
    {
    	version_request = 1;
    }
    else if(strncmp(arg,TARGET,strLen_TARGET) == 0)
    {
    	if (strlen(arg) >= strLen_TARGET && strcmp(&arg[strLen_TARGET], "=gcc") == 0)
    		simulation_code_target = "gcc";
    	else if (strlen(arg) >= strLen_TARGET && strcmp(&arg[strLen_TARGET], "=msvc") == 0)
    		simulation_code_target = "msvc";
    	else
    	{
        fprintf(stderr, "# Wrong option: usage: omc [+target=gcc|msvc], default to 'gcc'.\n");
        RML_TAILCALLK(rmlFC);
    	}
    }
    else if(strncmp(arg,METAMODELICA,strLen_METAMODELICA) == 0)
    {
        if (strlen(arg) >= strLen_METAMODELICA && strcmp(&arg[strLen_METAMODELICA], "=MetaModelica") == 0)
            acceptedGrammar = GRAMMAR_METAMODELICA;
        else if (strlen(arg) >= strLen_METAMODELICA && strcmp(&arg[strLen_METAMODELICA], "=Modelica") == 0)
            acceptedGrammar = GRAMMAR_MODELICA;
        else
        {
          fprintf(stderr, "# Wrong option: usage: omc [+g=Modelica|MetaModelica], default to 'Modelica'.\n");
          RML_TAILCALLK(rmlFC);
        }
    }
    else if(strncmp(arg,ANNOTATION_VERSION,strLen_ANNNOTATION_VERSION) == 0)
    {
      if (strlen(arg) >= strLen_ANNNOTATION_VERSION && strcmp(&arg[strLen_ANNNOTATION_VERSION], "=1.x") == 0)
        annotation_version = "1.x";
      else if (strlen(arg) >= strLen_ANNNOTATION_VERSION && strcmp(&arg[strLen_ANNNOTATION_VERSION], "=2.x") == 0)
        annotation_version = "2.x";
      else if (strlen(arg) >= strLen_ANNNOTATION_VERSION && strcmp(&arg[strLen_ANNNOTATION_VERSION], "=3.x") == 0)
        annotation_version = "3.x";
      else
      {
        fprintf(stderr, "# Wrong option: usage: omc [+annotationVersion=1.x|2.x|3.x], default to '2.x'.\n");
        RML_TAILCALLK(rmlFC);
      }
    }
    else if(strncmp(arg,SHOW_ERROR_MESSAGES,strLen_SHOW_ERROR_MESSAGES) == 0)
    {
        if (strlen(arg) == strLen_SHOW_ERROR_MESSAGES)
            showErrorMessages = 1;
        else
        {
          fprintf(stderr, "# Wrong option: usage: omc [+showErrorMessages], default to not show them.\n");
          RML_TAILCALLK(rmlFC);
        }
    }
    else if (arg[0] == '+')
    {
      if (strlen(arg) < 2)
      {
    		fprintf(stderr, "# Unknown option: %s \n", arg);
    		RML_TAILCALLK(rmlFC);
      }
      switch (arg[1]) {
      case 't':
	type_info = 1;
	break;
      case 'a':
	split_arrays = 0;
	type_info = 0;
	break;
      case 's':
	simulation_cg = 1;
	break;
      case 'm':
	modelica_output = 1;
	break;
      case 'p':
	params_struct = 1;
	break;
      case 'q':
	silent = 1;
	break;
	  case 'c':
	if (arg[2]!='=' || setCorbaSessionName(&(arg[3])) != 0)
	{
	  fprintf(stderr, "# Flag Usage:  +c=corbaSessionName\n") ;
	  RML_TAILCALLK(rmlFC);
	}
	break;
      case 'd':
	if (arg[2]=='d') {
	  debug_flag_info = 1;
	  break;
	}
	if (arg[2]!='=' ||
	    set_debug_flags(&(arg[3])) != 0) {
	  fprintf(stderr, "# Flag Usage:  +d=flg1,flg2,...\n") ;
	  fprintf(stderr, "#              +dd for debug flag info\n");
	  RML_TAILCALLK(rmlFC);
	}
	break;
      case 'n':
	if (arg[2] != '=') {
	  fprintf(stderr, "# Flag Usage:  +n=<no. of proc>") ;
	  RML_TAILCALLK(rmlFC);
	}
	nproc = atoi(&arg[3]);
	if (nproc == 0) {
	  fprintf(stderr, "Error, integer value expected for number of processors.\n") ;
	  RML_TAILCALLK(rmlFC);
	}
	break;
      case 'l':
	if (arg[2] != '=') {
	  fprintf(stderr, "# Flag Usage:  +l=<latency value>") ;
	  RML_TAILCALLK(rmlFC);
	}
	latency = (double)atoi(&arg[3]); /* ,NULL); */
	if (latency == 0.0) {
	  fprintf(stderr, "Error, integer expected for latency.\n") ;
	  RML_TAILCALLK(rmlFC);
	}
	break;
      case 'b':
	if (arg[2] != '=') {
	  fprintf(stderr, "# Flag Usage:  +b=<bandwidth value>") ;
	  RML_TAILCALLK(rmlFC);
	}
	bandwidth = (double)atoi(&arg[3]);
	if (bandwidth == 0.0) {
	  fprintf(stderr, "Error, integer expected for bandwidth.\n") ;
	  RML_TAILCALLK(rmlFC);
	}
	break;
	// Which level of algebraic elimination to use.
	  case 'e':
	if (arg[2] != '=') {
	  fprintf(stderr, "# Flag Usage:  +e=<algebraic_elimination_level 0, 1, 2(default) or 3>") ;
	  RML_TAILCALLK(rmlFC);
	}
	elimination_level = (int)atoi(&arg[3]);
	if (elimination_level < 0 || elimination_level > 3) {
		elimination_level = 2;
	  fprintf(stderr, "Warning, wrong value of elimination level, will use default = %d\n",elimination_level) ;
	}
	break;
      default:
	fprintf(stderr, "# Unknown option: %s\n", arg);
	RML_TAILCALLK(rmlFC);
      }
    }
    else
      res = (void*)mk_cons(RML_CAR(args), res);
    args = RML_CDR(args);
  }

  rmlA0 = res;
  RML_TAILCALLK(rmlSC);
}
RML_END_LABEL

RML_BEGIN_LABEL(RTOpts__typeinfo)
{
  rmlA0 = RML_PRIM_MKBOOL(type_info);
  RML_TAILCALLK(rmlSC);
}
RML_END_LABEL

RML_BEGIN_LABEL(RTOpts__splitArrays)
{
  rmlA0 = RML_PRIM_MKBOOL(split_arrays);
  RML_TAILCALLK(rmlSC);
}
RML_END_LABEL

RML_BEGIN_LABEL(RTOpts__modelicaOutput)
{
  rmlA0 = RML_PRIM_MKBOOL(modelica_output);
  RML_TAILCALLK(rmlSC);
}
RML_END_LABEL

RML_BEGIN_LABEL(RTOpts__paramsStruct)
{
  rmlA0 = RML_PRIM_MKBOOL(params_struct);
  RML_TAILCALLK(rmlSC);
}
RML_END_LABEL

RML_BEGIN_LABEL(RTOpts__silent)
{
  rmlA0 = RML_PRIM_MKBOOL(silent);
  RML_TAILCALLK(rmlSC);
}
RML_END_LABEL

RML_BEGIN_LABEL(RTOpts__eliminationLevel)
{
	rmlA0 = mk_icon(elimination_level);
	RML_TAILCALLK(rmlSC);
}
RML_END_LABEL

RML_BEGIN_LABEL(RTOpts__setEliminationLevel)
{
	int level = (int)RML_IMMEDIATE(RML_UNTAGFIXNUM(rmlA0));
	elimination_level = level;
	RML_TAILCALLK(rmlSC);
}
RML_END_LABEL

RML_BEGIN_LABEL(RTOpts__setDebugFlag)
{
  void *str = rmlA0;
  //int level = 1;
  int level = (int)RML_IMMEDIATE(RML_UNTAGFIXNUM(rmlA1));
  char *strdata = RML_STRINGDATA(str);
  level = set_debug_flag(strdata,level);
  rmlA0 = RML_PRIM_MKBOOL(level);
  RML_TAILCALLK(rmlSC);
}
RML_END_LABEL

RML_BEGIN_LABEL(RTOpts__debugFlag)
{
    void *str = rmlA0;
    char *strdata = RML_STRINGDATA(str);
    int flg = check_debug_flag(strdata);

    /*
    int flg=0;
    int i;
    if (strcmp(strdata,"none")==0 && debug_none == 1) {
      flg=1;
    }
    if (debug_all==1) {
      flg=1;
    }
    for (i=0; i<debug_flagc; i++) {
      if (strcmp(debug_flags[i], strdata)==0) {
	flg=1;
	break;
      }
      else if (debug_flags[i][0]=='-' &&
	       strcmp(debug_flags[i]+1, strdata)==0) {
	flg=0;
	break;
      }
    }
    if (flg==1 && debug_flag_info==1)
      fprintf(stdout, "--------- %s ---------\n", strdata);
    */

    rmlA0 = RML_PRIM_MKBOOL(flg);
    RML_TAILCALLK(rmlSC);
}
RML_END_LABEL

RML_BEGIN_LABEL(RTOpts__noProc)
{
  rmlA0 = (void*)mk_icon(nproc);
  RML_TAILCALLK(rmlSC);
}
RML_END_LABEL

RML_BEGIN_LABEL(RTOpts__latency)
{
  rmlA0 = (void*)mk_rcon(latency);
  RML_TAILCALLK(rmlSC);
}
RML_END_LABEL

RML_BEGIN_LABEL(RTOpts__bandwidth)
{
  rmlA0 = (void*)mk_rcon(bandwidth);
  RML_TAILCALLK(rmlSC);
}
RML_END_LABEL

RML_BEGIN_LABEL(RTOpts__simulationCg)
{
  rmlA0 = RML_PRIM_MKBOOL(simulation_cg);
  RML_TAILCALLK(rmlSC);
}
RML_END_LABEL

RML_BEGIN_LABEL(RTOpts__simulationCodeTarget)
{
  rmlA0 = mk_scon(simulation_code_target);
  RML_TAILCALLK(rmlSC);
}
RML_END_LABEL


RML_BEGIN_LABEL(RTOpts__versionRequest)
{
  rmlA0 = RML_PRIM_MKBOOL(version_request);
  RML_TAILCALLK(rmlSC);
}
RML_END_LABEL

/*
 * adrpo 2007-06-11
 * flag for accepting only Modelica grammar or also MetaModelica grammar
 */
RML_BEGIN_LABEL(RTOpts__acceptMetaModelicaGrammar)
{
    if (acceptedGrammar == GRAMMAR_METAMODELICA)
        rmlA0 = RML_PRIM_MKBOOL(RML_TRUE);
    else
        rmlA0 = RML_PRIM_MKBOOL(RML_FALSE);
  RML_TAILCALLK(rmlSC);
}
RML_END_LABEL

/*
 * adrpo 2008-11-28
 */
RML_BEGIN_LABEL(RTOpts__getAnnotationVersion)
{
  rmlA0 = mk_scon(annotation_version);
  RML_TAILCALLK(rmlSC);
}
RML_END_LABEL

/*
 * adrpo 2008-11-28
 */
RML_BEGIN_LABEL(RTOpts__setAnnotationVersion)
{
  char* str = strdup(RML_STRINGDATA(rmlA0));
  if (strcmp(annotation_version, "1.x") == 0 ||
      strcmp(annotation_version, "2.x") == 0 ||
      strcmp(annotation_version, "3.x") == 0)
  {
    annotation_version = str;
    RML_TAILCALLK(rmlSC);
  }
  RML_TAILCALLK(rmlFC);
}
RML_END_LABEL
