// name:     WhenPriority
// keywords: algorithm when
// status:   correct
// 
// Close defined by two equations
//

model WhenPriority
  Boolean close;
  parameter Real x = 5;
algorithm
  when x >= 5 then
    close := true;
  elsewhen x <= 5 then
    close := false;
  end when;
end WhenPriority;
