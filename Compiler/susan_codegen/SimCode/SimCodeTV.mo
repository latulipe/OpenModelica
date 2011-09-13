interface package SimCodeTV 

package builtin
  
  function listReverse
    replaceable type TypeVar subtypeof Any;    
    input list<TypeVar> lst;
    output list<TypeVar> result;
  end listReverse;

  function listLength "Return the length of the list"
    replaceable type TypeVar subtypeof Any;    
    input list<TypeVar> lst;
    output Integer result;
  end listLength;
  
  function listNth
    replaceable type TypeVar subtypeof Any;    
    input list<TypeVar> lst;
    input Integer ix;
    output TypeVar result;
  end listNth;
  
  function intAdd
    input Integer a;
    input Integer b;
    output Integer c;
  end intAdd;

  function intMul
    input Integer a;
    input Integer b;
    output Integer c;
  end intMul;

  function intDiv
    input Integer a;
    input Integer b;
    output Integer c;
  end intDiv;

  function arrayList 
    replaceable type TypeVar subtypeof Any;    
    input TypeVar[:] arr;
    output list<TypeVar> lst;
  end arrayList;
 
end builtin;


package SimCode

  type ExtConstructor = tuple<DAE.ComponentRef, String, list<DAE.Exp>>;
  type ExtDestructor = tuple<String, DAE.ComponentRef>;
  type ExtAlias = tuple<DAE.ComponentRef, DAE.ComponentRef>;
  type HelpVarInfo = tuple<Integer, DAE.Exp, Integer>;
  type SampleCondition = tuple<DAE.Exp,Integer>; // helpvarindex, expression,
  type JacobianColumn = tuple<list<SimEqSystem>, list<SimVar>, String>;
  type JacobianMatrix = tuple<list<JacobianColumn>,list<SimVar>,String>;
  
  uniontype SimCode
    record SIMCODE
      ModelInfo modelInfo;
      list<DAE.Exp> literals;
      list<RecordDeclaration> recordDecls;
      list<String> externalFunctionIncludes;
      list<SimEqSystem> allEquations;
      list<SimEqSystem> odeEquations;
      list<SimEqSystem> algebraicEquations;
      list<SimEqSystem> residualEquations;
      list<SimEqSystem> initialEquations;
      list<SimEqSystem> parameterEquations;
      list<SimEqSystem> removedEquations;
      list<DAE.Statement> algorithmAndEquationAsserts;
      list<BackendDAE.ZeroCrossing> zeroCrossings;
      list<list<SimVar>> zeroCrossingsNeedSave;
      list<SampleCondition> sampleConditions;
      list<SimEqSystem> sampleEquations;
      list<HelpVarInfo> helpVarInfo;
      list<SimWhenClause> whenClauses;
      list<DAE.ComponentRef> discreteModelVars;
      ExtObjInfo extObjInfo;
      MakefileParams makefileParams;
      DelayedExpression delayedExps;
      list<JacobianMatrix> jacobianMatrixes;  
      Option<SimulationSettings> simulationSettingsOpt;
      String fileNamePrefix;
    end SIMCODE;
  end SimCode;

  uniontype DelayedExpression
    record DELAYED_EXPRESSIONS
      list<tuple<Integer, DAE.Exp>> delayedExps;
      Integer maxDelayedIndex;
    end DELAYED_EXPRESSIONS;
  end DelayedExpression;

  uniontype FunctionCode
    record FUNCTIONCODE
      String name;
      Option<Function> mainFunction;
      list<Function> functions;
      list<DAE.Exp> literals;
      list<String> externalFunctionIncludes;
      MakefileParams makefileParams;
      list<RecordDeclaration> extraRecordDecls;
    end FUNCTIONCODE;
  end FunctionCode;

  uniontype MakefileParams
    record MAKEFILE_PARAMS
      String ccompiler;
      String cxxcompiler;
      String linker;
      String exeext;
      String dllext;
      String omhome;
      String cflags;
      String ldflags;
      String senddatalibs;
      list<String> includes;
      list<String> libs;
      String platform;
    end MAKEFILE_PARAMS;
  end MakefileParams;
  
  uniontype SimulationSettings
    record SIMULATION_SETTINGS
      Real startTime;
      Real stopTime;
      Integer numberOfIntervals;
      Real stepSize;
      Real tolerance;
      String method;
      String options;
      String outputFormat;
      String variableFilter;
      Boolean measureTime;
      String cflags;
    end SIMULATION_SETTINGS;
  end SimulationSettings;
  
  uniontype Context
    record SIMULATION
      Boolean genDiscrete;
    end SIMULATION;
    record FUNCTION_CONTEXT
    end FUNCTION_CONTEXT;
    record OTHER
    end OTHER;
    record INLINE_CONTEXT
    end INLINE_CONTEXT;
  end Context;
  
  uniontype Variable
    record VARIABLE
      DAE.ComponentRef name;
      DAE.ExpType ty;
      Option<DAE.Exp> value;
      list<DAE.Exp> instDims;
    end VARIABLE;  

    record FUNCTION_PTR
      String name;
      list<DAE.ExpType> tys;
      list<Variable> args;
    end FUNCTION_PTR;
  end Variable;
  
  uniontype Statement
    record ALGORITHM
       list<DAE.Statement> statementLst;
    end ALGORITHM;
  end Statement;

  uniontype ExtObjInfo
    record EXTOBJINFO
      list<SimVar> vars;
      list<ExtAlias> aliases;
    end EXTOBJINFO;
  end ExtObjInfo;
  
  uniontype SimEqSystem
    record SES_RESIDUAL
      DAE.Exp exp;
      DAE.ElementSource source;
    end SES_RESIDUAL;
    record SES_SIMPLE_ASSIGN
      DAE.ComponentRef cref;
      DAE.Exp exp;
      DAE.ElementSource source;
    end SES_SIMPLE_ASSIGN;
    record SES_ARRAY_CALL_ASSIGN
      DAE.ComponentRef componentRef;
      DAE.Exp exp;
      DAE.ElementSource source;
    end SES_ARRAY_CALL_ASSIGN;
    record SES_ALGORITHM
      list<DAE.Statement> statements;
    end SES_ALGORITHM;
    record SES_LINEAR
      Integer index;
      Boolean partOfMixed;
      list<SimVar> vars;
      list<DAE.Exp> beqs;
      list<tuple<Integer, Integer, SimEqSystem>> simJac;
    end SES_LINEAR;
    record SES_NONLINEAR
      Integer index;
      list<SimEqSystem> eqs;
      list<DAE.ComponentRef> crefs;
    end SES_NONLINEAR;
    record SES_MIXED
      Integer index;
      SimEqSystem cont;
      list<SimVar> discVars;
      list<SimEqSystem> discEqs;
      list<Integer> values;
      list<Integer> value_dims;
    end SES_MIXED;
    record SES_WHEN
      DAE.ComponentRef left;
      DAE.Exp right;
      list<tuple<DAE.Exp, Integer>> conditions;
      Option<SimEqSystem> elseWhen;
      DAE.ElementSource source;
    end SES_WHEN;
  end SimEqSystem;

  uniontype SimWhenClause
    record SIM_WHEN_CLAUSE
      list<DAE.ComponentRef> conditionVars;
      list<BackendDAE.WhenOperator> reinits;
      Option<BackendDAE.WhenEquation> whenEq;
      list<tuple<DAE.Exp, Integer>> conditions;
    end SIM_WHEN_CLAUSE;
  end SimWhenClause;

  uniontype ModelInfo
    record MODELINFO
      Absyn.Path name;
      String directory;
      VarInfo varInfo;
      SimVars vars;
      list<Function> functions;
    end MODELINFO;
  end ModelInfo;
  
  uniontype FunctionInfo
    record FUNCTIONINFO
      Absyn.Path name;
      Absyn.Info info;
    end FUNCTIONINFO;
  end FunctionInfo;

  uniontype VarInfo
	  record VARINFO
      Integer numHelpVars;
      Integer numZeroCrossings;
      Integer numTimeEvents;
      Integer numStateVars;
      Integer numAlgVars;
      Integer numIntAlgVars;
      Integer numBoolAlgVars;
      Integer numAlgAliasVars;
      Integer numIntAliasVars;
      Integer numBoolAliasVars;
      Integer numParams;
      Integer numIntParams;
      Integer numBoolParams;
      Integer numOutVars;
      Integer numInVars;
      Integer numResiduals;
      Integer numExternalObjects;
      Integer numStringAlgVars;
      Integer numStringParamVars;
      Integer numStringAliasVars;
      Integer numJacobianVars;
      Option <Integer> dimODE1stOrder;
      Option <Integer> dimODE2ndOrder; 
	  end VARINFO;
	end VarInfo;
	
	uniontype SimVars
	  record SIMVARS
	    list<SimVar> stateVars;
	    list<SimVar> derivativeVars;
	    list<SimVar> algVars;
	    list<SimVar> intAlgVars;
	    list<SimVar> boolAlgVars;
	    list<SimVar> inputVars;
	    list<SimVar> outputVars;
      list<SimVar> aliasVars;
      list<SimVar> intAliasVars;
      list<SimVar> boolAliasVars;	    
	    list<SimVar> paramVars;
	    list<SimVar> intParamVars;
	    list<SimVar> boolParamVars;
	    list<SimVar> stringAlgVars;
	    list<SimVar> stringParamVars;
	    list<SimVar> stringAliasVars;
	    list<SimVar> extObjVars;
	    list<SimVar> jacobianVars; //all vars for the matrices A,B,C,D
	    list<SimVar> constVars;
	    list<SimVar> intConstVars;
      list<SimVar> boolConstVars;
      list<SimVar> stringConstVars;
	  end SIMVARS;
	end SimVars;
  
  uniontype SimVar
    record SIMVAR
      DAE.ComponentRef name;
      BackendDAE.VarKind varKind;
      String comment;
      String unit;
      String displayUnit;
      Integer index;
      Option<DAE.Exp> initialValue;
      Option<DAE.Exp> nominalValue;
      Boolean isFixed;
      DAE.ExpType type_;
      Boolean isDiscrete;
      Option<DAE.ComponentRef> arrayCref;
      AliasVariable aliasvar;
      DAE.ElementSource source;
      Causality causality;
      Option<Integer> variable_index;
      list <String> numArrayElement;
    end SIMVAR;
  end SimVar;
  
  uniontype AliasVariable
    record NOALIAS end NOALIAS;
    record ALIAS 
      DAE.ComponentRef varName;
    end ALIAS;
    record NEGATEDALIAS 
      DAE.ComponentRef varName;
    end NEGATEDALIAS;
  end AliasVariable;
  
  uniontype Causality
    record NONECAUS end NONECAUS;
    record INTERNAL end INTERNAL;
    record OUTPUT end OUTPUT;
    record INPUT end INPUT;
  end Causality;  
  
  uniontype Function
    record FUNCTION    
      Absyn.Path name;
      list<Variable> inVars;
      list<Variable> outVars;
      list<Variable> functionArguments;
      list<Variable> variableDeclarations;
      list<Statement> body;
      Absyn.Info info;
    end FUNCTION;
    record EXTERNAL_FUNCTION
      Absyn.Path name;
      String extName;
      list<Variable> funArgs;
      list<SimExtArg> extArgs;
      SimExtArg extReturn;
      list<Variable> inVars;
      list<Variable> outVars;
      list<Variable> biVars;
      list<String> libs;
      String language;
      Absyn.Info info;
      Boolean dynamicLoad;
    end EXTERNAL_FUNCTION;
    record RECORD_CONSTRUCTOR
      Absyn.Path name;
      list<Variable> funArgs;
      Absyn.Info info;
    end RECORD_CONSTRUCTOR;
  end Function;
  
  uniontype RecordDeclaration
    record RECORD_DECL_FULL
      String name;
      Absyn.Path defPath;
      list<Variable> variables;
    end RECORD_DECL_FULL;
    record RECORD_DECL_DEF
      Absyn.Path path;
      list<String> fieldNames;
    end RECORD_DECL_DEF;
  end RecordDeclaration;

  uniontype SimExtArg
    record SIMEXTARG
      DAE.ComponentRef cref;
      Boolean isInput;
      Integer outputIndex;
      Boolean isArray;
      Boolean hasBinding;
      DAE.ExpType type_;
    end SIMEXTARG;
    record SIMEXTARGEXP
      DAE.Exp exp;
      DAE.ExpType type_;
    end SIMEXTARGEXP;
    record SIMEXTARGSIZE
      DAE.ComponentRef cref;
      Boolean isInput;
      Integer outputIndex;
      DAE.ExpType type_;
      DAE.Exp exp;
    end SIMEXTARGSIZE;
    record SIMNOEXTARG end SIMNOEXTARG;
  end SimExtArg;
  
  constant Context contextSimulationNonDiscrete;
  constant Context contextSimulationDiscrete;
  constant Context contextInlineSolver;
  constant Context contextFunction;
  constant Context contextOther;  
  constant list<DAE.Exp> listExpLength1;

  function elementVars
    input list<DAE.Element> ld;
    output list<Variable> vars;
  end elementVars;
  
  function crefSubIsScalar
    input DAE.ComponentRef cref;
    output Boolean isScalar;
  end crefSubIsScalar;
	
  function crefNoSub
    input DAE.ComponentRef cref;
    output Boolean noSub;
  end crefNoSub;

  function crefIsScalar
    input DAE.ComponentRef cref;
		input Context context;
    output Boolean isScalar;
  end crefIsScalar;

  function buildCrefExpFromAsub
    input DAE.Exp cref;
    input list<DAE.Exp> subs;
    output DAE.Exp cRefOut;
  end buildCrefExpFromAsub;

  function incrementInt
    input Integer inInt;
    input Integer increment;
    output Integer outInt;
  end incrementInt;
 
  function decrementInt
    input Integer inInt;
    input Integer decrement;
    output Integer outInt;
  end decrementInt;

  function makeCrefRecordExp
    input DAE.ComponentRef inCRefRecord;
    input DAE.ExpVar inVar;
    output DAE.Exp outExp;
  end makeCrefRecordExp;
   
  function cref2simvar
    input DAE.ComponentRef cref;
    input SimCode simCode;
    output SimVar outSimVar;
  end cref2simvar;
  
  function derComponentRef
    input DAE.ComponentRef inCref;
    output DAE.ComponentRef derCref;
  end derComponentRef;
  
  function hackArrayReverseToCref
    input DAE.Exp inExp;
    input Context context; 
    output DAE.Exp outExp;
  end hackArrayReverseToCref;
  
  function hackGetFirstExternalFunctionLib
    input list<String> libs;
    output String outFirstLib;
  end hackGetFirstExternalFunctionLib;

	function hackMatrixReverseToCref
	  input DAE.Exp inExp;
	  input Context context; 
	  output DAE.Exp outExp;
	end hackMatrixReverseToCref;
	
	function createArray
	  input Integer size;
	  output DAE.Exp outArray;
	end createArray;
	
	function createAssertforSqrt
	  input DAE.Exp inExp;
	  output DAE.Exp outExp;
	end createAssertforSqrt;

    function appendAllequations
      input list<JacobianMatrix> inJacobianMatrix;
      output list<SimEqSystem> eqn;
    end appendAllequations;
 
    function appendLists
      input list<SimEqSystem> inEqn1;
      input list<SimEqSystem> inEqn2;
      output list<SimEqSystem> outEqn;
    end appendLists;
 
    function createDAEString
      input String inString;
      output DAE.Exp outExp;
    end createDAEString;
  
    function isBoxedFunction
      input Function fn;
      output Boolean b;
    end isBoxedFunction;
    
    function functionInfo
      input Function fn;
      output Absyn.Info info;
    end functionInfo;
  
    function twodigit
      input Integer i;
      output String s;
    end twodigit;

   function countDynamicExternalFunctions
      input list<Function> inFncLst;
      output Integer outDynLoadFuncs;     
   end countDynamicExternalFunctions;

