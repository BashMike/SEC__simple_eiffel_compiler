#include "bytecode.h"
#include "../EiffelCore/eclass.h"
#include "../EiffelCore/EiffelClasses/eclassconsoleio.h"
#include "../EiffelCore/eprogram.h"

ByteCode::ByteCode() {}

#include <iostream>

ByteCode::ByteCode(const EUserClass& userClass) {
    // Main bytecode values
    this->_appendFourBytes(0xCAFEBABE);     // "Magic" java constant
    this->_appendTwoBytes(0x0000);          // Minor version of java
    this->_appendTwoBytes(0x003C);          // Major version of java

    // Bytecode of constant table
    ByteCode constantTableByteCode(userClass._constants);
    this->_append(constantTableByteCode);

    // Self class info
    this->_appendTwoBytes(0x0001);          // Access flags (0x0001 - PUBLIC class)
    this->_appendTwoBytes(userClass._constants.searchClassConstBy(userClass.fullName())); // Link to self constant class in constant table
    this->_appendTwoBytes(userClass._constants.searchClassConstBy(EClass::javaObjectFullName())); // Link to parent constantn class in constant table (java.lang.Object is parent)
    this->_appendTwoBytes(0x0000);          // Count of interfaces which class implements

    // Fields info
    const auto& userClassAttributesMetaInfo = userClass.attributesMetaInfo();
    this->_appendTwoBytes(userClassAttributesMetaInfo.size());

    for (const auto& attributeMetaInfo : userClassAttributesMetaInfo) {
        this->_append(ByteCode::attributesMetaByteCode(userClass._constants, *attributeMetaInfo));
    }

    // Methods info
    const auto& userClassRoutinesMetaInfo = userClass.routinesMetaInfo();
    this->_appendTwoBytes(userClassRoutinesMetaInfo.size() + userClass._featuresTable.size() + 1 + userClass.isMainClass());
    // this->_appendTwoBytes(userClassRoutinesMetaInfo.size() + 1);

    // ... Compile default constructor to bytecode
    this->_append(ByteCode::defaultConstructorByteCode(userClass._constants, userClass));

    // ... Compile methods to bytecode
    for (const auto& routineMetaInfo : userClassRoutinesMetaInfo) {
        this->_append(ByteCode::routinesMetaByteCode(userClass._constants, *routineMetaInfo));
    }

    // ... Compile polymethods to bytecode
    for (const auto& featureMetaInfo : userClass._featuresTable) {
        this->_append(ByteCode::polyMethodByteCode(userClass._constants, featureMetaInfo));
    }

    // ... Compile main function to bytecode if user class is the main class
    if (userClass.isMainClass()) {
        this->_append(ByteCode::mainFunctionByteCode(userClass._constants, userClass));
    }

    // Class attributes info
    this->_appendTwoBytes(0x0000); // Class attributes count
}

ByteCode::ByteCode(const EConstantTable& classConstantTable) {
    this->_appendTwoBytes(classConstantTable.size()+1);

    for (int i=1; i<=classConstantTable.size(); i++) {
        const auto& jvmConstantInfo = classConstantTable.getConstant(i);

        this->_appendByte(jvmConstantInfo.first);

        std::vector<char> stringBytes;
        switch (jvmConstantInfo.first) {
            case EConstantTable::jvm_utf8:
                this->_appendTwoBytes(jvmConstantInfo.second.jvm_utf8->length());

                stringBytes = std::vector<char>(jvmConstantInfo.second.jvm_utf8->begin(), jvmConstantInfo.second.jvm_utf8->end());
                for (char stringByte : stringBytes) {
                    this->_appendByte(stringByte);
                }
                break;

            case EConstantTable::jvm_string:
                this->_appendTwoBytes(jvmConstantInfo.second.jvm_string);
                break;

            case EConstantTable::jvm_class:
                this->_appendTwoBytes(jvmConstantInfo.second.jvm_class);
                break;

            case EConstantTable::jvm_nameAndType:
                this->_appendTwoBytes(jvmConstantInfo.second.jvm_nameAndType.first);
                this->_appendTwoBytes(jvmConstantInfo.second.jvm_nameAndType.second);
                break;

            case EConstantTable::jvm_fieldRef:
                this->_appendTwoBytes(jvmConstantInfo.second.jvm_fieldRef.first);
                this->_appendTwoBytes(jvmConstantInfo.second.jvm_fieldRef.second);
                break;

            case EConstantTable::jvm_methodRef:
                this->_appendTwoBytes(jvmConstantInfo.second.jvm_methodRef.first);
                this->_appendTwoBytes(jvmConstantInfo.second.jvm_methodRef.second);
                break;

            default:
                break;
        }
    }
}

