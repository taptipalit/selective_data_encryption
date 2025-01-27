/*
 * MTAResultValidator.h
 *
 *  Created on: 29/06/2015
 *      Author: Peng Di and Ding Ye
 */

#ifndef MTARESULTVALIDATOR_H_
#define MTARESULTVALIDATOR_H_

#include <MemoryModel/PointerAnalysis.h>
#include "llvm/Analysis/SVF/MTA/TCT.h"
#include "llvm/Analysis/SVF/MTA/MHP.h"
#include "llvm/Analysis/SVF/Util/AnalysisUtil.h"
#include "llvm/Analysis/SVF/Util/ThreadCallGraph.h"

/*!
 * Validate the result of context-sensitive analysis, including context-sensitive
 * thread detection and thread interleaving.
 */
class MTAResultValidator {

public:
    typedef int INTERLEV_FLAG;
    MTAResultValidator(MHP* mh) :
        mhp(mh) {
        tcg = mhp->getThreadCallGraph();
        tdAPI = tcg->getThreadAPI();
    }
    // Destructor
    ~MTAResultValidator() {
    }

    // Analysis
    void analyze();

protected:

    /*
     * Assistant functions
     */

    // Split string
    std::vector<std::string> &split(const std::string &s, char delim, std::vector<std::string> &elems);
    std::vector<std::string> split(const std::string &s, char delim);

    // Get special arguments of given call sites
    NodeID getIntArg(const llvm::Instruction* inst, unsigned int arg_num);
    std::vector<std::string> getStringArg(const llvm::Instruction* inst, unsigned int arg_num);
    CallStrCxt getCxtArg(const llvm::Instruction* inst, unsigned int arg_num);

    /*
     * Get the previous LoadInst or StoreInst from Instruction "I" in the
     * same BasicBlock. Return NULL if none exists.
     */
    const llvm::Instruction *getPreviousMemoryAccessInst(const llvm::Instruction *I);

    // Compare two cxts
    bool matchCxt(const CallStrCxt cxt1, const CallStrCxt cxt2) const;

    // Dump calling context information
    void dumpCxt(const CallStrCxt& cxt) const;

    void dumpInterlev(NodeBS& lev);

    // Get the validation result string of a single validation scenario.
    inline std::string getOutput(const char *scenario, bool analysisRes);
    inline std::string getOutputforInterlevAnalysis(const char *scenario, INTERLEV_FLAG analysisRes);

    /*
     * Collect the callsite targets for validations.
     * The targets are labeled by "cs1:", "cs2:"... that are the names of its basic blocks.
     * The collected targets are stored in csnumToInstMap that maps label "cs1" to CallInst.
     */
    bool collectCallsiteTargets();

    /*
     * Collect the CxtThread targets for validations.
     * The collected targets are stored in vthdToCxt that maps vthd to cxt.
     */
    bool collectCxtThreadTargets();

    /*
     * Collect TCT targets for validations.
     * The collected targets are stored in rthdToChildren.
     */
    bool collectTCTTargets();

    /*
     * Collect the thread interleaving targets for validations.
     * The collected targets are stored in instToTSMap and threadStmtToInterLeaving.
     */
    bool collectInterleavingTargets();

    /*
     * Perform validation for Cxtthread.
     * If correct, the validator maps given thread vthd to static CxtThread rthd stored in vthdTorthd.
     */
    bool validateCxtThread();

    /*
     * Perform validation for TCT.
     */
    bool validateTCT();

    /*
     * Perform validation for thread interleaving.
     */
    INTERLEV_FLAG validateInterleaving();

private:

    std::map<NodeID, const llvm::CallInst*> csnumToInstMap;
    std::map<NodeID, CallStrCxt> vthdToCxt;
    std::map<NodeID, NodeID> vthdTorthd;
    std::map<NodeID, NodeID> rthdTovthd;

    std::map<NodeID, std::set<NodeID>> rthdToChildren;

    MHP::InstToThreadStmtSetMap instToTSMap; // Map a instruction to CxtThreadStmtSet
    MHP::ThreadStmtToThreadInterleav threadStmtToInterLeaving; /// Map a statement to its thread interleavings

    static constexpr char const *CXT_THREAD = "CXT_THREAD";
    static constexpr char const *INTERLEV_ACCESS = "INTERLEV_ACCESS";
    static constexpr char const *TCT_ACCESS = "TCT_ACCESS";

    ThreadAPI* tdAPI;
    ThreadCallGraph* tcg;
    MHP* mhp;

