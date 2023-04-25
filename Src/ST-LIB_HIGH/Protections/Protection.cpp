#include "Protections/Protection.hpp"
#include "ErrorHandler/ErrorHandler.hpp"

map<ProtectionType, string> BoundaryInterface::protection_type_name = { {ProtectionType::BELOW,"minimo"},
                                                                        {ProtectionType::ABOVE,"maximo"},
                                                                        {ProtectionType::EQUALS, "deberia ser distinto al"},
                                                                        {ProtectionType::NOT_EQUALS, "deberia ser igual al"},
                                                                        {ProtectionType::OUT_OF_RANGE, "minimo y maximo"}
};