ByteCode ByteCode::attributesMetaByteCode(const EConstantTable& userClassConstants, const EFeatureMetaInfo& featureMetaInfo) {
    ByteCode result;
    result._appendTwoBytes(0x0001);
    result._appendTwoBytes(featureMetaInfo.featureName_utf8Link());
    result._appendTwoBytes(featureMetaInfo.featureDescriptor_utf8Link());
    result._appendTwoBytes(0x0000);

    return result;
}

ByteCode ByteCode::routinesMetaByteCode(const EConstantTable& userClassConstants, const EFeatureMetaInfo& featureMetaInfo) {
    ERoutine* classRoutine = (ERoutine*)featureMetaInfo.implementation();

    ByteCode result;
    result._appendTwoBytes(0x0001);
    result._appendTwoBytes(featureMetaInfo.featureName_utf8Link());
    result._appendTwoBytes(featureMetaInfo.featureDescriptor_utf8Link());
    result._appendTwoBytes(0x0001);

    // "Code" attribute of method
    ByteCode codeAttribute;
    codeAttribute._appendTwoBytes(0x1000);
    codeAttribute._appendTwoBytes((short)classRoutine->_formalParameters.size() + (short)classRoutine->_localVariables.size() + 1);

    ByteCode routineBodyCode(userClassConstants, classRoutine->_routineBody);

    codeAttribute._appendFourBytes((unsigned long)routineBodyCode._bytes.size());
    codeAttribute._append(routineBodyCode);
    codeAttribute._appendFourBytes(0x00000000);

    result._appendTwoBytes(0x0001);
    result._appendFourBytes((unsigned long)codeAttribute._bytes.size());
    result._append(codeAttribute);

    return result;
}

ByteCode ByteCode::mainFunctionByteCode(const EConstantTable& userClassConstants, const EUserClass& mainClass) {
    ByteCode result;
    result._appendTwoBytes(0x0009);
    result._appendTwoBytes(userClassConstants.searchUtf8By(EProgram::javaMainFunctionName()));
    result._appendTwoBytes(userClassConstants.searchUtf8By(EProgram::javaMainFunctionDescriptor()));
    result._appendTwoBytes(0x0001);

    // "Code" attribute of method
    ByteCode codeAttribute;
    codeAttribute._appendTwoBytes(0x1000);
    codeAttribute._appendTwoBytes(0x02);

    // ByteCode routineBodyCode(userClassConstants, routineInfo->_routineBody);
    ByteCode routineBodyCode;
    // Create main class
    routineBodyCode._append(ByteCode::new_(userClassConstants.searchClassConstBy(mainClass.fullName())));
    routineBodyCode._append(ByteCode::dup());
    routineBodyCode._append(ByteCode::invokespecial(userClassConstants.searchMethodRefBy(mainClass.fullName(), EProgram::javaDefaultConstructorName(), EProgram::javaDefaultConstructorDescriptor()), 0, 0));
    routineBodyCode._append(ByteCode::astore(0x01));

    // Run main method of main class
    routineBodyCode._append(ByteCode::aload(0x01));
    routineBodyCode._append(ByteCode::invokevirtual(userClassConstants.searchMethodRefBy(mainClass.fullName(), EProgram::eiffelMainFunctionName(), EProgram::javaDefaultConstructorDescriptor()), 0, 0));

    routineBodyCode._append(ByteCode::_return());

    codeAttribute._appendFourBytes((unsigned long)routineBodyCode._bytes.size());
    codeAttribute._append(routineBodyCode);
    codeAttribute._appendFourBytes(0x00000000);

    result._appendTwoBytes(0x0001);
    result._appendFourBytes((unsigned long)codeAttribute._bytes.size());
    result._append(codeAttribute);

    return result;
}

