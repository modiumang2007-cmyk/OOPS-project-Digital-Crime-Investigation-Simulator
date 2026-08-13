# Problem Statement
## Digital Crime Investigation Simulator ("CaseFile")

**Course:** Object-Oriented Programming
**Language:** C++

### Background

Investigative agencies handle cases involving multiple suspects, witnesses, evidence items, and investigators simultaneously. Each case must move through a defined lifecycle — from opening to active investigation to closure — while evidence integrity is preserved at every step, since compromised or mishandled evidence can invalidate an entire case. Manually tracking these relationships and enforcing correct case-state transitions is error-prone without a structured system behind it.

### Problem

There is no lightweight, extensible system that models the relationships between cases, evidence, suspects, and investigators while enforcing correct workflow rules (e.g., no modifying a closed case) and correctly handling the fact that different evidence types require fundamentally different analysis procedures. A rigid, non-object-oriented approach (e.g., one large procedural program with conditionals for every evidence type) does not scale as new evidence or investigator types are added.

### Proposed Solution

**CaseFile** is a console-based case management platform built around a class hierarchy that models the real structure of an investigation: cases contain suspects, evidence, and assigned investigators; evidence is polymorphic across digital, physical, and testimonial types, each with its own analysis logic; and investigators are polymorphic across specializations, each following a different investigation procedure. Case state transitions are validated, and invalid operations raise specific custom exceptions rather than failing silently.

### Objectives

- Model the investigation domain using inheritance, polymorphism, abstraction, and encapsulation rather than procedural conditionals
- Enforce case lifecycle rules automatically, not by convention
- Persist case data to file so investigations can be saved and resumed
- Produce a system where each design decision (why a class is abstract, why an interface is separate, why a destructor is virtual) has a clear justification

### Scope

In scope: case lifecycle management, three evidence types with type-specific analysis, investigator assignment, suspect/witness records, keyword search, report generation, file-based save/load, console interface.
Out of scope: GUI, database backend, multi-user login.

### Expected Outcome

A working console application demonstrating a clean, defensible object-oriented design, suitable as a course submission and a general software-engineering portfolio piece.
