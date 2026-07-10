release: minor
summary: Add variable-length TCP orders, diagnostic forwarding, and flush throttling

- `TcpOrderStreamParser` now supports variable-length orders: when an order's `get_size()` returns 0, all remaining stream data is passed to `parse()`, and the post-parse size is used for byte consumption. A `reset_for_receive()` hook is called after consumption so orders can reset state for the next packet.
- `Order` gains a virtual `reset_for_receive()` no-op that subclasses can override.
- `Socket` constructor no longer panics on `ERR_RTE` during `tcp_connect()` — sets `pending_connection_reset` instead, letting the connection retry when the route becomes available.
- `OrderProtocolDiagnosticSink` now accepts a target `OrderProtocol*` via its constructor, so diagnostic records are sent to a single socket instead of broadcasting to all open sockets.
- `DiagnosticTransportOrder` now parses incoming diagnostic orders (IDs 1555, 2555, 3000) and re-publishes them via `Hub::publish_runtime_*()`, enabling forwarding of remote board diagnostics to the control station.
- `Diagnostics::Hub::flush_pending` throttles records to at most one per 100 ms per priority level (urgent and non-urgent tracked separately) to prevent flooding sinks.
- Default TCP keepalive values tightened: 30 ms idle, 20 ms interval, 2 probes (commented out for now since it can't keep connection with the backend).
- `lwipopts.h`: `MEMP_NUM_SYS_TIMEOUT` increased by 10 to avoid pool exhaustion with SNTP and whatever else it may happen, `TCP_TMR_INTERVAL` reduced to 10 ms for faster keepalive response.
- Added `Diagnostics::install_ethernet_sink(OrderProtocol*)` to register the Ethernet sink with a specific target socket.