end SimCode;


package BackendDAE

  uniontype VarKind "- Variable kind"
    record VARIABLE end VARIABLE;
    record STATE end STATE;
    record STATE_DER end STATE_DER;
    record DUMMY_DER end DUMMY_DER;
    record DUMMY_STATE end DUMMY_STATE;
    record DISCRETE end DISCRETE;
    record PARAM end PARAM;
    record CONST end CONST;
    record EXTOBJ Absyn.Path fullClassName; end EXTOBJ;
  end VarKind;

  uniontype ZeroCrossing
    record ZERO_CROSSING
      DAE.Exp relation_;
    end ZERO_CROSSING;
  end ZeroCrossing;
  
  uniontype WhenOperator "- Reinit Statement"
    record REINIT
      DAE.ComponentRef stateVar "State variable to reinit" ;
      DAE.Exp value             "Value after reinit" ;
      DAE.ElementSource source "origin of equation";
    end REINIT;

    record ASSERT
      DAE.Exp condition;
      DAE.Exp message;
      DAE.ElementSource source "the origin of the component/equation/algorithm";
    end ASSERT;  
  
    record TERMINATE " The Modelica builtin terminate(msg)"
      DAE.Exp message;
      DAE.ElementSource source "the origin of the component/equation/algorithm";
    end TERMINATE; 
  end WhenOperator;

  uniontype WhenEquation
    record WHEN_EQ
      Integer index;
      DAE.ComponentRef left;
      DAE.Exp right;
      Option<WhenEquation> elsewhenPart;
    end WHEN_EQ;
  end WhenEquation;

  uniontype VarKind
    record EXTOBJ Absyn.Path fullClassName; end EXTOBJ;
  end VarKind;
  
