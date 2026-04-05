#pragma once

class FaultRuntime {
public:
    static void install_default_broadcasters();
    static void reset_for_testing();

private:
    static bool defaults_installed;
};