    /// Constant INTERLEV_FLAG values
    //@{
    static const INTERLEV_FLAG INTERLEV_TRUE = 0x01;
    static const INTERLEV_FLAG INTERLEV_IMPRECISE = 0x02;
    static const INTERLEV_FLAG INTERLEV_UNSOUND = 0x04;
    //@}
};



/*!
 * \brief Validate the result of concurrent analysis.
 *
 * The properties to validate of two memory accesses include
 * one or more of the following four:
 * (1) they may accesses aliases;
 * (2) they may happen in parallel;
 * (3) they are protected by common lock(s);
 * (4) they may cause a data race error.
 * The ground truth are specified by the "RC_ACCESS" function in the target program.
 *
 * Users may utilize this result validator to validate their analysis with
 * one or more of the four properties, by inheriting the RCResultValidator class.
 * The corresponding virtual function of the desired property should be overridden.
 */
class RaceResultValidator {
public:
    typedef int RC_FLAG;

    /*!
     * Data structure for recording access pairs for the validation.
     */
    class AccessPair {
    public:
        /// Constructor
        AccessPair(const llvm::Instruction *I1, const llvm::Instruction *I2,
                   const RC_FLAG flags) :
            I1(I1), I2(I2), flags(flags) {
        }

        /// Class member access
        //@{
        inline bool isFlaged(const RC_FLAG flag) const {
            return flags & flag;
        }
        inline const llvm::Instruction *getInstruction1() const {
            return I1;
        }
        inline const llvm::Instruction *getInstruction2() const {
            return I2;
        }
        //@}

    private:
        const llvm::Instruction *I1;
        const llvm::Instruction *I2;
        RC_FLAG flags;
    };

    /// Destructor
    virtual ~RaceResultValidator() {
        release();
    }

    /// Initialization
    void init(SVFModule M) {
        this->M = M;
        selectedValidationScenarios = RC_MHP | RC_ALIASES | RC_PROTECTED | RC_RACE;
        collectValidationTargets();
    }

    /// Analysis
    void analyze() {
        validateAll();
    }

    /// Release resource
    void release() {
    }

    /// Check if the input program has validation target
    inline bool hasValidationTarget() const {
        return !accessPairs.empty();
    }

protected:
    /// Interface to the specific validation properties.
    /// Override one or more to implement your own analysis.
    //@{
    virtual bool mayAccessAliases(const llvm::Instruction *I1,
                                  const llvm::Instruction *I2) {
        selectedValidationScenarios &= ~RC_ALIASES;
        return true;
    }
    virtual bool mayHappenInParallel(const llvm::Instruction *I1,
                                     const llvm::Instruction *I2) {
        selectedValidationScenarios &= ~RC_MHP;
        return true;
    }
    virtual bool protectedByCommonLocks(const llvm::Instruction *I1,
                                        const llvm::Instruction *I2) {
        selectedValidationScenarios &= ~RC_PROTECTED;
        return true;
    }
    virtual bool mayHaveDataRace(const llvm::Instruction *I1,
                                 const llvm::Instruction *I2) {
        selectedValidationScenarios &= ~RC_RACE;
        return true;
    }
    //@}

    /*!
     * Collect the targets for validations.
     * The targets should be memory access Instructions in pairs.
     * The collected targets are stored in the member variable "accessPairs".
     */
    void collectValidationTargets() {
        // Collect call sites of all RC_ACCESS function calls.
        std::vector<const llvm::CallInst*> csInsts;
        const llvm::Function *F = M.getFunction(RC_ACCESS);
        if (!F)     return;

        for (llvm::Value::const_use_iterator it = F->use_begin(), ie =
                    F->use_end(); it != ie; ++it) {
            const llvm::Use *u = &*it;
            const llvm::Value *user = u->getUser();
            const llvm::CallInst *csInst = llvm::dyn_cast<llvm::CallInst>(user);
            assert(csInst);
            csInsts.push_back(csInst);
        }
        assert(csInsts.size() % 2 == 0 && "We should have RC_ACCESS called in pairs.");

        // Sort the validation sites according to their ids.
        std::sort(csInsts.begin(), csInsts.end(), compare);

        // Generate access pairs.
        for (int i = 0, e = csInsts.size(); i != e;) {
            const llvm::CallInst *CI1 = csInsts[i++];
            const llvm::CallInst *CI2 = csInsts[i++];
            const llvm::ConstantInt *C = llvm::dyn_cast<llvm::ConstantInt>(CI1->getOperand(1));
            assert(C);
            const llvm::Instruction *I1 = getPreviousMemoryAccessInst(CI1);
            const llvm::Instruction *I2 = getPreviousMemoryAccessInst(CI2);
            assert(I1 && I2 && "RC_ACCESS should be placed immediately after the target memory access.");
            RC_FLAG flags = C->getZExtValue();
            accessPairs.push_back(AccessPair(I1, I2, flags));
        }
    }