end BackendDAE;

package System

  function stringReplace
    input String str;
    input String source;
    input String target;
    output String res;
  end stringReplace;
  
  function tmpTick
    output Integer tickNo;
  end tmpTick;
  
  function tmpTickReset
    input Integer start;
  end tmpTickReset;

  function tmpTickIndex
    input Integer index;
    output Integer tickNo;
  end tmpTickIndex;
  
  function tmpTickResetIndex
    input Integer start;
    input Integer index;
  end tmpTickResetIndex;

  function getCurrentTimeStr
    output String timeStr;
  end getCurrentTimeStr;

  function getUUIDStr 
    output String uuidStr;
  end getUUIDStr;

  function unescapedStringLength "Return the length of a C string literal"
    input String s;
    output Integer result;
  end unescapedStringLength;

  function stringHashDjb2Mod
    input String str;
    input Integer mod;
    output Integer hash;
  end stringHashDjb2Mod;
  
  function escapedString
    input String unescapedString;
    output String escapedString;
  end escapedString;

  function unquoteIdentifier
    input String str;
    output String outStr;
  end unquoteIdentifier;

  function os
    output String str;
  end os;
end System;


package Tpl
  function textFile
    input Text inText;
    input String inFileName;
  end textFile;
  
  //we do not import Error.addSourceMessage() directly
  //because of list creation in Susan is not possible (yet by design)  
  function addSourceTemplateError
    "Wraps call to Error.addSourceMessage() funtion with Error.TEMPLATE_ERROR and one MessageToken."
    input String inErrMsg;
    input Absyn.Info inInfo;
  end addSourceTemplateError;
  
  //for completeness; although the addSourceTemplateError() above is preferable
  function addTemplateError
  "Wraps call to Error.addMessage() funtion with Error.TEMPLATE_ERROR and one MessageToken."
    input String inErrMsg;  
  end addTemplateError;
end Tpl;


package Absyn

  type Ident = String;
  
  uniontype Path
    record QUALIFIED
      Ident name;
      Path path;
    end QUALIFIED;
    record IDENT
      String name;
    end IDENT;
    record FULLYQUALIFIED
      Path path;
    end FULLYQUALIFIED;
  end Path;
  
  uniontype Info
    record INFO
      String fileName;
      Boolean isReadOnly;
      Integer lineNumberStart;
      Integer columnNumberStart;
      Integer lineNumberEnd;
      Integer columnNumberEnd;
    end INFO;
  end Info;
  
  uniontype Within "Within Clauses"
    record WITHIN "the within clause"
      Path path "the path for within";
    end WITHIN;

    record TOP end TOP;

  end Within;

  constant Info dummyInfo;
end Absyn;


