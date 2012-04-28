within ThermoSysPro.WaterSteam.BoundaryConditions;
model Source "Water/steam source" 
  parameter ThermoSysPro.Units.SpecificEnthalpy h0=100000 
    "Fluid specific enthalpy (active if IEnthalpy connector is not connected)";
  
protected 
  ThermoSysPro.Units.AbsolutePressure P "Fluid pressure";
  Modelica.SIunits.MassFlowRate Q "Mass flow rate";
  ThermoSysPro.Units.SpecificEnthalpy h "Fluid specific enthalpy";
  
  annotation (
    Coordsys(
      extent=[-100, -100; 100, 100],
      grid=[2, 2],
      component=[20, 20]),
    Diagram(
      Line(points=[40,0; 90,0; 72,10]),
      Line(points=[90,0; 72,-10]),
      Text(extent=[-30,-40; -12,-60], string="h"),
      Rectangle(extent=[-40, 40; 40, -40], style(fillColor=6, rgbfillColor={255,
              255,0}))),
    Icon(
      Line(points=[40,0; 90,0; 72,10]),
      Rectangle(extent=[-40, 40; 40, -40], style(fillColor=6, rgbfillColor={255,
              255,0})),
      Line(points=[90,0; 72,-10]),
      Text(extent=[-32,-40; -12,-60], string="h")),
    Window(
      x=0.23,
      y=0.15,
      width=0.81,
      height=0.71),
    Documentation(info="<html>
<p><b>Copyright &copy; EDF 2002 - 2010</b></p>
</HTML>
<html>
<p><b>ThermoSysPro Version 2.0</b></p>
</HTML>
", revisions="<html>
<u><p><b>Authors</u> : </p></b>
<ul style='margin-top:0cm' type=disc>
<li>
    Baligh El Hefni</li>
<li>
    Daniel Bouskela</li>
</ul>
</html>
"));
public 
  ThermoSysPro.InstrumentationAndControl.Connectors.InputReal ISpecificEnthalpy 
    annotation (extent=[-10, -40; 10, -60], rotation=-90);
  Connectors.FluidOutlet C                annotation (extent=[90,-10; 110,10]);
equation 
  
  C.P = P;
  C.Q = Q;
  C.h_vol = h;
  
  /* Specific enthalpy */
  if (cardinality(ISpecificEnthalpy) == 0) then
    ISpecificEnthalpy.signal = h0;
  end if;
  
  h = ISpecificEnthalpy.signal;
  
end Source;