    /// Perform validation for all targets.
    void validateAll() {
        llvm::outs() << analysisUtil::pasMsg(" --- Analysis Result Validation ---\n");

        // Iterate every memory access pair to perform the validation.
        for (int i = 0, e = accessPairs.size(); i != e; ++i) {
            const AccessPair &ap = accessPairs[i];
            const llvm::Instruction *I1 = ap.getInstruction1();
            const llvm::Instruction *I2 = ap.getInstruction2();

            bool mhp = mayHappenInParallel(I1, I2);
            bool alias = mayAccessAliases(I1, I2);
            bool protect = protectedByCommonLocks(I1, I2);
            bool racy = mayHaveDataRace(I1, I2);

            llvm::outs() << "For the memory access pair at ("
                         << analysisUtil::getSourceLoc(I1) << ", "
                         << analysisUtil::getSourceLoc(I2) << ")\n";
            if (selectedValidationScenarios & RC_ALIASES) {
                llvm::outs() << "\t"
                             << getOutput("ALIASES", alias, ap.isFlaged(RC_ALIASES))
                             << "\n";
            }
            if (selectedValidationScenarios & RC_MHP) {
                llvm::outs() << "\t"
                             << getOutput("MHP", mhp, ap.isFlaged(RC_MHP)) << "\n";
            }
            if (selectedValidationScenarios & RC_PROTECTED) {
                llvm::outs() << "\t"
                             << getOutput("PROTECT", protect,
                                          ap.isFlaged(RC_PROTECTED)) << "\n";
            }
            if (selectedValidationScenarios & RC_RACE) {
                llvm::outs() << "\t"
                             << getOutput("RACE", racy, ap.isFlaged(RC_RACE))
                             << "\n";
            }
        }

        llvm::outs() << "\n";
    }

    /// Get the validation result string of a single validation scenario.
    inline std::string getOutput(const char *scenario,
                                 bool analysisRes, bool expectedRes) {
        std::string ret(scenario);
        ret += "\t";
        if (expectedRes)
            ret += " T: ";
        else
            ret += " F: ";
        if (analysisRes == expectedRes)
            ret += analysisUtil::sucMsg("SUCCESS");
        else
            ret += analysisUtil::errMsg("FAILURE");
        return ret;
    }

private:
    SVFModule M;
    std::vector<AccessPair> accessPairs;
    RC_FLAG selectedValidationScenarios;

    /*!
     * Comparison function to sort the validation targets in ascending order of
     * the validation id (i.e., the 1st argument of RC_ACCESS function call).
     */
    static bool compare(const llvm::CallInst *CI1, const llvm::CallInst *CI2) {
        const llvm::Value *V1 = CI1->getOperand(0);
        const llvm::Value *V2 = CI2->getOperand(0);
        const llvm::ConstantInt *C1 = llvm::dyn_cast<llvm::ConstantInt>(V1);
        const llvm::ConstantInt *C2 = llvm::dyn_cast<llvm::ConstantInt>(V2);
        assert(0 != C1 && 0 != C2);
        return C1->getZExtValue() < C2->getZExtValue();
    }

    /*!
     * Get the previous LoadInst or StoreInst from Instruction "I" in the
     * same BasicBlock.
     * Return NULL if none exists.
     */
    const llvm::Instruction *getPreviousMemoryAccessInst(
        const llvm::Instruction *I) {
        I = I->getPrevNode();
        while (I) {
            if (llvm::isa<llvm::LoadInst>(I) || llvm::isa<llvm::StoreInst>(I))
                return I;
            if (const llvm::Function *callee = analysisUtil::getCallee(I)) {
                if (ExtAPI::EFT_L_A0__A0R_A1R == ExtAPI::getExtAPI()->get_type(callee)
                        || callee->getName().find("llvm.memset") != llvm::StringRef::npos)
                    return I;
            }
            I = I->getPrevNode();
        }
        return NULL;
    }

    /// Constant RC_FLAG values
    //@{
    static const RC_FLAG RC_MHP = 0x01;
    static const RC_FLAG RC_ALIASES = 0x02;
    static const RC_FLAG RC_PROTECTED = 0x04;
    static const RC_FLAG RC_RACE = 0x10;
    //@}

    /// The name of the function which is used to specify the ground truth
    /// of the validation properties in the target program.
    static constexpr char const *RC_ACCESS = "RC_ACCESS";
};

#endif /* MTARESULTVALIDATOR_H_ */