package DAE

  type Type = tuple<TType, Option<Absyn.Path>>;
  type Ident = String;

  uniontype ExpType
    record ET_INT end ET_INT;
    record ET_REAL end ET_REAL;
    record ET_BOOL end ET_BOOL;
    record ET_STRING end ET_STRING;
    record ET_ENUMERATION
      Absyn.Path path;
      list<String> names;
      list<ExpVar> varLst;
    end ET_ENUMERATION;
    record ET_COMPLEX
      Absyn.Path name;
      list<ExpVar> varLst; 
      ClassInf.State complexClassType;
    end ET_COMPLEX;
    record ET_OTHER end ET_OTHER;
    record ET_ARRAY
      ExpType ty;
      list<DAE.Dimension> arrayDimensions;
    end ET_ARRAY;
    record ET_METATYPE end ET_METATYPE;
    record ET_BOXED ExpType ty; end ET_BOXED;
    record ET_FUNCTION_REFERENCE_VAR end ET_FUNCTION_REFERENCE_VAR;
    record ET_FUNCTION_REFERENCE_FUNC
      Boolean builtin;
    end ET_FUNCTION_REFERENCE_FUNC;
    record ET_NORETCALL end ET_NORETCALL;
  end ExpType;

  uniontype ExpVar
    record COMPLEX_VAR
      String name;
      ExpType tp;
    end COMPLEX_VAR;
  end ExpVar;

  uniontype Exp
    record ICONST
      Integer integer;
    end ICONST;
    record RCONST
      Real real;
    end RCONST;
    record SCONST
      String string;
    end SCONST;
    record BCONST
      Boolean bool;
    end BCONST;
    record ENUM_LITERAL
      Absyn.Path name;
      Integer index;
    end ENUM_LITERAL;
    record CREF
      ComponentRef componentRef;
      ExpType ty;
    end CREF;
    record BINARY
      Exp exp1;
      Operator operator;
      Exp exp2; 
    end BINARY;
    record UNARY
      Operator operator;
      Exp exp; 
    end UNARY;
    record LBINARY
      Exp exp1;
      Operator operator;
      Exp exp2; 
    end LBINARY;
    record LUNARY
      Operator operator;
      Exp exp; 
    end LUNARY;
    record RELATION
      Exp exp1;
      Operator operator;
      Exp exp2;
      Integer index;
      Option<tuple<DAE.Exp,Integer,Integer>> optionExpisASUB; 
    end RELATION;
    record IFEXP
      Exp expCond;
      Exp expThen;
      Exp expElse;
    end IFEXP;
    record CALL
      Absyn.Path path;
      list<Exp> expLst;
      CallAttributes attr;
    end CALL;
    record ARRAY
      ExpType ty;
      Boolean scalar;
      list<Exp> array;
    end ARRAY;
    record MATRIX
      ExpType ty;
      Integer integer;
      Boolean scalar;
      list<list<Exp>> matrix;
    end MATRIX;
    record RANGE
      ExpType ty;
      Exp exp;
      Option<Exp> expOption;
      Exp range;
    end RANGE;
    record TUPLE
      list<Exp> PR;
    end TUPLE;
    record CAST
      ExpType ty;
      Exp exp;
    end CAST;
    record ASUB
      Exp exp;
      list<Exp> sub;
    end ASUB;
    record TSUB
      Exp exp;
      Integer index;
      ExpType ty;
    end TSUB;
    record SIZE
      Exp exp;
      Option<Exp> sz;
    end SIZE;
    record CODE
      Absyn.CodeNode code;
      ExpType ty;
    end CODE;
    record REDUCTION
      ReductionInfo reductionInfo;
      Exp expr;
      ReductionIterators iterators;
    end REDUCTION;
    record LIST
      list<Exp> valList;
    end LIST;
    record CONS
      Exp car;
      Exp cdr;
    end CONS;
    record META_TUPLE
      list<Exp> listExp;
    end META_TUPLE;
    record META_OPTION
      Option<Exp> exp;
    end META_OPTION;
    record METARECORDCALL
      Absyn.Path path;
      list<Exp> args;
      list<String> fieldNames;
      Integer index;
    end METARECORDCALL;
    record MATCHEXPRESSION
      DAE.MatchType matchType;
      list<Exp> inputs;
      list<Element> localDecls;
      list<MatchCase> cases;
      ExpType et;
    end MATCHEXPRESSION;
    record BOX
      Exp exp;
    end BOX;
    record UNBOX
      Exp exp;
      ExpType ty;
    end UNBOX;
    record SHARED_LITERAL
      Integer index;
      ExpType ty;
    end SHARED_LITERAL;
    record PATTERN
      Pattern pattern;
    end PATTERN;
  end Exp;
  
  uniontype CallAttributes
    record CALL_ATTR
      ExpType ty "The type of the return value, if several return values this is undefined";
      Boolean tuple_ "tuple" ;
      Boolean builtin "builtin Function call" ;
      InlineType inlineType;
      TailCall tailCall "Input variables of the function if the call is tail-recursive";
    end CALL_ATTR;
  end CallAttributes;

  uniontype ReductionIterator
    record REDUCTIONITER
      String id;
      Exp exp;
      Option<Exp> guardExp;
      Type ty;
    end REDUCTIONITER;
  end ReductionIterator;

  type ReductionIterators = list<ReductionIterator>;
  
  uniontype ReductionInfo
    record REDUCTIONINFO "A separate uniontype containing the information not required by traverseExp, etc"
      Absyn.Path path "array, sum,..";
      Type exprType;
      Option<Values.Value> defaultValue "if there is no default value, the reduction is not defined for 0-length arrays/lists";
      Option<Exp> foldExp "For example, max(ident,$res) or ident+$res; array() does not use this feature; DO NOT TRAVERSE THIS EXPRESSION!";
    end REDUCTIONINFO;
  end ReductionInfo;

  uniontype MatchCase
    record CASE
      list<Pattern> patterns "ELSE is handled by not doing pattern-matching";
      list<Element> localDecls;
      list<Statement> body;
      Option<Exp> result;
      Absyn.Info resultInfo;
      Integer jump;
    end CASE;
  end MatchCase;

  uniontype Pattern "Patterns deconstruct expressions"
    record PAT_WILD "_"
    end PAT_WILD;
    record PAT_CONSTANT "compare to this constant value using equality"
      Option<ExpType> ty "so we can unbox if needed";
      Exp exp;
    end PAT_CONSTANT;
    record PAT_AS "id as pat"
      String id;
      Option<ExpType> ty;
      Pattern pat;
    end PAT_AS;
    record PAT_AS_FUNC_PTR "id as pat"
      String id;
      Pattern pat;
    end PAT_AS_FUNC_PTR;
    record PAT_META_TUPLE "(pat1,...,patn)"
      list<Pattern> patterns;
    end PAT_META_TUPLE;
    record PAT_CALL_TUPLE "(pat1,...,patn)"
      list<Pattern> patterns;
    end PAT_CALL_TUPLE;
    record PAT_CONS "head::tail"
      Pattern head;
      Pattern tail;
    end PAT_CONS;
    record PAT_CALL "RECORD(pat1,...,patn); all patterns are positional"
      Absyn.Path name;
      Integer index;
      list<Pattern> patterns;
      Boolean knownSingleton;
    end PAT_CALL;
    record PAT_CALL_NAMED "RECORD(pat1,...,patn); all patterns are named"
      Absyn.Path name;
      list<tuple<Pattern,String,ExpType>> patterns;
    end PAT_CALL_NAMED;
    record PAT_SOME "SOME(pat)"
      Pattern pat;
    end PAT_SOME;
  end Pattern;

  uniontype ComponentRef
    record CREF_QUAL
      Ident ident;
      ExpType identType;
      list<Subscript> subscriptLst;
      ComponentRef componentRef;
    end CREF_QUAL;
    record CREF_IDENT
      Ident ident;
      ExpType identType;
      list<Subscript> subscriptLst;
    end CREF_IDENT;
    record WILD end WILD;
  end ComponentRef;

  uniontype Operator
    record ADD
      ExpType ty;
    end ADD;
    record SUB
      ExpType ty;
    end SUB;
    record MUL
      ExpType ty;
    end MUL;
    record DIV
      ExpType ty;
    end DIV;
    record POW
      ExpType ty;
    end POW;
    record UMINUS
      ExpType ty;
    end UMINUS;
    record UPLUS
      ExpType ty;
    end UPLUS;
    record UMINUS_ARR
      ExpType ty;
    end UMINUS_ARR;
    record UPLUS_ARR
      ExpType ty;
    end UPLUS_ARR;
    record ADD_ARR
      ExpType ty;
    end ADD_ARR;
    record SUB_ARR
      ExpType ty;
    end SUB_ARR;
    record MUL_ARR
      ExpType ty;
    end MUL_ARR;
    record DIV_ARR
      ExpType ty;
    end DIV_ARR;
    record MUL_SCALAR_ARRAY
      ExpType ty;
    end MUL_SCALAR_ARRAY;
    record MUL_ARRAY_SCALAR
      ExpType ty;
    end MUL_ARRAY_SCALAR;
    record ADD_SCALAR_ARRAY
      ExpType ty;
    end ADD_SCALAR_ARRAY;
    record ADD_ARRAY_SCALAR
      ExpType ty;
    end ADD_ARRAY_SCALAR;
    record SUB_SCALAR_ARRAY
      ExpType ty;
    end SUB_SCALAR_ARRAY;
    record SUB_ARRAY_SCALAR
      ExpType ty;
    end SUB_ARRAY_SCALAR;
    record MUL_SCALAR_PRODUCT
      ExpType ty;
    end MUL_SCALAR_PRODUCT;
    record MUL_MATRIX_PRODUCT
      ExpType ty;
    end MUL_MATRIX_PRODUCT;
    record DIV_ARRAY_SCALAR
      ExpType ty;
    end DIV_ARRAY_SCALAR;
    record DIV_SCALAR_ARRAY
      ExpType ty;
    end DIV_SCALAR_ARRAY;
    record POW_ARRAY_SCALAR
      ExpType ty;
    end POW_ARRAY_SCALAR;
    record POW_SCALAR_ARRAY
      ExpType ty;
    end POW_SCALAR_ARRAY;
    record POW_ARR
      ExpType ty;
    end POW_ARR;
    record POW_ARR2
      ExpType ty;
    end POW_ARR2;
    record AND 
      ExpType ty;
    end AND;
    record OR 
      ExpType ty;
    end OR;
    record NOT 
      ExpType ty;
    end NOT;
    record LESS
      ExpType ty;
    end LESS;
    record LESSEQ
      ExpType ty;
    end LESSEQ;
    record GREATER
      ExpType ty;
    end GREATER;
    record GREATEREQ
      ExpType ty;
    end GREATEREQ;
    record EQUAL
      ExpType ty;
    end EQUAL;
    record NEQUAL
      ExpType ty;
    end NEQUAL;
    record USERDEFINED
      Absyn.Path fqName;
    end USERDEFINED;
  end Operator;
  
  uniontype Statement
    record STMT_ASSIGN
      ExpType type_;
      Exp exp1;
      Exp exp;
      ElementSource source;
    end STMT_ASSIGN;
    record STMT_ASSIGN_ARR
      ExpType type_;
      ComponentRef componentRef;
      Exp exp;
      ElementSource source;
    end STMT_ASSIGN_ARR;
    record STMT_TUPLE_ASSIGN
      ExpType type_;
      list<Exp> expExpLst;
      Exp exp;
      ElementSource source;
    end STMT_TUPLE_ASSIGN;
    record STMT_IF
      Exp exp;
      list<Statement> statementLst;
      Else else_;
      ElementSource source;
    end STMT_IF;
    record STMT_FOR
      ExpType type_;
      Boolean iterIsArray;
      Ident iter;
      Exp range;
      list<Statement> statementLst;
      ElementSource source;
    end STMT_FOR;
    record STMT_WHILE
      Exp exp;
      list<Statement> statementLst;
      ElementSource source;
    end STMT_WHILE;
    record STMT_WHEN
      Exp exp;
      list<Statement> statementLst;
      Option<Statement> elseWhen;
      list<Integer> helpVarIndices;
      ElementSource source;
    end STMT_WHEN;
    record STMT_TERMINATE
      Exp msg;
      ElementSource source;
    end STMT_TERMINATE;  
    record STMT_ASSERT
      Exp cond;
      Exp msg;
      ElementSource source;
    end STMT_ASSERT;
    record STMT_REINIT
      Exp var;
      Exp value;
      ElementSource source;
    end STMT_REINIT;  
    record STMT_RETURN
      ElementSource source;
    end STMT_RETURN;
    record STMT_BREAK
      ElementSource source;
    end STMT_BREAK;
    record STMT_FAILURE
      list<Statement> body;
      ElementSource source;
    end STMT_FAILURE;
    record STMT_TRY
      list<Statement> tryBody;
      ElementSource source;
    end STMT_TRY;
    record STMT_CATCH
      list<Statement> catchBody;
      ElementSource source;
    end STMT_CATCH;
    record STMT_THROW
      ElementSource source;
    end STMT_THROW;
    record STMT_NORETCALL
      Exp exp;
      ElementSource source;
    end STMT_NORETCALL;
  end Statement;

  uniontype Else
    record NOELSE end NOELSE;
    record ELSEIF
      Exp exp;
      list<Statement> statementLst;
      Else else_;
    end ELSEIF;
    record ELSE
      list<Statement> statementLst;
    end ELSE;
  end Else;

  uniontype Var
    record TYPES_VAR
      Ident name;
      Attributes attributes;
      Boolean protected_;
      Type type_;
      Binding binding;
    end TYPES_VAR;
  end Var;

  uniontype TType
    record T_INTEGER
      list<Var> varLstInt;
    end T_INTEGER;
    record T_REAL
      list<Var> varLstReal;
    end T_REAL;
    record T_STRING
      list<Var> varLstString;
    end T_STRING;
    record T_BOOL
      list<Var> varLstBool;
    end T_BOOL;
    record T_ARRAY
      Dimension arrayDim;
      Type arrayType;
    end T_ARRAY;
    record T_NORETCALL end T_NORETCALL;
    record T_NOTYPE end T_NOTYPE;
    record T_ANYTYPE
      Option<ClassInf.State> anyClassType;
    end T_ANYTYPE;
    record T_COMPLEX
      ClassInf.State complexClassType;
      list<Var> complexVarLst;
      Option<Type> complexTypeOption;
      EqualityConstraint equalityConstraint;
    end T_COMPLEX;
  end TType;

  uniontype Dimension
    record DIM_INTEGER
      Integer integer;
    end DIM_INTEGER;

    record DIM_ENUM
      Absyn.Path enumTypeName;
      list<String> literals;
      Integer size;
    end DIM_ENUM;

    record DIM_UNKNOWN
    end DIM_UNKNOWN;
  end Dimension;

  uniontype Subscript
    record WHOLEDIM end WHOLEDIM;
    record SLICE
      Exp exp;
    end SLICE;
    record INDEX
      Exp exp;
    end INDEX;
  end Subscript;

  uniontype MatchType
    record MATCHCONTINUE end MATCHCONTINUE;
    record MATCH
      Option<tuple<Integer,ExpType,Integer>> switch;
    end MATCH;
  end MatchType;

  uniontype ElementSource
    record SOURCE
      Absyn.Info info;
      list<Absyn.Within> partOfLst;
      list<Option<ComponentRef>> instanceOptLst;
      list<Option<tuple<ComponentRef, ComponentRef>>> connectEquationOptLst;
      list<Absyn.Path> typeLst;
      list<SymbolicOperation> operations;
    end SOURCE;
  end ElementSource;

  uniontype SymbolicOperation
    record SIMPLIFY
      Exp before;
      Exp after;
    end SIMPLIFY;
    record SUBSTITUTION "A chain of substitutions"
      list<Exp> substitutions;
      Exp source;
    end SUBSTITUTION;
    record SOLVE
      ComponentRef cr;
      Exp exp1;
      Exp exp2;
      Exp res;
      list<Exp> assertConds;
    end SOLVE;
    record SOLVED
      ComponentRef cr;
      Exp exp;
    end SOLVED;
    record LINEAR_SOLVED
      list<ComponentRef> vars;
      list<list<Real>> jac;
      list<Real> rhs;
      list<Real> result;
    end LINEAR_SOLVED;
    record OP_INLINE
      Exp before;
      Exp after;
    end OP_INLINE;
    record NEW_DUMMY_DER
      ComponentRef chosen;
      list<ComponentRef> candidates;
    end NEW_DUMMY_DER;
    record OP_DERIVE
      ComponentRef cr;
      Exp before;
      Exp after;
    end OP_DERIVE;
  end SymbolicOperation;

  uniontype TailCall
    record NO_TAIL
    end NO_TAIL;
    record TAIL
      list<String> vars;
    end TAIL;
  end TailCall;