ByteCode ByteCode::polyMethodByteCode(const EConstantTable& userClassConstants, const EFeatureMetaInfo& featureMetaInfo) {
    ByteCode result;

    result._appendTwoBytes(0x0001);
    result._appendTwoBytes(featureMetaInfo.polyMethodName_utf8Link());
    result._appendTwoBytes(featureMetaInfo.polyMethodDescriptor_utf8Link());
    result._appendTwoBytes(0x0001);

    // "Code" attribute of method
    ByteCode codeAttribute;
    codeAttribute._appendTwoBytes(0x1000);

    short formalParamsCount = 0;
    if (featureMetaInfo.implementation()->featureType() == EFeature::efeature_routine) {
        formalParamsCount = ((ERoutine*)featureMetaInfo.implementation())->_formalParameters.size();
    }

    codeAttribute._appendTwoBytes(1 + formalParamsCount);

    ByteCode featureMetaInfoBodyCode;
    for (const auto& polymorphicFeatureInfo : featureMetaInfo.polyMethodImplementations()) {
        featureMetaInfoBodyCode._append(ByteCode::aload(0x0));
        featureMetaInfoBodyCode._append(ByteCode::instanceof(polymorphicFeatureInfo.first));

        std::vector<ByteCode> ifElseBlocks;

        ByteCode ifBlock;
        ifBlock._append(ByteCode::aload(0));
        for (short i=0; i<formalParamsCount; i++) {
            ifBlock._append(ByteCode::aload(i+1));
        }

        ifBlock._append((polymorphicFeatureInfo.second.first == EFeature::efeature_attribute
                         ? ByteCode::getfield(polymorphicFeatureInfo.second.second)
                         : ByteCode::invokevirtual(polymorphicFeatureInfo.second.second, formalParamsCount, false)));

        switch (featureMetaInfo.returnType()) {
            case EFeatureMetaInfo::ereturntype_void:    ifBlock._append(ByteCode::_return()); break;
            case EFeatureMetaInfo::ereturntype_integer: ifBlock._append(ByteCode::ireturn()); break;
            case EFeatureMetaInfo::ereturntype_object:  ifBlock._append(ByteCode::areturn()); break;
        }

        featureMetaInfoBodyCode._append(ByteCode::ifeq(0x03 + ifBlock._bytes.size()));
        featureMetaInfoBodyCode._append(ifBlock);
    }
    featureMetaInfoBodyCode._append(ByteCode::_return());

    codeAttribute._appendFourBytes((unsigned long)featureMetaInfoBodyCode._bytes.size());
    codeAttribute._append(featureMetaInfoBodyCode);
    codeAttribute._appendFourBytes(0x00000000);

    result._appendTwoBytes(0x0001);
    result._appendFourBytes((unsigned long)codeAttribute._bytes.size());
    result._append(codeAttribute);

    return result;
}

