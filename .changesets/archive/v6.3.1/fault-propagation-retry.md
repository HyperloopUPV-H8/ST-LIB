release: patch
summary: Retry pending FAULT order propagation to disconnected peers via check_transitions()

When propagate_fault() fails to send the FAULT order (e.g. TCP connection not yet
established), the target is marked pending. check_transitions() automatically
retries pending targets on each call while the board remains faulted, until the
send succeeds.
