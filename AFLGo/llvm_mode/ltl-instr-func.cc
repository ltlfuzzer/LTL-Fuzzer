#include "ltl-instr-func.h"

std::vector<Value*> instr::InstrFunc::gbptr_vec;     // store global variable pointers 
std::vector<Value*> instr::InstrFunc::gbsize_vec;    // store global variable size
std::vector<Value*> instr::InstrFunc::lcptr_vec;     // store local variable pointers
std::vector<Value*> instr::InstrFunc::lcsize_vec;    // store local variable size
std::vector<std::string> instr::InstrFunc::gbname_vec;    // store global variable names
std::vector<std::string> instr::InstrFunc::lcname_vec;    // store local varibale names

IntegerType* instr::InstrFunc::i32;
IntegerType* instr::InstrFunc::i64;
PointerType* instr::InstrFunc::i32_ptr;
PointerType* instr::InstrFunc::i64_ptr;
PointerType* instr::InstrFunc::i8_ptr;

void instr::InstrFunc::getDebugLoc(const Instruction *I, std::string &Filename,
                        unsigned &Line) {
#ifdef LLVM_OLD_DEBUG_API
    DebugLoc Loc = I->getDebugLoc();
    if (!Loc.isUnknown()) {
        DILocation cDILoc(Loc.getAsMDNode(M.getContext()));
        DILocation oDILoc = cDILoc.getOrigLocation();

        Line = oDILoc.getLineNumber();
        Filename = oDILoc.getFilename().str();

        if (filename.empty()) {
            Line = cDILoc.getLineNumber();
            Filename = cDILoc.getFilename().str();
        }
    }
#else
    if (DILocation *Loc = I->getDebugLoc()) {
        Line = Loc->getLine();
        Filename = Loc->getFilename().str();

        if (Filename.empty()) {
            DILocation *oDILoc = Loc->getInlinedAt();
            if (oDILoc) {
                Line = oDILoc->getLine();
                Filename = oDILoc->getFilename().str();
            }
        }
    }
#endif /* LLVM_OLD_DEBUG_API */
}

void instr::InstrFunc::initTypes(Module &M){
    /** 
        Types of Arguments 
    **/
    i32 = llvm::IntegerType::get(M.getContext(), 32);
    i64 = llvm::IntegerType::get(M.getContext(), 64);
    i32_ptr = llvm::IntegerType::getInt32PtrTy(M.getContext());
    i64_ptr = llvm::IntegerType::getInt64PtrTy(M.getContext());
    i8_ptr  = llvm::IntegerType::getInt8PtrTy(M.getContext());
}

void instr::InstrFunc::storeGlobalVariables(Module &M){
    /** 
        Get global variables 
    **/
    gbptr_vec.clear();
    gbsize_vec.clear();

    for(auto gv_iter = M.global_begin();gv_iter != M.global_end(); gv_iter++){
        GlobalVariable *gv = &*gv_iter;
        std::string gName = (gv->getName()).str();
        //errs() << "gbname: " << gName << "\n";
        if(!gName.empty() && gName != "__afl_area_ptr" && gName != "__afl_prev_loc" && gName.find(".str") == std::string::npos &&  gName.find("std") == std::string::npos){
            int gsize = gv->getAlignment();
            Value *ssize = ConstantInt::get(Type::getInt32Ty(M.getContext()), gsize);
            gbptr_vec.push_back(&*gv_iter);
            gbsize_vec.push_back(ssize);
            gbname_vec.push_back(gName);
        }
    }
}

void instr::InstrFunc::storeLocalVariables(Module &M, Instruction &I){
    /** 
        Get local variables 
    **/
    if ( DbgDeclareInst *dbg = dyn_cast<DbgDeclareInst>(&I)){
        if (AllocaInst *sinst = dyn_cast<AllocaInst>(dbg->getAddress())){  // find alloca instructions
            
            DILocalVariable *DILocVar = dbg->getVariable();
            int lsize = sinst->getAlignment();
            Value *ssize = ConstantInt::get(Type::getInt32Ty(M.getContext()), lsize);
            std::string lcname = (DILocVar->getName()).str();
            
            lcptr_vec.push_back(&*sinst); 
            lcsize_vec.push_back(ssize);  
            lcname_vec.push_back(lcname);
        }
    }
}

void instr::InstrFunc::instrStateHandler(Module &M, Instruction &I){
    IRBuilder<> IRB(&(I));

    /** 
        Instrument the function: void state_handler(long *ptr_vec, int *size_vec, int size) 
    **/
    std::vector<Type*> args_state;
    args_state.push_back(i64_ptr);  // address pointer
    args_state.push_back(i32_ptr);  // size pointer
    args_state.push_back(i32);      // array size
    FunctionType *type_state = FunctionType::get(Type::getVoidTy(M.getContext()), args_state, false);
    auto func_state = (M.getOrInsertFunction("state_handler", type_state));

    size_t gb_size = gbsize_vec.size();
    size_t lc_size = lcsize_vec.size();
    size_t size = gb_size + lc_size;

    Type* arrayType64 = ArrayType::get(i64, size); 
    Type* arrayType32 = ArrayType::get(i32, size); 
    Value* ptr_arr = IRB.CreateAlloca(arrayType64);
    Value* size_arr = IRB.CreateAlloca(arrayType32);

    for(size_t i = 0; i < gb_size; i++){
        IRB.CreateStore(gbptr_vec[i], IRB.CreateConstGEP2_64(ptr_arr, 0, i));
        IRB.CreateStore(gbsize_vec[i], IRB.CreateConstGEP2_32(arrayType32, size_arr, 0, i));
    }

    for(size_t i = 0; i < lc_size; i++){
        IRB.CreateStore(lcptr_vec[i], IRB.CreateConstGEP2_64(ptr_arr, 0, i + gb_size));
        IRB.CreateStore(lcsize_vec[i], IRB.CreateConstGEP2_32(arrayType32, size_arr, 0, i + gb_size));
    }

    Value *ssize = ConstantInt::get(Type::getInt32Ty(M.getContext()), size);

    std::vector<Value*> func_args;
    func_args.push_back(ptr_arr);
    func_args.push_back(size_arr);
    func_args.push_back(ssize);
    IRB.CreateCall(func_state, func_args);

}