end DAE;


package ClassInf

  uniontype State
    record UNKNOWN
      String string;
    end UNKNOWN;
    record MODEL
      String string;
    end MODEL;
    record RECORD
        Absyn.Path path;
    end RECORD;
    record BLOCK
      String string;
    end BLOCK;
    record CONNECTOR
      String string;
      Boolean isExpandable;
    end CONNECTOR;
    record TYPE
      String string;
    end TYPE;
    record PACKAGE
      String string;
    end PACKAGE;
    record FUNCTION
      String string;
    end FUNCTION;
    record ENUMERATION
      String string;
    end ENUMERATION;
    record HAS_EQUATIONS
      String string;
    end HAS_EQUATIONS;
    record IS_NEW
      String string;
    end IS_NEW;
    record TYPE_INTEGER
      String string;
    end TYPE_INTEGER;
    record TYPE_REAL
      String string;
    end TYPE_REAL;
    record TYPE_STRING
      String string;
    end TYPE_STRING;
    record TYPE_BOOL
      String string;
    end TYPE_BOOL;
    record TYPE_ENUM
      String string;
    end TYPE_ENUM;
    record EXTERNAL_OBJ
      Absyn.Path path;
    end EXTERNAL_OBJ;
  end State;

end ClassInf;

package SCode

type Ident = Absyn.Ident "Some definitions are borrowed from `Absyn\'";

type Path = Absyn.Path;

type Subscript = Absyn.Subscript;

