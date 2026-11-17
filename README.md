# CaseFile — Digital Crime Investigation Simulator (v2)

A full-stack, web-based case management system that models how investigative agencies track cases, suspects, witnesses, investigators and digital/physical evidence. Built as an Object-Oriented Programming course project, **v2 is deliberately designed as a complete OOP-in-C++ showcase**: every concept in the syllabus lives in real, running backend code behind a REST endpoint, not only in diagrams.

> **Stack at a glance:** vanilla HTML/CSS/JS frontend · C++17/20 backend on the [Crow](https://crowcpp.org) framework · PostgreSQL database

---

## Table of Contents

1. [About](#about)
2. [Team](#team)
3. [Goals](#goals)
4. [Tech Stack](#tech-stack)
5. [Actors](#actors)
6. [Core Modules](#core-modules)
7. [OOP Concept Coverage Matrix](#oop-concept-coverage-matrix)
8. [Domain Model & Class Hierarchy](#domain-model--class-hierarchy)
9. [OOP Highlights](#oop-highlights)
10. [Backend Architecture](#backend-architecture)
11. [Database Design](#database-design)
12. [API Reference](#api-reference)
13. [Frontend Pages](#frontend-pages)
14. [Non-Functional Requirements](#non-functional-requirements)
15. [Testing & Verification](#testing--verification)
16. [Build & Run](#build--run)
17. [Documentation](#documentation)

---

## About

Investigations involve multiple interrelated entities — cases, suspects, evidence and investigators — each with different behavior and strict lifecycle rules (e.g. a closed case can't be modified). CaseFile models this domain using core OOP principles rather than procedural conditionals, so adding a new evidence type or investigator specialization means adding a class, not rewriting existing logic.

Evidence integrity is maintained with **SHA-256 checksum verification**, and the backend is designed to be both **memory-safe** and **thread-safe**.

Full problem framing: [`problem-statement.md`](./problem-statement.md) · Full requirements: *CaseFile v2 PRD*

## Team

| Name | GitHub | Role |
|---|---|---|
| Umang Modi | `modiumang2007-cmyk` | Team leader |
| Riya Vasoya | `riyaa1211` | Team member |
| Vedant Songire | `vedant25-iiitv` | Team member |
| Pratyush Pandit | `Pratyush116` | Team member |
| Kunal Patni | `kunal786-design` | Team member |

## Goals

- Implement every OOP concept in the [coverage matrix](#oop-concept-coverage-matrix) in the C++ backend, with a traceable file, class and API for each.
- Build a complete full-stack web application with a native C++ backend using Crow.
- Provide a realistic crime-investigation workflow.
- Maintain evidence integrity using SHA-256 checksum verification.
- Be memory-safe and thread-safe, verified with AddressSanitizer, ThreadSanitizer and Valgrind.

**Non-goal:** the frontend demonstrates no OOP. All OOP lives in C++.

## Tech Stack

| Layer | Technology |
|---|---|
| **Frontend** | HTML5, CSS3, JavaScript (ES6+, vanilla, no framework); Fetch API; simple view-switching or History API routing |
| **Backend** | C++17/20 (C++20 preferred for `std::jthread` and `operator<=>`) |
| Web framework | Crow (header-only) with `app.multithreaded()` enabled |
| DB access | libpqxx |
| JSON | nlohmann/json |
| Hashing | OpenSSL (SHA-256) |
| Auth | jwt-cpp (JWT) |
| Testing | Catch2 or GoogleTest |
| **Database** | PostgreSQL — native ENUM, UUID/SERIAL, JSONB, foreign keys, transactions |
| **Tooling** | CMake with vcpkg or Conan, Postman/curl, `-fsanitize=address,undefined` and `-fsanitize=thread` presets, Valgrind, Docker |

## Actors

| Actor | Responsibility |
|---|---|
| **Admin** | Create cases, manage users, assign investigators, run system/demo endpoints |
| **Investigator** | Analyze evidence, update cases, generate reports |
| **System** | Validate evidence integrity, run background scans, maintain audit logs |

## Core Modules

- **Authentication** — register, login, JWT (jwt-cpp), role-based authorization (`ADMIN`, `INVESTIGATOR`) in a Crow middleware class (`before_handle` / `after_handle`). Passwords are hashed, never stored in plain text.
- **Case Management** — create, update, assign investigators, track lifecycle `OPEN → UNDER_INVESTIGATION → CLOSED`. Closed cases are immutable (enforced inside `Case` via `CaseClosedError`); duplicate titles throw `DuplicateCaseError`.
- **Evidence Management** — polymorphic evidence via the abstract `Evidence` hierarchy. Workflow: upload → `EvidenceFactory` creates the right subclass → `HashService` computes SHA-256 → hash stored in PostgreSQL → `verifyIntegrity()` recomputes and compares. A mismatch throws `IntegrityViolationError`.
- **Investigator Management** — profiles, workload tracking, assignment, thread-safe transfer between investigators. `CyberInvestigator`, `FieldInvestigator` and `TaskForceLead` are built by a factory from the stored specialization.
- **Reporting** — case summary, evidence report and investigator activity report, produced through the overloaded `ReportService::generate(...)`.
- **Concurrency & Diagnostics** — `IntegrityScanner`, `AuditLogger`, `ConnectionPool`, `CaseCache`, and the demo endpoints.

## OOP Concept Coverage Matrix

Each row is a mandatory deliverable. The **Demo** column is the endpoint that proves the concept at runtime.

| # | Concept | Implemented in | Demo |
|---|---|---|---|
| 1 | Abstraction | `Evidence`, `IReportable`, `ICertifiable` abstract classes | `GET /api/demo/oop` |
| 2 | Encapsulation | `Case`, `User` (private state, validated setters) | `PUT /api/cases/:id` on a closed case |
| 3 | Inheritance (pillar) | `Evidence` and `Person` hierarchies | `GET /api/demo/oop` |
| 4 | Polymorphism (pillar) | `vector<unique_ptr<Evidence>>` virtual dispatch | `POST /api/evidence/analyze` |
| 5 | Classes and objects | All model classes | Whole app |
| 6 | Constructors | Default, parameterized, copy, move, delegating, initializer-list | `GET /api/demo/lifecycle` |
| 7 | Destructors | Virtual destructors, RAII classes | `GET /api/demo/lifecycle` |
| 8 | Single inheritance | `DigitalEvidence : Evidence` | `GET /api/demo/oop` |
| 9 | Multiple inheritance | `CyberInvestigator : Investigator, ICertifiable` | `GET /api/demo/oop` |
| 10 | Multilevel inheritance | `Person → User → Investigator → CyberInvestigator` | `GET /api/demo/oop` |
| 11 | Hierarchical inheritance | `Evidence → Digital/Physical/Testimonial`; `Investigator → Cyber/Field` | `GET /api/demo/oop` |
| 12 | Hybrid inheritance | `TaskForceLead : CyberInvestigator, FieldInvestigator` (diamond, virtual base) | `GET /api/demo/oop` |
| 13 | Function overloading (compile-time polymorphism) | `ReportService::generate(...)`, constructors, `addEvidence(...)` | `GET /api/reports/...` |
| 14 | Overriding (runtime polymorphism) | `analyzeEvidence()`, `generateReport()`, `describe()` | `POST /api/evidence/analyze` |
| 15 | Virtual functions & abstract classes | Pure virtuals, virtual destructor, `clone()`, `override` / `final` | `POST /api/evidence/analyze` |
| 16 | Operator overloading | `==`, `<=>` / `<`, `<<`, `+=`, `[]`, `()`, `=`, `++` | `GET /api/demo/operators` |
| 17 | Exception handling | `AppException` hierarchy, `noexcept`, stack unwinding, global handler | Any failing request |
| 18 | Pointers & memory management | `unique_ptr`, `shared_ptr`, `weak_ptr`, raw `new[]`/`delete[]` in `RawBuffer`, function pointers | `GET /api/demo/memory` |
| 19 | Multithreading | Crow worker threads, `IntegrityScanner`, `std::async` parallel hashing | `POST /api/evidence/verify-all` |
| 20 | Mutex | `ConnectionPool`, `AuditLogger`, `CaseCache` (`shared_mutex`), investigator locks | `GET /api/system/threads` |
| 21 | Deadlock (cause, detection, fix) | Case-transfer demo with unsafe and safe modes | `POST /api/demo/deadlock?mode=unsafe\|safe` |

## Domain Model & Class Hierarchy

### Evidence hierarchy (single and hierarchical)

```
            Evidence (abstract)
                   |
   ----------------------------------------
   |                |                     |
DigitalEvidence  PhysicalEvidence  TestimonialEvidence (final)
```

| Class | `analyzeEvidence()` | Specific functions |
|---|---|---|
| `DigitalEvidence` | Metadata extraction | SHA-256 hash verification |
| `PhysicalEvidence` | Simulated laboratory analysis | Chain-of-custody log |
| `TestimonialEvidence` | Credibility scoring | Statement consistency check |

### People hierarchy (multilevel, multiple, hierarchical, hybrid)

```
Person
  |
User                         ICertifiable (interface)
  |                                  |
Investigator (virtual base)          |
  |        \                         |
  |         \                        |
CyberInvestigator -------------------+   (multiple: Investigator + ICertifiable)
FieldInvestigator
        \          /
        TaskForceLead   (hybrid: diamond over virtual Investigator)
```

### Other classes

- `Case` — owns evidence and suspects, enforces the lifecycle.
- `Suspect`, `Witness` — both derive from `Person`.
- `Report` — implements `IReportable`.
- `AuditLogger`, `ConnectionPool`, `IntegrityScanner`, `RawBuffer` — infrastructure classes demonstrating RAII, mutexes and manual memory.
- `AppException` hierarchy.

## OOP Highlights

### Four pillars
- **Abstraction** — `Evidence` and `IReportable` expose only what callers need; `HashService` hides OpenSSL.
- **Encapsulation** — all data members are `private`/`protected`; changes go through validated setters. A closed case rejects every mutation inside `Case` itself, not only in the API layer.
- **Inheritance** and **Polymorphism** — see below.

### Classes, constructors and destructors
`Case`, `DigitalEvidence`, `RawBuffer` and `Report` show the full constructor/destructor set: default, parameterized, delegating, initializer-list, copy (deep copy via `Evidence::clone()`), move, copy/move assignment (self-assignment safe), and a virtual destructor. Further rules:
- Member initializer lists in every constructor; explicit `this` in at least one method (fluent setters); `explicit` on single-argument constructors.
- Every constructor and destructor writes to `AuditLogger`, so the lifecycle is visible at `GET /api/demo/lifecycle`.
- RAII objects (`ConnectionLease`, `ScopedTimer`, `std::lock_guard`) release resources in their destructors.
- `static` live-object counter (`Case::liveCount()`).

### Five types of inheritance
Each type is labelled with a comment in code and listed in the viva table:
1. **Single** — `DigitalEvidence : Evidence`
2. **Multilevel** — `Person → User → Investigator → CyberInvestigator`
3. **Multiple** — `CyberInvestigator : Investigator, ICertifiable`
4. **Hierarchical** — `Evidence → Digital/Physical/Testimonial`; `Investigator → Cyber/Field`
5. **Hybrid** — `TaskForceLead : CyberInvestigator, FieldInvestigator` (diamond problem solved with virtual inheritance; the demo prints and compares addresses to show exactly one `Investigator` sub-object)

The demos also show constructor/destructor order across levels and public/protected/private access in inheritance.

### Polymorphism
- **Compile-time (overloading)** — `ReportService::generate(...)` has four overloads (case summary, evidence report, investigator activity report, summary + evidence); constructors and `EvidenceService::addEvidence(...)` are overloaded for JSON, file path and raw buffer input.
- **Run-time (overriding)** — `std::vector<std::unique_ptr<Evidence>>` dispatches `analyzeEvidence()` to three different behaviours. `Person::describe()` is overridden at every level, and one non-virtual call through a base pointer contrasts static and dynamic binding.

### Virtual functions & abstract classes
Pure virtuals (`analyzeEvidence`, `verifyIntegrity`, `generateReport`, `clone`), virtual destructor, `override` everywhere, `final` on `TestimonialEvidence`, a second interface `IReportable` (implemented by `Case`, `Evidence`, `Report`), `EvidenceFactory::create(type, json)`, and an isolated sample showing that deleting through a base pointer without a virtual destructor is undefined behaviour.

### Operator overloading
Exposed at `GET /api/demo/operators`.

| Operator | Class | Purpose |
|---|---|---|
| `==` / `!=` | `Evidence` | Equal if hash and id match |
| `<=>` (or `<`) | `Case` | Order by priority and created date |
| `<<` (friend) | `Case`, `Evidence`, `Investigator` | Formatted output for logs and reports |
| `+=` | `Case` | `case += std::move(evidencePtr);` adds evidence |
| `[]` | `Case` | i-th evidence, bounds-checked, throws on error |
| `()` | `EvidenceFilter` (functor) | Matches a type/date predicate |
| `=` | `Case`, `RawBuffer` | Copy and move assignment |
| `++` (pre/post) | `Investigator` | Increment workload counter |
| `+` | Duration / Workload helper | Combine values |

### Exception handling
```
AppException (std::runtime_error, carries HTTP status)
├── DuplicateCaseError          409
├── CaseClosedError             409
├── InvalidEvidenceError        400
├── UnauthorizedAccessError     401 / 403
├── InvalidInvestigatorError    404
├── IntegrityViolationError     422  (hash mismatch)
└── DatabaseError               500
```
- Catch by `const&`, most specific to most general; `noexcept` on destructors and move operations.
- Demonstrates rethrow (`throw;`), exception translation (`pqxx::sql_error` → `DatabaseError`), and stack unwinding with RAII (the log proves destructors ran).
- A central `safeHandle(...)` wrapper maps exceptions to HTTP responses so controllers stay clean.

### Pointers & memory management
| Pointer kind | Where used | Why |
|---|---|---|
| `std::unique_ptr<T>` | `Case::evidence_`, `EvidenceFactory` | Exclusive ownership of polymorphic objects |
| `std::shared_ptr<T>` | `Case` shared by `CaseService`, `CaseCache`, investigators | Shared ownership with ref counting |
| `std::weak_ptr<T>` | `Investigator → Case` back-references | Break `shared_ptr` cycles |
| Raw pointer (non-owning) | Observer parameters, `Evidence*` views | Borrowed, never `delete`d |
| Raw owning pointer | `RawBuffer` (`new[]` / `delete[]`) | Manual memory management (Rule of Five) |
| Function pointer / `std::function` | Pluggable hash/validation strategy | Pointers to functions |
| Pointer to member (optional) | Sorting by different getters | Pointer to member function |

Also demonstrated: pointer arithmetic on `RawBuffer`, `nullptr` checks, `const T*` vs `T* const`, an isolated demo of a dangling pointer / double free / leak with fixed versions (clean under ASan and Valgrind), and `std::move` ownership transfer from the upload handler into `Case`. `GET /api/demo/memory` returns live object counts, `use_count()` values and the single virtual-base address.

### Multithreading, mutexes and deadlock
| Component | Concurrency feature |
|---|---|
| `ConnectionPool` | `std::mutex` + `std::condition_variable` (blocks when empty) |
| `AuditLogger` | `std::mutex` + `std::lock_guard` so log lines never interleave |
| `CaseCache` | `std::shared_mutex` — `shared_lock` for readers, `unique_lock` for writers |
| `IntegrityScanner` | Background `std::jthread` re-verifies hashes periodically; `std::atomic<bool>` stop flag; clean join in destructor |
| `POST /api/evidence/verify-all` | `std::async` / `std::future` parallel hashing |
| `Investigator::workload` | `std::atomic<int>` lock-free counter |
| Per-investigator mutex | Used by the deadlock demo |

**Deadlock demo** — `POST /api/demo/deadlock?mode=unsafe|safe` transfers a case between two investigators from two threads:
- `unsafe`: thread 1 locks A then B, thread 2 locks B then A → circular wait, detected via `try_lock_for(2s)` so the server never hangs.
- `safe`: `std::scoped_lock` acquires both locks atomically (deadlock avoidance).
- The response reports the mode, duration, whether deadlock was detected, and the four Coffman conditions (mutual exclusion, hold-and-wait, no preemption, circular wait) with the one the fix breaks.

Also demonstrated: a race condition (unsynchronized counter) fixed with `std::mutex`/`std::atomic`, and `lock_guard` vs `unique_lock` vs `scoped_lock` each used at least once.

## Backend Architecture

```
backend/
├── CMakeLists.txt
├── include/
│   ├── Person.h  User.h  Investigator.h  CyberInvestigator.h
│   ├── FieldInvestigator.h  TaskForceLead.h  ICertifiable.h  IReportable.h
│   ├── Evidence.h  DigitalEvidence.h  PhysicalEvidence.h  TestimonialEvidence.h
│   ├── Case.h  Suspect.h  Witness.h  Report.h
│   └── RawBuffer.h  Exceptions.h  Operators.h
├── src/
│   ├── models/         # all class implementations
│   ├── controllers/    # Auth, Case, Evidence, Investigator, Report, Demo
│   ├── services/       # AuthService, CaseService, EvidenceService,
│   │                   # HashService, ReportService, EvidenceFactory
│   ├── concurrency/    # ConnectionPool, AuditLogger, CaseCache, IntegrityScanner
│   ├── database/       # DBConnection, Repositories/{Case,Evidence,User,Investigator}Repository
│   ├── middleware/     # AuthMiddleware, ErrorHandler
│   ├── routes/         # Router.cpp (Crow route registration)
│   └── main.cpp
└── tests/              # one test file per OOP concept
```

Clean layering: **models / controllers / services / repositories / concurrency**.

## Database Design

PostgreSQL with native ENUM types, foreign keys with `ON DELETE` / `ON UPDATE` policies, and transactions for multi-step writes such as assignment and transfer.

| Table | Columns |
|---|---|
| `users` | `user_id` (SERIAL PK), `name`, `email` (UNIQUE), `password` (hashed), `role` (ENUM: ADMIN, INVESTIGATOR) |
| `cases` | `case_id` (SERIAL PK), `title` (UNIQUE), `description`, `status` (ENUM: OPEN, UNDER_INVESTIGATION, CLOSED), `priority` (INT), `created_date` (TIMESTAMP) |
| `evidence` | `evidence_id` (SERIAL PK), `case_id` (FK), `type` (ENUM: DIGITAL, PHYSICAL, TESTIMONIAL), `description`, `collected_by`, `hash` (TEXT), `metadata` (JSONB), `created_date` |
| `investigators` | `investigator_id` (SERIAL PK), `user_id` (FK), `specialization` (ENUM: CYBER, FIELD, TASK_FORCE), `availability` (BOOLEAN), `workload` (INT) |
| `case_assignments` | `case_id` (FK), `investigator_id` (FK), `assigned_date` — composite PK on both IDs |
| `suspects` | `suspect_id` (SERIAL PK), `case_id` (FK), `name`, `details` |
| `audit_log` | `log_id` (SERIAL PK), `timestamp`, `thread_id`, `level`, `message` |

## API Reference

### Authentication
| Method | Endpoint |
|---|---|
| POST | `/api/auth/register` |
| POST | `/api/auth/login` |

### Cases
| Method | Endpoint |
|---|---|
| GET | `/api/cases` |
| POST | `/api/cases` |
| PUT | `/api/cases/:id` |
| DELETE | `/api/cases/:id` |
| POST | `/api/cases/:id/assign` |

### Evidence
| Method | Endpoint | Notes |
|---|---|---|
| POST | `/api/evidence/upload` | |
| GET | `/api/evidence/:caseId` | |
| POST | `/api/evidence/analyze` | Polymorphic analysis |
| POST | `/api/evidence/verify-all` | Parallel, `std::async` |

### Investigators & Reports
| Method | Endpoint | Notes |
|---|---|---|
| GET | `/api/investigators` | |
| POST | `/api/investigators/transfer` | Uses `scoped_lock` |
| GET | `/api/reports/case/:id` | |
| GET | `/api/reports/evidence/:id` | |
| GET | `/api/reports/investigator/:id` | |

### System & OOP Demo (ADMIN only)
| Method | Endpoint | Purpose |
|---|---|---|
| GET | `/api/system/threads` | Worker count, scanner status, pool usage |
| GET | `/api/demo/oop` | All five inheritance types, virtual dispatch, overloads |
| GET | `/api/demo/lifecycle` | Constructor and destructor order log |
| GET | `/api/demo/operators` | Every overloaded operator |
| GET | `/api/demo/memory` | Smart-pointer counts, `RawBuffer` copy and move |
| POST | `/api/demo/deadlock?mode=unsafe\|safe` | Deadlock cause and fix |

## Frontend Pages

Vanilla HTML/CSS/JS, responsive (Grid/Flexbox, media queries).

| Page | Purpose |
|---|---|
| `login.html` | Authentication form; role read from JWT payload |
| `admin-dashboard.html` | Total cases, active investigators, pending investigations, plus a Concurrency panel (thread status, deadlock demo buttons) |
| `investigator-dashboard.html` | Assigned cases, evidence analysis, reports |
| `case-details.html` | Case info, suspects, evidence, timeline, "Verify all" button |
| `oop-lab.html` | Buttons that call each `/api/demo/*` endpoint and render the JSON, so every concept can be shown live in the viva |

JS structure: `js/api.js`, `js/auth.js` (sessionStorage / in-memory token), `js/dashboard.js`, `js/caseDetails.js`, `js/oopLab.js`.

## Non-Functional Requirements

- Responsive UI; input validation on client and server.
- JWT security and hashed passwords.
- Custom exception hierarchy mapped to HTTP status codes.
- Database consistency through constraints and transactions.
- **Memory safety:** zero leaks and zero errors under ASan/Valgrind for the test suite.
- **Thread safety:** no data races under ThreadSanitizer; no unbounded blocking (all lock waits in demos have timeouts).
- Every overriding function uses `override`; every polymorphic base has a virtual destructor.

## Testing & Verification

Build presets: `debug-asan`, `debug-tsan`, `release`. CI-style checklist: all tests pass under both sanitizer presets, plus a Valgrind leak summary.

| Test file | Covers |
|---|---|
| `test_constructors.cpp` | All constructor kinds, copy depth, destructor order, live counter |
| `test_inheritance.cpp` | Five inheritance types, diamond single sub-object |
| `test_polymorphism.cpp` | Overload resolution, virtual dispatch, `clone()` |
| `test_operators.cpp` | Every overloaded operator, including `operator[]` bounds error |
| `test_exceptions.cpp` | Hierarchy, rethrow, status mapping, stack unwinding with RAII |
| `test_memory.cpp` | `RawBuffer` Rule of Five, smart-pointer ownership, `weak_ptr` cycle break |
| `test_threads.cpp` | Pool, logger, cache under many threads |
| `test_deadlock.cpp` | Unsafe mode detects deadlock, safe mode completes |

## Build & Run

**Prerequisites:** C++17/20 compiler, CMake, PostgreSQL, and the dependencies above (Crow, libpqxx, nlohmann/json, OpenSSL, jwt-cpp) via vcpkg or Conan.

```bash
# Configure and build (release)
cd backend
cmake --preset release
cmake --build --preset release

# Run the backend
./build/release/casefile

# Run tests under sanitizers
cmake --preset debug-asan && cmake --build --preset debug-asan && ctest --preset debug-asan
cmake --preset debug-tsan && cmake --build --preset debug-tsan && ctest --preset debug-tsan

# Valgrind leak check
valgrind --leak-check=full ./build/debug-asan/tests/casefile_tests
```

Serve the frontend (any static file server works) and open `login.html`:

```bash
cd frontend
python3 -m http.server 8000
```

> Adjust preset names, binary paths and the database connection settings to match your local `CMakePresets.json` and environment.

## Documentation

- [Problem Statement](./problem-statement.md)
- [Use Case Specifications](docs/use-cases.md)
- [Use Case Diagram (PlantUML source)](docs/use-case-diagram.puml)
- CaseFile v2 Product Requirements Document (PRD)
