// name:     Cross
// keywords: equation, vector
// status:   correct
// 
// 

model Cross
  Real x[3];
  Real y[3];
  Real[3] z;
  Integer xi[3];
  Integer yi[3];
  Integer[3] zi;
equation
  z = cross(x,y);
  zi = cross(xi,yi);
end Cross;

// Result:
// class Cross
// Real x[1];
// Real x[2];
// Real x[3];
// Real y[1];
// Real y[2];
// Real y[3];
// Real z[1];
// Real z[2];
// Real z[3];
// Integer xi[1];
// Integer xi[2];
// Integer xi[3];
// Integer yi[1];
// Integer yi[2];
// Integer yi[3];
// Integer zi[1];
// Integer zi[2];
// Integer zi[3];
// equation
//   z[1] = x[2] * y[3] - x[3] * y[2];
//   z[2] = x[3] * y[1] - x[1] * y[3];
//   z[3] = x[1] * y[2] - x[2] * y[1];
//   zi[1] = xi[2] * yi[3] - xi[3] * yi[2];
//   zi[2] = xi[3] * yi[1] - xi[1] * yi[3];
//   zi[3] = xi[1] * yi[2] - xi[2] * yi[1];
// end Cross;
// endResult
