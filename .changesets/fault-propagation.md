release: minor
summary: Add fault propagation to FaultController for sending FAULT order to remote peers and suppressing re-propagation on received faults

Star-topology design: the `!faulted` guard in `request_fault()` is the only loop breaker. Each board propagates exactly once on the `!faulted → faulted` transition, so no `fault_received_from_peer` flag is needed. Works for any node faulting first (local fault or CS-commanded fault).