uniontype Restriction
  record R_CLASS end R_CLASS;
  record R_OPTIMIZATION end R_OPTIMIZATION;
  record R_MODEL end R_MODEL;
  record R_RECORD end R_RECORD;
  record R_BLOCK end R_BLOCK;
  record R_CONNECTOR "a connector"
    Boolean isExpandable "is expandable?";
  end R_CONNECTOR;
  record R_OPERATOR "an operator definition"
    Boolean isFunction "is this operator a function?";
  end R_OPERATOR;
  record R_TYPE end R_TYPE;
  record R_PACKAGE end R_PACKAGE;
  record R_FUNCTION end R_FUNCTION;
  record R_EXT_FUNCTION "Added c.t. Absyn" end R_EXT_FUNCTION;
  record R_ENUMERATION end R_ENUMERATION;

  // predefined internal types
  record R_PREDEFINED_INTEGER     "predefined IntegerType" end R_PREDEFINED_INTEGER;
  record R_PREDEFINED_REAL        "predefined RealType"    end R_PREDEFINED_REAL;
  record R_PREDEFINED_STRING      "predefined StringType"  end R_PREDEFINED_STRING;
  record R_PREDEFINED_BOOLEAN     "predefined BooleanType" end R_PREDEFINED_BOOLEAN;
  record R_PREDEFINED_ENUMERATION "predefined EnumType"    end R_PREDEFINED_ENUMERATION;

  // MetaModelica extensions
  record R_METARECORD "Metamodelica extension"
    Absyn.Path name; //Name of the uniontype
    Integer index; //Index in the uniontype
  end R_METARECORD; /* added by x07simbj */

  record R_UNIONTYPE "Metamodelica extension"
  end R_UNIONTYPE; /* added by simbj */
end Restriction;

uniontype Mod "- Modifications"

  record MOD
    Final finalPrefix "final prefix";    
    Each  eachPrefix "each prefix";    
    list<SubMod> subModLst;
    Option<tuple<Absyn.Exp,Boolean>> binding "The binding expression of a modification
    has an expression and a Boolean delayElaboration which is true if elaboration(type checking)
    should be delayed. This can for instance be used when having A a(x = a.y) where a.y can not be
    type checked -before- a is instantiated, which is the current design in instantiation process.";
  end MOD;

  record REDECL
    Final         finalPrefix "final prefix";    
    Each          eachPrefix "each prefix";    
    list<Element> elementLst  "elements";
  end REDECL;

  record NOMOD end NOMOD;

end Mod;

uniontype SubMod "Modifications are represented in an more structured way than in
    the `Absyn\' module.  Modifications using qualified names
    (such as in `x.y =  z\') are normalized (to `x(y = z)\').  And a
    special case when arrays are subscripted in a modification.
"
  record NAMEMOD
    Ident ident;
    Mod A "A named component" ;
  end NAMEMOD;

  record IDXMOD
    list<Subscript> subscriptLst;
    Mod an "An array element" ;
  end IDXMOD;

end SubMod;

type Program = list<Element> "- Programs
As in the AST, a program is simply a list of class definitions.";

uniontype Enum "Enum, which is a name in an enumeration and an optional Comment."
  record ENUM
    Ident           literal;
    Option<Comment> comment;
  end ENUM;
end Enum;

uniontype ClassDef
"The major difference between these types and their Absyn
 counterparts is that the PARTS constructor contains separate
 lists for elements, equations and algorithms.

 SCode.PARTS contains elements of a class definition. For instance,
    model A
      extends B;
      C c;
    end A;
 Here PARTS contains two elements ('extends B' and 'C c')
 SCode.DERIVED is used for short class definitions, i.e:
  class A = B(modifiers);
 SCode.CLASS_EXTENDS is used for extended class definition, i.e:
  class extends A (modifier)
    new elements;
  end A;"
  
  record PARTS "a class made of parts"
    list<Element>              elementLst          "the list of elements";
    list<Equation>             normalEquationLst   "the list of equations";
    list<Equation>             initialEquationLst  "the list of initial equations";
    list<AlgorithmSection>     normalAlgorithmLst  "the list of algorithms";
    list<AlgorithmSection>     initialAlgorithmLst "the list of initial algorithms";
    Option<Absyn.ExternalDecl> externalDecl        "used by external functions";
    list<Annotation>           annotationLst       "the list of annotations found in between class elements, equations and algorithms";
    Option<Comment>            comment             "the class comment";
  end PARTS;

  record CLASS_EXTENDS "an extended class definition plus the additional parts"
    Ident                      baseClassName       "the name of the base class we have to extend";
    Mod                        modifications       "the modifications that need to be applied to the base class";
    ClassDef                   composition         "the new composition";
  end CLASS_EXTENDS;

  record DERIVED "a derived class"
    Absyn.TypeSpec typeSpec "typeSpec: type specification" ;
    Mod modifications       "the modifications";
    Attributes attributes   "the element attributes";
    Option<Comment> comment "the translated comment from the Absyn";
  end DERIVED;

  record ENUMERATION "an enumeration"
    list<Enum> enumLst      "if the list is empty it means :, the supertype of all enumerations";
    Option<Comment> comment "the translated comment from the Absyn";
  end ENUMERATION;

  record OVERLOAD "an overloaded function"
    list<Absyn.Path> pathLst "the path lists";
    Option<Comment> comment  "the translated comment from the Absyn";
  end OVERLOAD;

  record PDER "the partial derivative"
    Absyn.Path  functionPath     "function name" ;
    list<Ident> derivedVariables "derived variables" ;
    Option<Comment> comment      "the Absyn comment";
  end PDER;

end ClassDef;

uniontype Comment

  record COMMENT
    Option<Annotation> annotation_;
    Option<String> comment;
  end COMMENT;

  record CLASS_COMMENT
    list<Annotation> annotations;
    Option<Comment> comment;
  end CLASS_COMMENT;
end Comment;

uniontype Annotation

  record ANNOTATION
    Mod modification;
  end ANNOTATION;

end Annotation;

uniontype Equation "- Equations"
  record EQUATION "an equation"
    EEquation eEquation "an equation";
  end EQUATION;

end Equation;

uniontype EEquation
"These represent equations and are almost identical to their Absyn versions.
 In EQ_IF the elseif branches are represented as normal else branches with
 a single if statement in them."
  record EQ_IF
    list<Absyn.Exp> condition "conditional" ;
    list<list<EEquation>> thenBranch "the true (then) branch" ;
    list<EEquation>       elseBranch "the false (else) branch" ;
    Option<Comment> comment;
    Absyn.Info info;
  end EQ_IF;

  record EQ_EQUALS "the equality equation"
    Absyn.Exp expLeft  "the expression on the left side of the operator";
    Absyn.Exp expRight "the expression on the right side of the operator";
    Option<Comment> comment;
    Absyn.Info info;
  end EQ_EQUALS;

  record EQ_CONNECT "the connect equation"
    Absyn.ComponentRef crefLeft  "the connector/component reference on the left side";
    Absyn.ComponentRef crefRight "the connector/component reference on the right side";
    Option<Comment> comment;
    Absyn.Info info;
  end EQ_CONNECT;

  record EQ_FOR "the for equation"
    Ident           index        "the index name";
    Absyn.Exp       range        "the range of the index";
    list<EEquation> eEquationLst "the equation list";
    Option<Comment> comment;
    Absyn.Info info;
  end EQ_FOR;

  record EQ_WHEN "the when equation"
    Absyn.Exp        condition "the when condition";
    list<EEquation>  eEquationLst "the equation list";
    list<tuple<Absyn.Exp, list<EEquation>>> tplAbsynExpEEquationLstLst "the elsewhen expression and equation list";
    Option<Comment> comment;
    Absyn.Info info;
  end EQ_WHEN;

  record EQ_ASSERT "the assert equation"
    Absyn.Exp condition "the assert condition";
    Absyn.Exp message   "the assert message";
    Option<Comment> comment;
    Absyn.Info info;
  end EQ_ASSERT;

  record EQ_TERMINATE "the terminate equation"
    Absyn.Exp message "the terminate message";
    Option<Comment> comment;
    Absyn.Info info;
  end EQ_TERMINATE;

  record EQ_REINIT "a reinit equation"
    Absyn.ComponentRef cref      "the variable to initialize";
    Absyn.Exp          expReinit "the new value" ;
    Option<Comment> comment;
    Absyn.Info info;
  end EQ_REINIT;

  record EQ_NORETCALL "function calls without return value"
    Absyn.ComponentRef functionName "the function nanme";
    Absyn.FunctionArgs functionArgs "the function arguments";
    Option<Comment> comment;
    Absyn.Info info;
  end EQ_NORETCALL;

