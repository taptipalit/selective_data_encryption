//===- SaberAnnotator.h -- Annotating LLVM IR---------------------------------//
//
//                     SVF: Static Value-Flow Analysis
//
// Copyright (C) <2013-2017>  <Yulei Sui>
//

// This program is free software: you can redistribute it and/or modify
// it under the terms of the GNU General Public License as published by
// the Free Software Foundation, either version 3 of the License, or
// (at your option) any later version.

// This program is distributed in the hope that it will be useful,
// but WITHOUT ANY WARRANTY; without even the implied warranty of
// MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
// GNU General Public License for more details.

// You should have received a copy of the GNU General Public License
// along with this program.  If not, see <http://www.gnu.org/licenses/>.
//
//===----------------------------------------------------------------------===//

/*
 * SaberAnnotator.h
 *
 *  Created on: May 4, 2014
 *      Author: Yulei Sui
 */

#ifndef SABERANNOTATOR_H_
#define SABERANNOTATOR_H_

#include "llvm/Analysis/SVF/Util/BasicTypes.h"
#include "llvm/Analysis/SVF/Util/Annotator.h"
#include <llvm/IR/Instructions.h>
#include <llvm/IR/CallSite.h>

class ProgSlice;
class SVFGNode;
/*!
 * Saber annotation
 */
class SaberAnnotator : public Annotator {

private:
    const ProgSlice* _curSlice;
public:
    /// Constructor
    SaberAnnotator(ProgSlice* slice): _curSlice(slice) {

    }
    /// Destructor
    virtual ~SaberAnnotator() {

    }
    /// Annotation
    //@{
    void annotateSource();
    void annotateSinks();
    void annotateFeasibleBranch(const llvm::BranchInst *brInst, u32_t succPos);
    void annotateInfeasibleBranch(const llvm::BranchInst *brInst, u32_t succPos);

    void annotateSwitch(llvm::SwitchInst *brInst, u32_t succPos);
    //@}
};


#endif /* SABERANNOTATOR_H_ */
