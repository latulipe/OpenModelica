// name:     Array1
// keywords: array
// status:   correct
// 
// This is a simple test of basic array handling.
// 

model Array1
  Integer x[5] = {1,2,3,4,5};
  Integer y[3] = 1:3;
end Array1;

// Result:
// class Array1
// Integer x[1] = 1;
// Integer x[2] = 2;
// Integer x[3] = 3;
// Integer x[4] = 4;
// Integer x[5] = 5;
// Integer y[1] = 1;
// Integer y[2] = 2;
// Integer y[3] = 3;
// end Array1;
// endResult
