release: major
summary: Redesign fault handling, protections, and Board bootstrap around explicit fault policies

This PR changes the public integration contract for applications built on ST-LIB.

Breaking changes:

- `Board` now takes the fault policy type as its first template parameter.
- The global `FAULT` runtime is owned exclusively by `FaultController`.
- User state machines are now nested under the global `OPERATIONAL` state through `FaultPolicy` or `FaultPolicyNoMachine`.
- Protections now use `ProtectionEngine` and `Protections::Rules::*`; the previous `ProtectionManager` and boundary split is no longer the active model.
- Runtime reporting is unified under `PANIC(...)`, `FAULT(...)`, `WARNING(...)`, and `INFO(...)`.
- The real bootstrap path is `Board::init()`. Legacy `STLIB::start()`, `STLIB::update()`, `STLIB_LOW::start()`, and `STLIB_HIGH::start()` must not be used as the integration path.

Migration notes:

- Declare the board as `Board<YourFaultPolicy, ...>`.
- Use `FaultPolicy<app_machine, on_fault_enter>` when you want an operational state machine nested under the global runtime.
- Use `FaultPolicyNoMachine<on_fault_enter>` when you only need a fault-entry callback.
- Use `DefaultFaultPolicy` when you want neither an operational machine nor a fault-entry callback.
- In the main loop, drive the runtime through `FaultController::check_transitions()`, `ProtectionEngine::evaluate()`, and `Diagnostics::Hub::flush()`.
