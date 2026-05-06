release: patch
summary: MPUDomain dynamic regions were configured using the linker symbol values instead of their addresses (that is the correct way of using linker symbols). That made non-cached regions cached, and caused harware undefined behaviour.
