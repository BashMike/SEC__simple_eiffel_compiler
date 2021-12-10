#ifndef ERROR_CODES_H
#define ERROR_CODES_H

enum SemanticErrorCode {
    // First semantic stage error codes
    CLASSES__NAME_CLASHES_WITH_OTHER_CLASS_NAME,

    // Second semantic stage error codes
    FEATURES__NAME_CLASHES_WITH_CLASS_NAME,
    FEATURES__NAME_CLASHES_WITH_NAME_OF_SAME_CLASS_FEATURE,

    FEATURES__FORMAL_PARAM_NAME_CLASHES_WITH_CLASS_NAME,
    FEATURES__FORMAL_PARAM_NAME_CLASHES_WITH_NAME_OF_SELF_FEATURE,
    FEATURES__FORMAL_PARAM_NAME_CLASHES_WITH_NAME_OF_SAME_CLASS_FEATURE,
    FEATURES__FORMAL_PARAM_NAME_CLASHES_WITH_NAME_OF_OTHER_FORMAL_PARAM,

    FEATURES__LOCAL_VAR_NAME_CLASHES_WITH_CLASS_NAME,
    FEATURES__LOCAL_VAR_NAME_CLASHES_WITH_NAME_OF_SELF_FEATURE,
    FEATURES__LOCAL_VAR_NAME_CLASHES_WITH_NAME_OF_SAME_CLASS_FEATURE,
    FEATURES__LOCAL_VAR_NAME_CLASHES_WITH_NAME_OF_FORMAL_PARAM,
    FEATURES__LOCAL_VAR_NAME_CLASHES_WITH_NAME_OF_OTHER_LOCAL_VAR,

    FEATURES__FEATURE_INVALID_TYPE,
    FEATURES__FORMAL_PARAM_INVALID_TYPE,
    FEATURES__LOCAL_VAR_INVALID_TYPE,

    // Third semantic stage error codes ...
    // ... Global error codes
    INHERITANCE__UNKNOWN_PARENT,
    INHERITANCE__INHERIT_CYCLE,
    FEATURES__NAME_CLASHES_WITH_NAME_OF_INHERITED_FEATURE,

    // ... Rename error codes
    INHERITANCE__TWO_OR_MORE_RENAMES_OF_SAME_FEATURE,
    INHERITANCE__RENAME_UKNOWN_FEATURE,

    // ... Redefine error codes
    INHERITANCE__REDEFINE_CLAUSE_FEATURE_DUPLICATED,
    INHERITANCE__REDEFINE_UNKNOWN_FEATURE,
    INHERITANCE__REDEFINE_WITHOUT_REDEFINING_IN_CLASS,
    INHERITANCE__NONCONFORMING_REDEFINE,

    // ... Select error codes
    INHERITANCE__SELECT_CLAUSE_FEATURE_DUPLICATED,
    INHERITANCE__SELECT_UNKNOWN_FEATURE,
    INHERITANCE__IMPROPER_SELECT,
    INHERITANCE__REPEATEDLY_INHERITED_FEATURE_VERSIONS_CONFLICT,

    // Fourth semantic stage error codes
};

#endif // ERROR_CODES_H