end EEquation;

uniontype AlgorithmSection "- Algorithms
  The Absyn module uses the terminology from the
  grammar, where algorithm means an algorithmic
  statement. But here, an Algorithm means a whole
  algorithm section."
  record ALGORITHM "the algorithm section"
    list<Statement> statements "the algorithm statements" ;
  end ALGORITHM;

end AlgorithmSection;

uniontype Statement "The Statement type describes one algorithm statement in an algorithm section."
  record ALG_ASSIGN
    Absyn.Exp assignComponent "assignComponent" ;
    Absyn.Exp value "value" ;
    Option<Comment> comment;
    Absyn.Info info;
  end ALG_ASSIGN;

  record ALG_IF
    Absyn.Exp boolExpr;
    list<Statement> trueBranch;
    list<tuple<Absyn.Exp, list<Statement>>> elseIfBranch;
    list<Statement> elseBranch;
    Option<Comment> comment;
    Absyn.Info info;
  end ALG_IF;

  record ALG_FOR
    Absyn.ForIterators iterators;
    list<Statement> forBody "forBody" ;
    Option<Comment> comment;
    Absyn.Info info;
  end ALG_FOR;

  record ALG_WHILE
    Absyn.Exp boolExpr "boolExpr" ;
    list<Statement> whileBody "whileBody" ;
    Option<Comment> comment;
    Absyn.Info info;
  end ALG_WHILE;

  record ALG_WHEN_A
    list<tuple<Absyn.Exp, list<Statement>>> branches;
    Option<Comment> comment;
    Absyn.Info info;
  end ALG_WHEN_A;

  record ALG_NORETCALL
    Absyn.ComponentRef functionCall "functionCall" ;
    Absyn.FunctionArgs functionArgs "functionArgs; general fcalls without return value" ;
    Option<Comment> comment;
    Absyn.Info info;
  end ALG_NORETCALL;

  record ALG_RETURN
    Option<Comment> comment;
    Absyn.Info info;
  end ALG_RETURN;

  record ALG_BREAK
    Option<Comment> comment;
    Absyn.Info info;
  end ALG_BREAK;

  // Part of MetaModelica extension. KS
  record ALG_TRY
    list<Statement> tryBody;
    Option<Comment> comment;
    Absyn.Info info;
  end ALG_TRY;

  record ALG_CATCH
    list<Statement> catchBody;
    Option<Comment> comment;
    Absyn.Info info;
  end ALG_CATCH;

  record ALG_THROW
    Option<Comment> comment;
    Absyn.Info info;
  end ALG_THROW;

  record ALG_FAILURE
    list<Statement> stmts;
    Option<Comment> comment;
    Absyn.Info info;
  end ALG_FAILURE;
  //-------------------------------

end Statement;

// common prefixes to elements 
uniontype Visibility "the visibility prefix"
  record PUBLIC    "a public element"    end PUBLIC;
  record PROTECTED "a protected element" end PROTECTED;
end Visibility;

uniontype Redeclare "the redeclare prefix"
  record REDECLARE     "a redeclare prefix"     end REDECLARE;
  record NOT_REDECLARE "a non redeclare prefix" end NOT_REDECLARE;
end Redeclare;

uniontype Replaceable "the replaceable prefix"
  record REPLACEABLE "a replaceable prefix containing an optional constraint"
    Option<Absyn.ConstrainClass> cc  "the constraint class"; 
  end REPLACEABLE;
  record NOT_REPLACEABLE "a non replaceable prefix" end NOT_REPLACEABLE;
end Replaceable;

uniontype Final "the final prefix"
  record FINAL    "a final prefix"      end FINAL;
  record NOT_FINAL "a non final prefix" end NOT_FINAL;
end Final;

uniontype Each "the each prefix"
  record EACH     "a each prefix"     end EACH;
  record NOT_EACH "a non each prefix" end NOT_EACH;
end Each;

uniontype Encapsulated "the encapsulated prefix"
  record ENCAPSULATED     "a encapsulated prefix"     end ENCAPSULATED;
  record NOT_ENCAPSULATED "a non encapsulated prefix" end NOT_ENCAPSULATED;
end Encapsulated;

uniontype Partial "the partial prefix"
  record PARTIAL     "a partial prefix"     end PARTIAL;
  record NOT_PARTIAL "a non partial prefix" end NOT_PARTIAL;
end Partial;

uniontype Stream "the stream prefix"
  record STREAM     "a stream prefix"     end STREAM;
  record NOT_STREAM "a non stream prefix" end NOT_STREAM;
end Stream;

uniontype Flow "the flore prefix"
  record FLOW     "a flow prefix"     end FLOW;
  record NOT_FLOW "a non flow prefix" end NOT_FLOW;
end Flow;

uniontype Prefixes "the common class or component prefixes"
  record PREFIXES "the common class or component prefixes"
    Visibility       visibility           "the protected/public prefix";
    Redeclare        redeclarePrefix      "redeclare prefix";    
    Final            finalPrefix          "final prefix, be it at the element or top level";
    Absyn.InnerOuter innerOuter           "the inner/outer/innerouter prefix";
    Replaceable      replaceablePrefix    "replaceable prefix";
  end PREFIXES;      
end Prefixes;

uniontype Element "- Elements
  There are four types of elements in a declaration, represented by the constructors:
  IMPORT     (for import clauses)
  EXTENDS    (for extends clauses),
  CLASS      (for top/local class definitions)
  COMPONENT  (for local variables)
  DEFINEUNIT (for units)"
  
  record IMPORT "an import element"
    Absyn.Import imp                 "the import definition";
    Visibility   visibility          "the protected/public prefix";    
    Absyn.Info   info                "the import information";
  end IMPORT;  
  
  record EXTENDS "the extends element"
    Path baseClassPath               "the extends path";
    Visibility visibility            "the protected/public prefix";
    Mod modifications                "the modifications applied to the base class";
    Option<Annotation> ann           "the extends annotation";
    Absyn.Info info                  "the extends info";
  end EXTENDS;

  record CLASS "a class definition"
    Ident   name                     "the name of the class";
    Prefixes prefixes                "the common class or component prefixes";
    Encapsulated encapsulatedPrefix  "the encapsulated prefix";
    Partial partialPrefix            "the partial prefix";    
    Restriction restriction          "the restriction of the class";
    ClassDef classDef                "the class specification";
    Absyn.Info info                  "the class information";
  end CLASS;
  
  record COMPONENT "a component"
    Ident name                      "the component name";
    Prefixes prefixes               "the common class or component prefixes";    
    Attributes attributes           "the component attributes";
    Absyn.TypeSpec typeSpec         "the type specification";
    Mod modifications               "the modifications to be applied to the component";
    Option<Comment> comment         "this if for extraction of comments and annotations from Absyn";
    Option<Absyn.Exp> condition     "the conditional declaration of a component";
    Absyn.Info info                 "this is for line and column numbers, also file name.";
  end COMPONENT;

  record DEFINEUNIT "a unit defintion has a name and the two optional parameters exp, and weight"
    Ident name;
    Visibility visibility            "the protected/public prefix";
    Option<String> exp               "the unit expression";
    Option<Real> weight              "the weight";
  end DEFINEUNIT;
  
