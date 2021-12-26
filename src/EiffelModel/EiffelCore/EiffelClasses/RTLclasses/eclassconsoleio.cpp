#include "eclassconsoleio.h"

#include "../../../EiffelFeatureInfo/eattribute.h"
#include "../../../EiffelFeatureInfo/eroutine.h"

std::string EClassCONSOLEIO::classRTLname() { return "CONSOLEIO"; }

EClassCONSOLEIO::EClassCONSOLEIO() {
    this->_initSelf();
}

void EClassCONSOLEIO::_defineParents() {}
void EClassCONSOLEIO::_defineCreators() {}
void EClassCONSOLEIO::_defineFeatures() {
    std::vector<EFeature> features;

    this->_addFeature( std::make_shared<ERoutine>(ERoutine("PUT_BOOLEAN", this, EType::voidType(), {{"input_value", EType::boolType()}}, {})) );
    this->_addFeature( std::make_shared<ERoutine>(ERoutine("PUT_INTEGER", this, EType::voidType(), {{"input_value", EType::intType()}}, {})) );
    this->_addFeature( std::make_shared<ERoutine>(ERoutine("PUT_CHARACTER", this, EType::voidType(), {{"input_value", EType::charType()}}, {})) );
    this->_addFeature( std::make_shared<ERoutine>(ERoutine("PUT_STRING", this, EType::voidType(), {{"input_value", EType::stringType()}}, {})) );
    this->_addFeature( std::make_shared<ERoutine>(ERoutine("NEW_LINE", this, EType::voidType(), {}, {})) );

    this->_addFeature( std::make_shared<ERoutine>(ERoutine("READ_BOOLEAN", this, EType::voidType(), {}, {})) );
    this->_addFeature( std::make_shared<ERoutine>(ERoutine("READ_INTEGER", this, EType::voidType(), {}, {})) );
    this->_addFeature( std::make_shared<ERoutine>(ERoutine("READ_CHARACTER", this, EType::voidType(), {}, {})) );
    this->_addFeature( std::make_shared<ERoutine>(ERoutine("READ_LINE", this, EType::voidType(), {}, {})) );

    this->_addFeature( std::make_shared<EAttribute>(EAttribute("LAST_BOOLEAN", this, EType::boolType())) );
    this->_addFeature( std::make_shared<EAttribute>(EAttribute("LAST_INTEGER", this, EType::intType())) );
    this->_addFeature( std::make_shared<EAttribute>(EAttribute("LAST_CHARACTER", this, EType::charType())) );
    this->_addFeature( std::make_shared<EAttribute>(EAttribute("LAST_STRING", this, EType::stringType())) );
}

std::string EClassCONSOLEIO::name() const { return EClassCONSOLEIO::classRTLname(); }
