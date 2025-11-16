#include <cstdio>
#include <fstream>

#include "lexer/lexer.hpp"
#include "parser/parser.hpp"
#include "genIR/genIR.hpp"
#include "error/error.hpp"
#include "llvm/Support/CommandLine.h"
#include "llvm/MC/TargetRegistry.h"
#include "llvm/Support/TargetSelect.h"
#include "llvm/Target/TargetMachine.h"
#include "llvm/Target/TargetOptions.h"
#include "llvm/TargetParser/Host.h"
#include "llvm/IR/LegacyPassManager.h"
#include "llvm/Config/llvm-config.h"

llvm::cl::opt<std::string> inputSource(
    llvm::cl::Positional,
    llvm::cl::desc("<input source>"),
    llvm::cl::Required);
llvm::cl::opt<std::string> OutputFile(
    "o",
    llvm::cl::desc("output file"),
    llvm::cl::value_desc("filename"),
    llvm::cl::init("out.o"));
llvm::cl::opt<bool> isOutputAST(
    "ast",
    llvm::cl::desc("Output AST."),
    // llvm::cl::value_desc("filename"),
    llvm::cl::init(false));
llvm::cl::opt<bool> isOutputIR(
    "ir",
    llvm::cl::desc("Output LLVM IR."),
    // llvm::cl::value_desc("filename"),
    llvm::cl::init(false));

int main(int argc, char **argv)
{
    llvm::cl::ParseCommandLineOptions(argc, argv, "hinalang compiler");

    std::ifstream f(inputSource);
    if (!f)
    {
        Error::err("File '%s' open failed.", inputSource.c_str());
        return 1;
    }

    lexer lex(f);

    parser par(lex);
    ProgramAST *program = par.parseProgram();

    f.close();

    if (isOutputAST)
    {
        FILE *f2 = fopen(OutputFile.c_str(), "w");

        if (!f2)
        {
            Error::err("File '%s' save failed. (err: %s)", OutputFile.c_str(), strerror(errno));
            return 1;
        }
        program->dump(0, f2);
        fclose(f2);

        return 0;
    }

    genIR gen;
    gen.generate(program);

    if (isOutputIR)
    {
        std::error_code errorCode;
        llvm::raw_fd_ostream stream(OutputFile, errorCode);

        if (stream.has_error())
        {
            /* code */
            Error::err("File '%s' save failed. (err: %s)", OutputFile.c_str(), errorCode.message().c_str());
        }

        gen.getModule()->print(stream, nullptr);

        return 0;
    }

    // オブジェクトファイルの生成
    llvm::InitializeAllTargetInfos();
    llvm::InitializeAllTargets();
    llvm::InitializeAllTargetMCs();
    llvm::InitializeAllAsmParsers();
    llvm::InitializeAllAsmPrinters();

    std::string targetTripleStr = llvm::sys::getDefaultTargetTriple();
#if LLVM_VERSION_MAJOR < 21
    gen.getModule()->setTargetTriple(targetTripleStr);
#else
    // llvm 21でもコンパイルできるように修正
    gen.getModule()->setTargetTriple(llvm::Triple(targetTripleStr));
#endif
    std::string error;
    const llvm::Target *target = llvm::TargetRegistry::lookupTarget(targetTripleStr, error);
    if (!target)
    {
        llvm::errs() << error;
        Error::err("can't lookup target (triple: %s)", targetTripleStr.c_str());
        return 1;
    }

    std::string cpu = "generic";
    std::string features = "";

    llvm::TargetOptions targetOpt;
#if LLVM_VERSION_MAJOR < 21
    auto targetMachine = target->createTargetMachine(
        targetTripleStr, cpu, features, targetOpt, llvm::Reloc::PIC_);
#else
    // llvm 21でもコンパイルできるように修正
    auto targetMachine = target->createTargetMachine(
        llvm::Triple(targetTripleStr), cpu, features, targetOpt, llvm::Reloc::PIC_);
#endif
    gen.getModule()->setDataLayout(targetMachine->createDataLayout());

    std::error_code errorCode;
    llvm::raw_fd_ostream stream(OutputFile, errorCode);

    if (stream.has_error())
    {
        /* code */
        Error::err("File '%s' save failed. (err: %s)", OutputFile.c_str(), errorCode.message().c_str());
    }

    llvm::legacy::PassManager pass;
    targetMachine->addPassesToEmitFile(pass, stream, nullptr, llvm::CodeGenFileType::ObjectFile);
    pass.run(*gen.getModule());
    stream.flush();

    // gen.getModule()->print(stream, nullptr);

    return 0;
}