ByteCode ByteCode::defaultConstructorByteCode(const EConstantTable& userClassConstants, const EUserClass& userClass) {
    ByteCode result;
    result._appendTwoBytes(0x0001);
    result._appendTwoBytes(userClassConstants.searchUtf8By(EProgram::javaDefaultConstructorName()));
    result._appendTwoBytes(userClassConstants.searchUtf8By(EProgram::javaDefaultConstructorDescriptor()));
    result._appendTwoBytes(0x0001);

    // "Code" attribute of method
    ByteCode codeAttribute;
    codeAttribute._appendTwoBytes(0x1000);
    codeAttribute._appendTwoBytes(0x01);

    ByteCode routineBodyCode;
    // Call this.super
    routineBodyCode._append(ByteCode::aload(0x0));
    routineBodyCode._append(ByteCode::invokespecial(userClassConstants.searchMethodRefBy(EClass::javaObjectFullName(),
                                                                                         EProgram::javaDefaultConstructorName(),
                                                                                         EProgram::javaDefaultConstructorDescriptor()
                                                                                         ),
                                                    0,
                                                    0));

    // Initialize IO variable
    routineBodyCode._append(ByteCode::aload(0x0));
    routineBodyCode._append(ByteCode::new_(userClassConstants.searchClassConstBy("rtl/CONSOLEIO")));
    routineBodyCode._append(ByteCode::dup());
    routineBodyCode._append(ByteCode::invokespecial(userClassConstants.searchMethodRefBy("rtl/CONSOLEIO", "<init>", "()V"), 0, 0));
    short fieldRef = userClassConstants.searchFieldRefBy(userClass.fullName(), "io", "Lrtl/CONSOLEIO;");
    routineBodyCode._append(ByteCode::putfield(fieldRef));

    routineBodyCode._append(ByteCode::_return());

    codeAttribute._appendFourBytes((unsigned long)routineBodyCode._bytes.size());
    codeAttribute._append(routineBodyCode);
    codeAttribute._appendFourBytes(0x00000000);

    result._appendTwoBytes(0x0001);
    result._appendFourBytes((unsigned long)codeAttribute._bytes.size());
    result._append(codeAttribute);

    return result;

    return result;
}

ByteCode::ByteCode(const EConstantTable& userClassConstants, const instruction_seq_strct* routineBody) {
    this->_append(ByteCode::_return());
}

ByteCode& ByteCode::_appendByte(unsigned char value) {
    this->_bytes.push_back(value);
    return *this;
}

ByteCode& ByteCode::_appendTwoBytes(unsigned short int value) {
    for (int i=1; i>=0; i--) {
        unsigned char eachByte = *(((char*)(&value)) + i);
        this->_bytes.push_back(eachByte);
    }

    return *this;
}

ByteCode& ByteCode::_appendFourBytes(unsigned long int value) {
    for (int i=3; i>=0; i--) {
        unsigned char eachByte = *(((char*)(&value)) + i);
        this->_bytes.push_back(eachByte);
    }

    return *this;
}

ByteCode& ByteCode::_append(const ByteCode& other) {
    for (char byteValue : other._bytes) {
        this->_bytes.push_back(byteValue);
    }

    return *this;
}

bool ByteCode::writeToFile(std::ofstream& outputClassFile) const {
    if (outputClassFile.is_open()) {
        outputClassFile.clear();

        for (char byteValue : this->_bytes) {
            outputClassFile.write(reinterpret_cast<char*>(&byteValue), sizeof(byteValue));
        }

        return true;
    }
    else {
        return false;
    }
}

ByteCode ByteCode::iconst(signed char i) {
    ByteCode result;

    if ( i < -1 || i > 5) { throw "Invalid parameter for \"iconst\" command (must be an integer in [-1; 5])"; }

    result._appendByte(i+3);
    return result;
}

ByteCode ByteCode::bipush(char s1) {
    ByteCode result;
    result._appendByte(0x10);
    result._appendByte(s1);

    return result;
}

ByteCode ByteCode::sipush(short int s2) {
    ByteCode result;
    result._appendByte(0x11);
    result._appendTwoBytes(s2);

    return result;
}

ByteCode ByteCode::ldc(char u1) {
    ByteCode result;
    result._appendByte(0x12);
    result._appendByte(u1);

    return result;
}

ByteCode ByteCode::ldc_w(short int u2) {
    ByteCode result;
    result._appendByte(0x13);
    result._appendTwoBytes(u2);

    return result;
}

ByteCode ByteCode::iload(char u1) {
    ByteCode result;
    result._appendByte(0x15);
    result._appendByte(u1);

    return result;
}

ByteCode ByteCode::aload(char u1) {
    ByteCode result;
    result._appendByte(0x19);
    result._appendByte(u1);

    return result;
}

ByteCode ByteCode::istore(char u1) {
    ByteCode result;
    result._appendByte(0x36);
    result._appendByte(u1);

    return result;
}

ByteCode ByteCode::astore(char u1) {
    ByteCode result;
    result._appendByte(0x3A);
    result._appendByte(u1);

    return result;
}

