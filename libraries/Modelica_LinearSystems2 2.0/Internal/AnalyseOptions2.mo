within Modelica_LinearSystems2.Internal;
record AnalyseOptions2
  "Defines the characteristics of the eigenvalues to be print or to be plot"
  extends Modelica.Icons.Record;
  Boolean plotEigenValues=true "plot eigenvalues"     annotation(Dialog(group="analyse options"),choices(checkBox=true));
  Boolean plotInvariantZeros=true "plot invariant zeros"    annotation(Dialog(group="analyse options"),choices(checkBox=true));
  Boolean plotStepResponse=true "plot step respones. Only for SISO system" 
                                                            annotation(Dialog(group="analyse options"),choices(checkBox=true));
  Boolean plotFrequencyResponse=true "plot bode diagram. Only for SISO system" 
                                                              annotation(Dialog(group="analyse options"),choices(checkBox=true));
  Boolean printEigenValues=true "write eigenvalues into the report" 
                                    annotation(Dialog(group="analyse options"),choices(checkBox=true));

  Boolean printInvariantZeros=true "write invariant zreos into the report" 
                                       annotation(Dialog(group="analyse options"),choices(checkBox=true));
  Boolean printControllability=false
    "indicates controllability of every single pole" 
                                        annotation(Dialog(group="analyse options"),choices(checkBox=true));
  Boolean printObservability=false
    "indicates observability of every single pole" 
                                      annotation(Dialog(group="analyse options"),choices(checkBox=true));
  String headingEigenValues="Eigenvalues";
  String headingInvariantzeros="Invariant zeros";
  String headingStepResponse="Step response";
  String headingFrequencyResponse="Frequency response";

final Boolean printEigenValueProperties = false
    "write eigenvalues with properties" 
                                    annotation(Dialog(group="analyse options"),choices(checkBox=true));

end AnalyseOptions2;