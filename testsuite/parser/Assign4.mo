// name:     Assign4
// keywords: parse error
// status:   incorrect
// 

model Assign4
equation
  x := res;
end Assign4;

// Result:
// Error processing file: Assign4.mo
// [Assign4.mo:8:5-8:6:writable] Error: Parse error: Equations can not contain assignments (':='), use equality ('=') instead
// 
// # Error encountered! Exiting...
// # Please check the error message and the flags.
// 
// Execution failed!
// endResult
