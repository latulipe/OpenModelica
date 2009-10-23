package Parse

import AssignTwoType;

protected function yyparse
  output Integer i;
external;
end yyparse;

protected function getAST
  output AssignTwoType.Program program;
external;
end getAST;

public function parse
  output AssignTwoType.Program program;
  Integer yyres;
algorithm
  yyres := yyparse();
  program := matchcontinue (yyres)
    case 0 then getAST();
 end matchcontinue;
end parse;

end Parse;