void instr::InstrFunc::instrAutomataHandler(Module &M, Function &F, Instruction &I, Value* input, Value* output){
    /** 
        Instrument the function: void automata_handler(int input, int output) 
    **/
    IRBuilder<> IRB(&(I));

    std::vector<Type*> args_auta;
    args_auta.push_back(i32);
    args_auta.push_back(i32);
    FunctionType *type_auta = FunctionType::get(Type::getVoidTy(M.getContext()), args_auta, false);
    auto func_auta = (M.getOrInsertFunction("automata_handler", type_auta));

    std::vector<Value*> func_args_auta;
    func_args_auta.push_back(input);
    func_args_auta.push_back(output);
    IRB.CreateCall(func_auta, func_args_auta);
}

void instr::InstrFunc::instrEvaluateTrace(Module &M, Instruction &I,  int flag){
    /** 
        Instrument the function: void evaluate_trace(int flag) 
    **/
    IRBuilder<> IRB(&(I)); 

    std::vector<Type*> args_eva;
    args_eva.push_back(i32);
    FunctionType *type_eva = FunctionType::get(Type::getVoidTy(M.getContext()), args_eva, false);
    auto func_eva = (M.getOrInsertFunction("evaluate_trace", type_eva));

    Value *fval = ConstantInt::get(Type::getInt32Ty(M.getContext()), flag);
    std::vector<Value*> func_args_eva;
    func_args_eva.push_back(fval);
    IRB.CreateCall(func_eva, func_args_eva);
}

void instr::InstrFunc::clearLocalVariables(){
    /** 
        Clear local containers when switching to differnet functions 
    **/
    lcptr_vec.clear();
    lcsize_vec.clear();
    lcname_vec.clear();
}

void instr::InstrFunc::printLocalVariables(std::string loc_name){
    errs() << "Print Local variables at " << loc_name << "\n";
    for(size_t i = 0; i < lcname_vec.size(); i++){
        errs() << "\tl" << i << ": " << lcname_vec[i] << "\n";
    }
}

void instr::InstrFunc::printGlobalVariables(std::string loc_name){
    errs() << "Print Global variables at " << loc_name << "\n";
    for(size_t i = 0; i < gbname_vec.size(); i++){
        errs() << "\tg" << i << ": " << gbname_vec[i] << "\n";
    }
}

void instr::InstrFunc::instrInputHandler(Module &M, Instruction &I, std::string event){
     
    //    Instrument the function: void input_handler(const char* input)  
    
    IRBuilder<> IRB(&(I));

    std::vector<Type*> arg_types;
    arg_types.push_back(i8_ptr);
    FunctionType *type_input = FunctionType::get(Type::getVoidTy(M.getContext()), arg_types, false);
    auto func_input = (M.getOrInsertFunction("input_handler", type_input));

    Value* etPtr = IRB.CreateGlobalStringPtr(event);
    const std::vector<llvm::Value *> args_input{etPtr};
    IRB.CreateCall(func_input, args_input);
    
}

void instr::InstrFunc::instrPropHandler(Module &M, Instruction &I, std::string event){
    /** 
        Instrument the function: void proposition_handler(const char* prop) 
    **/
    IRBuilder<> IRB(&(I));

    std::vector<Type*> arg_types;
    arg_types.push_back(i8_ptr);
    FunctionType *type_prop = FunctionType::get(Type::getVoidTy(M.getContext()), arg_types, false);
    auto func_prop = (M.getOrInsertFunction("proposition_handler", type_prop));

    Value* etPtr = IRB.CreateGlobalStringPtr(event);
    const std::vector<llvm::Value *> args_prop{etPtr};
    IRB.CreateCall(func_prop, args_prop);
    
}

Value *instr::InstrFunc::instrBeginTime(Module &M, Instruction &I){
    /**
        Instrument the function: long begin_time();
    **/
    IRBuilder<> IRB(&(I));
    FunctionType *type_begin = FunctionType::get(Type::getInt64Ty(M.getContext()), {}, false);
    auto func_begin = (M.getOrInsertFunction("begin_time", type_begin));

    Value *ret = IRB.CreateCall(func_begin, {});

    return ret;
}

void instr::InstrFunc::instrEndTime(Module &M, Instruction &I, Value* bval){
    /**
        Instrument the function: void end_time(long btime);
    **/
    IRBuilder<> IRB(&(I));
    std::vector<Type*> arg_types;
    arg_types.push_back(i64);
    FunctionType *type_end = FunctionType::get(Type::getInt64Ty(M.getContext()), arg_types, false);
    auto func_end = (M.getOrInsertFunction("end_time", type_end));

    IRB.CreateCall(func_end, {bval});
}
