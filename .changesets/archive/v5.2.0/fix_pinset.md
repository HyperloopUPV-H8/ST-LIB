release: patch
summary: RXER pin for Ethernet is now optional

H11 ethernet doesn't use RXER pin. The RXER pin is now a pointer so it can hold a nullptr. Ethernet constructor now has a std::optional for this pin