ByteCode ByteCode::pop() {
    ByteCode result;
    result._appendByte(0x57);

    return result;
}

ByteCode ByteCode::dup() {
    ByteCode result;
    result._appendByte(0x59);

    return result;
}

ByteCode ByteCode::dup2() {
    ByteCode result;
    result._appendByte(0x5C);

    return result;
}

ByteCode ByteCode::iadd() {
    ByteCode result;
    result._appendByte(0x60);

    return result;
}

ByteCode ByteCode::imul() {
    ByteCode result;
    result._appendByte(0x68);

    return result;
}

ByteCode ByteCode::isub() {
    ByteCode result;
    result._appendByte(0x64);

    return result;
}

ByteCode ByteCode::ineg() {
    ByteCode result;
    result._appendByte(0x74);

    return result;
}

ByteCode ByteCode::idiv() {
    ByteCode result;
    result._appendByte(0x6C);

    return result;
}

ByteCode ByteCode::iinc(char u1, signed char i) {
    ByteCode result;
    result._appendByte(0x84);
    result._appendByte(u1);
    result._appendByte(i);

    return result;
}

ByteCode ByteCode::if_icmpeq(short int s2) {
    ByteCode result;
    result._appendByte(0x9F);
    result._appendTwoBytes(s2);

    return result;
}

ByteCode ByteCode::if_icmpne(short int s2) {
    ByteCode result;
    result._appendByte(0xA0);
    result._appendTwoBytes(s2);

    return result;
}

ByteCode ByteCode::if_icmplt(short int s2) {
    ByteCode result;
    result._appendByte(0xA1);
    result._appendTwoBytes(s2);

    return result;
}

ByteCode ByteCode::if_icmpge(short int s2) {
    ByteCode result;
    result._appendByte(0xA2);
    result._appendTwoBytes(s2);

    return result;
}

ByteCode ByteCode::if_icmpgt(short int s2) {
    ByteCode result;
    result._appendByte(0xA3);
    result._appendTwoBytes(s2);

    return result;
}

ByteCode ByteCode::if_icmple(short int s2) {
    ByteCode result;
    result._appendByte(0xA4);
    result._appendTwoBytes(s2);

    return result;
}

ByteCode ByteCode::ifeq(short int s2) {
    ByteCode result;
    result._appendByte(0x99);
    result._appendTwoBytes(s2);

    return result;
}

ByteCode ByteCode::ifne(short int s2) {
    ByteCode result;
    result._appendByte(0x9A);
    result._appendTwoBytes(s2);

    return result;
}

ByteCode ByteCode::iflt(short int s2) {
    ByteCode result;
    result._appendByte(0x9B);
    result._appendTwoBytes(s2);

    return result;
}

ByteCode ByteCode::ifle(short int s2) {
    ByteCode result;
    result._appendByte(0x9E);
    result._appendTwoBytes(s2);

    return result;
}

ByteCode ByteCode::ifgt(short int s2) {
    ByteCode result;
    result._appendByte(0x9D);
    result._appendTwoBytes(s2);

    return result;
}

ByteCode ByteCode::ifge(short int s2) {
    ByteCode result;
    result._appendByte(0x9C);
    result._appendTwoBytes(s2);

    return result;
}

ByteCode ByteCode::if_acmpeq(short int s2) {
    ByteCode result;
    result._appendByte(0xA5);
    result._appendTwoBytes(s2);

    return result;
}

ByteCode ByteCode::if_acmpne(short int s2) {
    ByteCode result;
    result._appendByte(0xA6);
    result._appendTwoBytes(s2);

    return result;
}

ByteCode ByteCode::goto_(short int s2) {
    ByteCode result;
    result._appendByte(0xA7);
    result._appendTwoBytes(s2);

    return result;
}

ByteCode ByteCode::newarray(char u1) {
    ByteCode result;
    result._appendByte(0xBC);
    result._appendByte(u1);

    return result;
}

ByteCode ByteCode::anewarray(short int u2) {
    ByteCode result;
    result._appendByte(0xBD);
    result._appendTwoBytes(u2);

    return result;
}

