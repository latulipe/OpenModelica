// name: ConnectorIllegal2
// keywords: connector
// status: incorrect
//
// Tests an illegal connector definition
//

connector IllegalConnector
algorithm
end IllegalConnector;

model ConnectorIllegal2
  IllegalConnector ic;
end ConnectorIllegal2;
// Result:
// Error processing file: ConnectorIllegal2.mo
// [ConnectorIllegal2.mo:13:3-13:22:writable] Error: Variable ic: Algorithm section is not allowed in connector.
// 
// # Error encountered! Exiting...
// # Please check the error message and the flags.
// 
// Execution failed!
// endResult