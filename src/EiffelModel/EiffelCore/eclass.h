#ifndef ECLASSINFO_H
#define ECLASSINFO_H

#include <vector>
#include <map>
#include <string>
#include <memory>

#include "econstanttable.h"
#include "efeature.h"
#include "efeaturemetainfo.h"
#include "../../bison/tree_nodes.h"

class EClass {
    // ================= SUBTYPES =================
protected:
    struct EParentInfo {
        std::vector<std::pair<std::string, std::string>>    renameSeq;
        std::vector<std::string>                            redefineSeq;
        std::vector<std::string>                            selectSeq;
    };

private:
    enum EFeatureTableState {
        NOT_SETUP,
        IN_PROCESS,
        DONE
    };

    // ================ ATTRIBUTES ================
protected:
    std::map<std::string, EParentInfo>                  _parents;
    std::vector<std::string>                            _creators;
    std::map<std::string, std::shared_ptr<EFeature>>    _features;

    std::vector<EFeatureMetaInfo>       _featuresTable;
    EFeatureTableState                  _featuresTableState;

    // ================ OPERATIONS ================
    // ----------------- creating -----------------
public:
    EClass();

protected:
    void _initSelf();

    virtual void _defineParents() = 0;
    virtual void _defineCreators() = 0;
    virtual void _defineFeatures() = 0;

private:
    void _checkRenameAndSelectDuplicates() const;
    void _defineFeaturesTable();

public:
    void setupAcceptableFeaturesTable(const std::vector<std::string>& classInheritPath = {});

private:
    void _fillSelfFeaturesTableUsingParent(const EClass* parent, const EParentInfo& parentInfo);
    bool _checkOnlyExistFeaturesAreSelected();
    void _resolveSelects();
    void _validateSelfFeaturesTable() const;

    // ---------------- attributes ----------------
public:
    virtual std::string name() const = 0;
    virtual std::string javaPackageName() const = 0;

    const std::map<std::string, EParentInfo> parents() const;
    const std::map<std::string, std::shared_ptr<EFeature>> features() const;
};

#endif // ECLASSINFO_H