ByteCode ByteCode::arraylength() {
    ByteCode result;
    result._appendByte(0xBE);

    return result;
}

ByteCode ByteCode::iaload() {
    ByteCode result;
    result._appendByte(0x2E);

    return result;
}

ByteCode ByteCode::aaload() {
    ByteCode result;
    result._appendByte(0x32);

    return result;
}

ByteCode ByteCode::iastore() {
    ByteCode result;
    result._appendByte(0x4F);
    return result;
}

ByteCode ByteCode::aastore() {
    ByteCode result;
    result._appendByte(0x53);
    return result;
}

ByteCode ByteCode::new_(short int u2) {
    ByteCode result;
    result._appendByte(0xBB);
    result._appendTwoBytes(u2);

    return result;
}

ByteCode ByteCode::getfield(short int u2) {
    ByteCode result;
    result._appendByte(0xB4);
    result._appendTwoBytes(u2);

    return result;
}

ByteCode ByteCode::putfield(short int u2) {
    ByteCode result;
    result._appendByte(0xB5);
    result._appendTwoBytes(u2);

    return result;
}

ByteCode ByteCode::instanceof(short int u2) {
    ByteCode result;
    result._appendByte(0xC1);
    result._appendTwoBytes(u2);

    return result;
}

ByteCode ByteCode::invokevirtual(short int u2, short int argCount, bool isVoid) {
    ByteCode result;
    result._appendByte(0xB6);
    result._appendTwoBytes(u2);

    return result;
}

ByteCode ByteCode::invokespecial(short int u2, short int argCount, bool isVoid) {
    ByteCode result;
    result._appendByte(0xB7);
    result._appendTwoBytes(u2);

    return result;
}

ByteCode ByteCode::invokestatic(short int u2, short int argCount, bool isVoid) {
    ByteCode result;
    result._appendByte(0xB8);
    result._appendTwoBytes(u2);

    return result;
}

ByteCode ByteCode::ireturn() {
    ByteCode result;
    result._appendByte(0xAC);

    return result;
}

ByteCode ByteCode::areturn() {
    ByteCode result;
    result._appendByte(0xB0);

    return result;
}

ByteCode ByteCode::_return() {
    ByteCode result;
    result._appendByte(0xB1);

    return result;
}

