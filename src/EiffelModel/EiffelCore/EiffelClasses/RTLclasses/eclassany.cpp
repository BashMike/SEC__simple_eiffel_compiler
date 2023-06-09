#include "eclassany.h"

#include "../../../EiffelFeatureInfo/eattribute.h"

std::string EClassANY::classRTLname() { return "ANY"; }
std::string EClassANY::classRTLfullName() { return EClass::rtlClassPackageName() + "/" + EClassANY::classRTLname(); }

EClassANY::EClassANY() {
    this->_initSelf();
}

void EClassANY::_defineParents() {}
void EClassANY::_defineCreators() {}
void EClassANY::_defineFeatures() {
    this->_addFeature( std::make_shared<EAttribute>(EAttribute("IO", this, EType("CONSOLEIO"))) );
}

std::string EClassANY::name() const { return EClassANY::classRTLname(); }
