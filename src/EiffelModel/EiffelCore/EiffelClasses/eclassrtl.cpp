#include "eclassrtl.h"

EClassRTL::EClassRTL() {}

std::string EClassRTL::javaPackageName() const { return EClass::rtlClassPackageName(); }