/*
=== EXAMPLE OF BYTE CODE STRUCTURE =======

    this->_appendFourBytes(0xCAFEBABE);     // "Magic" java constant
    this->_appendTwoBytes(0x0000);          // Minor version of java
    this->_appendTwoBytes(0x003C);          // Major version of java

    // Bytecode of constant table
    ((EUserClass*)userClass)->_constants.appendUtf8("<init>");
    ((EUserClass*)userClass)->_constants.appendUtf8("()V");
    ((EUserClass*)userClass)->_constants.appendNameAndType({19, 20});
    ((EUserClass*)userClass)->_constants.appendUtf8("java/lang/Object");
    ((EUserClass*)userClass)->_constants.appendConstClass(22);
    ((EUserClass*)userClass)->_constants.appendMethodRef({23, 21});
    ((EUserClass*)userClass)->_constants.appendUtf8("<init>");
    ((EUserClass*)userClass)->_constants.appendUtf8("()V");
    ((EUserClass*)userClass)->_constants.appendNameAndType({6, 7});
    ((EUserClass*)userClass)->_constants.appendMethodRef({5, 8});

    ((EUserClass*)userClass)->_constants.appendUtf8("main");
    ((EUserClass*)userClass)->_constants.appendUtf8("([Ljava/lang/String;)V");
    ((EUserClass*)userClass)->_constants.appendUtf8("java/lang/System");
    ((EUserClass*)userClass)->_constants.appendConstClass(12);
    ((EUserClass*)userClass)->_constants.appendUtf8("out");
    ((EUserClass*)userClass)->_constants.appendUtf8("Ljava/io/PrintStream;");
    ((EUserClass*)userClass)->_constants.appendNameAndType({14, 15});
    ((EUserClass*)userClass)->_constants.appendFieldRef({13, 16});
    ((EUserClass*)userClass)->_constants.appendUtf8("java/io/PrintStream");
    ((EUserClass*)userClass)->_constants.appendConstClass(18);
    ((EUserClass*)userClass)->_constants.appendUtf8("println");
    ((EUserClass*)userClass)->_constants.appendUtf8("(Ljava/lang/String;)V");
    ((EUserClass*)userClass)->_constants.appendNameAndType({20, 21});
    ((EUserClass*)userClass)->_constants.appendMethodRef({19, 22});
    ((EUserClass*)userClass)->_constants.appendUtf8("Hello! It\'s me, " + userClass->fullName() + "!\n");
    ((EUserClass*)userClass)->_constants.appendString(24);

    ByteCode constantTableByteCode(&userClass->_constants);
    this->_append(constantTableByteCode);

    // Self class info
    this->_appendTwoBytes(0x0001);          // Access flags (0x0001 - PUBLIC class)
    this->_appendTwoBytes(userClass->_constants.searchClassConstBy(userClass->fullName())); // Link to self constant class in constant table
    this->_appendTwoBytes(userClass->_constants.searchClassConstBy(EClass::javaObjectFullName())); // Link to parent constantn class in constant table (java.lang.Object is parent)
    this->_appendTwoBytes(0x0000);          // Count of interfaces which class implements
    // Fields info
    this->_appendTwoBytes(0x0000);

    // Methods info
    this->_appendTwoBytes(0x0002); // Methods count

    // Default constructor ...
    // ... Main settings
    this->_appendTwoBytes(0x0001);  // Access flags (PUBLIC)
    this->_appendTwoBytes(0x0006);  // Link to utf8 string with name of method
    this->_appendTwoBytes(0x0007);  // Link to utf8 string with descriptor of method
    this->_appendTwoBytes(0x0001);  // Attributes count
    this->_appendTwoBytes(0x0001);  // Attribute index ("Code" attribute)
    this->_appendFourBytes(0x0011); // Attribute length (in bytes)
    this->_appendTwoBytes(0x0080); // max stack size (in bytes)
    this->_appendTwoBytes(0x0080); // max local size (in bytes)
    this->_appendFourBytes(0x5); // "Code" length (in bytes)

    // ... Code
    // Aload "this"
    this->_appendByte(0x2A);

    // Invokespecial
    this->_appendByte(0xB7);
    this->_appendTwoBytes(0x0009);

    // Return
    this->_appendByte(0xB1);

    // ... End of method
    this->_appendTwoBytes(0x0000); // Length of exception (in bytes)
    this->_appendTwoBytes(0x0000); // Attributes count

    // "main" function
    // ... Main settings
    this->_appendTwoBytes(0x0009);  // Access flags (PUBLIC + STATIC)
    this->_appendTwoBytes(0x000A);  // Link to utf8 string with name of method
    this->_appendTwoBytes(0x000B);  // Link to utf8 string with descriptor of method
    this->_appendTwoBytes(0x0001);  // Attributes count
    this->_appendTwoBytes(0x0001);  // Attribute index ("Code" attribute)
    this->_appendFourBytes(0x15);   // Attribute length (in bytes)
    this->_appendTwoBytes(0x0010);  // max stack size (in bytes)
    this->_appendTwoBytes(0x0010);  // max local size (in bytes)
    this->_appendFourBytes(0x09);   // "Code" length (in bytes)

    // ... Code
    // Getstatic "out"
    this->_appendByte(0xB2);
    this->_appendTwoBytes(0x0011);

    // Ldc "string"
    this->_appendByte(0x12);
    this->_appendByte(0x19);

    // Invokevirtual
    this->_appendByte(0xB6);
    this->_appendTwoBytes(0x0017);

    // Return
    this->_appendByte(0xB1);

    // ... End of method
    this->_appendTwoBytes(0x0000); // Length of exception (in bytes)
    this->_appendTwoBytes(0x0000); // Attributes count

    this->_appendTwoBytes(0x0000); // Class attributes count

*/