end Element;

uniontype Attributes "- Attributes"
  record ATTR "the attributes of the component"
    Absyn.ArrayDim arrayDims "the array dimensions of the component";
    Flow   flowPrefix   "the flow prefix";
    Stream streamPrefix "the stream prefix";
    Variability variability " the variability: parameter, discrete, variable, constant" ;
    Absyn.Direction direction "the direction: input, output or bidirectional" ;
  end ATTR;
end Attributes;

uniontype Variability "the variability of a component"
  record VAR      "a variable"          end VAR;
  record DISCRETE "a discrete variable" end DISCRETE;
  record PARAM    "a parameter"         end PARAM;
  record CONST    "a constant"          end CONST;
end Variability;

uniontype Initial "the initial attribute of an algorithm or equation
 Intial is used as argument to instantiation-function for
 specifying if equations or algorithms are initial or not."
  record INITIAL     "an initial equation or algorithm" end INITIAL;
  record NON_INITIAL "a normal equation or algorithm"   end NON_INITIAL;
end Initial;

end SCode;

package Util
  
	uniontype DateTime
	  record DATETIME
	    Integer sec;
	    Integer min;
	    Integer hour;
	    Integer mday;
	    Integer mon;
	    Integer year;
	  end DATETIME;
	end DateTime;  
  
  function escapeModelicaStringToCString
    input String modelicaString;
    output String cString;
  end escapeModelicaStringToCString;

  function escapeModelicaStringToXmlString
    input String modelicaString;
    output String xmlString;
  end escapeModelicaStringToXmlString;

	function getCurrentDateTime
	  output DateTime dt;
	end getCurrentDateTime;

	function listFill 
  	input Type_a inTypeA;
	  input Integer inInteger;
  	output list<Type_a> outTypeALst;
	  replaceable type Type_a subtypeof Any;
	end listFill;
	
	function listUnion 
    input list<Type_a> inTypeALst1;
    input list<Type_a> inTypeALst2;
    output list<Type_a> outTypeALst;
    replaceable type Type_a subtypeof Any;
  end listUnion;

  function listThreadTuple
    replaceable type Type_b subtypeof Any;
    input list<Type_a> inTypeALst;
    input list<Type_b> inTypeBLst;
    output list<tuple<Type_a, Type_b>> outTplTypeATypeBLst;
    replaceable type Type_a subtypeof Any;
  end listThreadTuple;

end Util;


package ComponentReference

	function crefStripLastSubs
		input DAE.ComponentRef inComponentRef;
		output DAE.ComponentRef outComponentRef;
	end crefStripLastSubs;

  function crefSubs
    input DAE.ComponentRef cref;
    output list<DAE.Subscript> subs;
  end crefSubs;

  function crefLastType
    input DAE.ComponentRef inRef;
    output DAE.ExpType res;
  end crefLastType;

end ComponentReference;

package Expression

	function crefHasScalarSubscripts
		input DAE.ComponentRef cr;
		output Boolean hasScalarSubs;
	end crefHasScalarSubscripts;

  function typeof
    input DAE.Exp inExp;
    output DAE.ExpType outType;
  end typeof;

  function isHalf
    input DAE.Exp inExp;
    output Boolean outBoolean;
  end isHalf;

  function realExpIntLit
    input DAE.Exp exp;
    output Option<Integer> oi;
  end realExpIntLit;

  function flattenArrayExpToList
    input DAE.Exp e;
    output list<DAE.Exp> expLst;
  end flattenArrayExpToList;

end Expression;

package ExpressionDump
  function printExpStr
    input DAE.Exp e;
    output String s;
  end printExpStr;
  function typeString
    input DAE.ExpType et;
    output String s;
  end typeString;
end ExpressionDump;

package RTOpts
  function acceptMetaModelicaGrammar
    output Boolean outBoolean;
  end acceptMetaModelicaGrammar;
end RTOpts;

package Settings
  function getVersionNr
    output String outString;
  end getVersionNr;
end Settings;

package Patternm
  function getValueCtor
    input Integer ix;
    output Integer ctor;
  end getValueCtor;
end Patternm;

package Error
  function infoStr
    input Absyn.Info info;
    output String str;
  end infoStr;
end Error;

package Values
end Values;

package ValuesUtil
  function valueExp
    input Values.Value inValue;
    output DAE.Exp outExp;
  end valueExp;
end ValuesUtil;

package BackendQSS
	function replaceCondWhens
  	input list<SimCode.SimWhenClause> whenClauses;
	  input list<SimCode.HelpVarInfo> helpVars;
		input list<BackendDAE.ZeroCrossing> zeroCrossings;
		output list<SimCode.SimWhenClause> replacedWhenClauses;
	end replaceCondWhens;

	function replaceZC
		input SimCode.SimEqSystem i;
		input list<BackendDAE.ZeroCrossing> zc;
		output SimCode.SimEqSystem o;
	end replaceZC;

	uniontype DevsStruct "DEVS structure"
  	record DEVS_STRUCT  
    	list<list<Integer>>[:] outLinks "output connections for each DEVS block";
    	list<list<Integer>>[:] outVars "output variables for each DEVS block";
    	list<list<Integer>>[:] inLinks "input connections for each DEVS block";
    	list<list<Integer>>[:] inVars "input variables for each DEVS block";
  	end DEVS_STRUCT;
	end DevsStruct;

	uniontype QSSinfo "- equation indices in static blocks and DEVS structure"
  	record QSSINFO
    	list<list<list<Integer>>> BLTblocks "BLT blocks in static functions";
	    DevsStruct DEVSstructure "DEVS structure of the model";
			list<list<SimCode.SimEqSystem>> eqs;
  		list<BackendDAE.Var> outVarLst;
  		list<Integer> zcSamplesInd;
  	end QSSINFO;
	end QSSinfo;

	function getInputs
		input DevsStruct st;
		input Integer index;
		output list<Integer> vars;
	end getInputs;

	function getOutputs
		input DevsStruct st;
		input Integer index;
		output list<Integer> vars;
	end getOutputs;


	function derPrefix
 		input BackendDAE.Var var;
		output String prefix;
	end derPrefix;

	function numInputs
		input QSSinfo qssInfo;
		input Integer numBlock;
		output Integer inputs;
	end numInputs;

	function numOutputs
		input QSSinfo qssInfo;
		input Integer numBlock;
		output Integer outputs;
	end numOutputs;

  function generateConnections
    input QSSinfo qssInfo;
    output list<list<Integer>> conns;
  end generateConnections;

  function getStates
    input QSSinfo qssInfo;
    output list<BackendDAE.Var> states;
  end getStates;

  function getAllInputs
    input QSSinfo qssInfo;
    output list<Integer> vars_tuple;
  end getAllInputs;

  function getAllOutputs
    input QSSinfo qssInfo;
    output list<Integer> vars_tuple;
  end getAllOutputs;

end BackendQSS;

package BackendVariable
	function varCref
  	input BackendDAE.Var inVar;
	  output DAE.ComponentRef outComponentRef;
	end varCref;

  function isStateVar
    input BackendDAE.Var inVar;
    output Boolean outBoolean;
  end isStateVar;

  function varIndex 
    input BackendDAE.Var inVar;
    output Integer outInteger;
  end varIndex;


end BackendVariable;

package DAEDump

  function ppStmtStr
    input DAE.Statement stmt;
    input Integer inInteger;
    output String outString;
  end ppStmtStr;

end DAEDump;

package Algorithm
  function getStatementSource
    input DAE.Statement stmt;
    output DAE.ElementSource source;
  end getStatementSource;
end Algorithm;

end SimCodeTV;
