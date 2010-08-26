// name: Cosh
// keywords: cosh
// status: correct
//
// Tests the built-in cosh function
//

model Cosh
  Real r;
equation
  r = cosh(45);
end Cosh;

// Result:
// class Cosh
// Real r;
// equation
//   r = 1.74671355287425e+19;
// end Cosh;
// endResult
