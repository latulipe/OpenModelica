// name:     Assign1
// keywords: parse error
// status:   incorrect
// 

model Assign1
algorithm
  x = 3;
end Assign1;

// Result:
// Error processing file: Assign1.mo
// [Assign1.mo:8:5-8:6:writable] Error: Parse error: Algorithms can not contain equations ('='), use assignments (':=') instead
// 
// # Error encountered! Exiting...
// # Please check the error message and the flags.
// 
// Execution failed!
// endResult
