// name: DiscreteDeclRecord
// keywords: discrete
// status: correct
//
// Tests the discrete prefix on a record type
//

record DiscreteRecord
  Real r;
end DiscreteRecord;

class DiscreteDeclRecord
  discrete DiscreteRecord dr;
equation
  dr.r = 1.0;
end DiscreteDeclRecord;

// Result:
// class DiscreteDeclRecord
// discrete Real dr.r;
// equation
//   dr.r = 1.0;
// end DiscreteDeclRecord;
// endResult
