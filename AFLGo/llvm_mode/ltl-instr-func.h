#ifndef LTL_INSTR_FUNC_H
#define LTL_INSTR_FUNC_H

#include <string.h>
#include <sstream>
#include <list>
#include <vector>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>

#include "llvm/ADT/Statistic.h"
#include "llvm/ADT/ArrayRef.h"
#include "llvm/IR/IRBuilder.h"
#include "llvm/IR/LegacyPassManager.h"
#include "llvm/IR/Module.h"
#include "llvm/IR/DebugInfoMetadata.h"
#include "llvm/IR/DebugLoc.h"
#include "llvm/IR/Instructions.h"
#include "llvm/IR/IntrinsicInst.h"
#include "llvm/IR/GlobalVariable.h"
#include "llvm/IR/Type.h"
#include "llvm/Transforms/IPO/PassManagerBuilder.h"
#include "llvm/Support/Debug.h"
#include "llvm/Support/CommandLine.h"
#include "llvm/Support/FileSystem.h"
#include "llvm/Support/raw_ostream.h"
#include "llvm/Support/Casting.h"
#include "llvm/Analysis/CFGPrinter.h"
#include "llvm/IR/DerivedTypes.h"

using namespace llvm;

namespace instr{
    class InstrFunc{
        public:
            static void storeGlobalVariables(Module &M);
            static void storeLocalVariables(Module &M, Instruction &I);
            static void clearLocalVariables();
            static void printLocalVariables(std::string loc_name);
            static void printGlobalVariables(std::string loc_name);

            static void initTypes(Module &M);
            static void instrStateHandler(Module &M, Instruction &I);
            static void instrEvaluateTrace(Module &M, Instruction &I, int flag); 

            static void instrAutomataHandler(Module &M, Function &F, Instruction &I, Value* input, Value* output);

            static void instrInputHandler(Module &M, Instruction &I, std::string event);
            static void instrPropHandler(Module &M, Instruction &I, std::string event);

            static void getDebugLoc(const Instruction *I, std::string &Filename, unsigned &Line);
            static Value *instrBeginTime(Module &M, Instruction &I);
            static void instrEndTime(Module &M, Instruction &I, Value* bval);

        private:

            static IntegerType* i32;
            static IntegerType* i64;
            static PointerType* i32_ptr;
            static PointerType* i64_ptr;
            static PointerType* i8_ptr;

            static std::vector<Value*> gbptr_vec;     // store global variable pointers 
            static std::vector<Value*> gbsize_vec;    // store global variable size
            static std::vector<Value*> lcptr_vec;     // store local variable pointers
            static std::vector<Value*> lcsize_vec;    // store local variable size
            static std::vector<std::string> gbname_vec; // store global variable names
            static std::vector<std::string> lcname_vec; // store local varibale names
            
    };
}

#endif
