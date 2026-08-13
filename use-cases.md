Actors in CaseFile
Actor	Type	How it interacts
Admin	Primary (human)	Creates cases, assigns investigators, closes cases
Cyber Investigator	Primary (human)	Logs in via console menu, adds/analyzes evidence, updates case status, generates reports
Field Investigator	Primary (human)	Same as above, different investigate() behavior
File System	Secondary (external)	Receives save requests, provides load requests — CaseFile depends on it but doesn't control it
Console/Terminal	Secondary (external)	The I/O boundary itself — technically an external actor since input/output devices sit outside your program's logic
