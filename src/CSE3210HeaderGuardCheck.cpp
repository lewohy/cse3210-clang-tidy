#include "clang-tidy/ClangTidy.h"
#include "clang-tidy/ClangTidyModule.h"
#include "clang-tidy/ClangTidyModuleRegistry.h"
#include "clang-tidy/utils/HeaderGuard.h"
#include "clang/Tooling/Tooling.h"
#include <iostream>
#include <string>

using namespace clang;
using namespace clang::tidy;
using namespace clang::ast_matchers;

const char DefaultHeaderGuardPrefix[] = "";

class CSE3210HeaderGuardCheck : public utils::HeaderGuardCheck {
public:
  CSE3210HeaderGuardCheck(StringRef Name, ClangTidyContext *Context);
  void storeOptions(ClangTidyOptions::OptionMap &Opts) override;
  bool shouldSuggestEndifComment(StringRef Filename) override { return false; }
  std::string getHeaderGuard(StringRef Filename, StringRef OldGuard) override;

private:
  const std::string HeaderGuardPrefix;
};

CSE3210HeaderGuardCheck::CSE3210HeaderGuardCheck(StringRef Name,
                                                 ClangTidyContext *Context)
    : HeaderGuardCheck(Name, Context),
      HeaderGuardPrefix(
          Options.get("HeaderGuardPrefix", DefaultHeaderGuardPrefix)) {}

void CSE3210HeaderGuardCheck::storeOptions(ClangTidyOptions::OptionMap &Opts) {
  Options.store(Opts, "HeaderGuardPrefix", HeaderGuardPrefix);
}

std::string CSE3210HeaderGuardCheck::getHeaderGuard(StringRef Filename,
                                                    StringRef OldGuard) {
  std::string Guard = tooling::getAbsolutePath(Filename);

  // When running under Windows, need to convert the path separators from
  // `\` to `/`.
  Guard = llvm::sys::path::convert_to_slash(Guard);

  // Sanitize the path. There are some rules for compatibility with the
  // historic style in include/llvm and include/clang which we want to
  // preserve.

  // We don't want _INCLUDE_ in our guards.
  size_t PosInclude = Guard.rfind("include/");
  if (PosInclude != StringRef::npos) {
    Guard = Guard.substr(PosInclude + std::strlen("include/"));
  }

  // For clang we drop the _TOOLS_.
  size_t PosToolsClang = Guard.rfind("tools/clang/");
  if (PosToolsClang != StringRef::npos) {
    Guard = Guard.substr(PosToolsClang + std::strlen("tools/"));
  }

  // Unlike LLVM svn, LLVM git monorepo is named llvm-project, so we replace
  // "/llvm-project/" with the canonical "/llvm/".
  const static StringRef LLVMProject = "/llvm-project/";
  size_t PosLLVMProject = Guard.rfind(LLVMProject);
  if (PosLLVMProject != StringRef::npos) {
    Guard = Guard.replace(PosLLVMProject, LLVMProject.size(), "/llvm/");
  }

  // The remainder is LLVM_FULL_PATH_TO_HEADER_H
  size_t PosLLVM = Guard.rfind("llvm/");
  if (PosLLVM != StringRef::npos) {
    Guard = Guard.substr(PosLLVM);
  }

  llvm::replace(Guard, '/', '_');
  llvm::replace(Guard, '.', '_');
  llvm::replace(Guard, '-', '_');

  // The prevalent style in clang is LLVM_CLANG_FOO_BAR_H
  if (StringRef(Guard).starts_with("clang")) {
    Guard = "LLVM_" + Guard;
  }

  // The prevalent style in flang is FORTRAN_FOO_BAR_H
  if (StringRef(Guard).starts_with("flang")) {
    Guard = "FORTRAN" + Guard.substr(sizeof("flang") - 1);
  }

  // Prefix 추가
  Guard = HeaderGuardPrefix + Guard;

  // 맨 마지막에 _ 추가
  Guard = Guard + "_";

  std::cout << "Suggested header guard: " << Guard << "\n";

  return StringRef(Guard).upper();
}

namespace {

class CSE3210CheckModule : public ClangTidyModule {
public:
  void addCheckFactories(ClangTidyCheckFactories &CheckFactories) override {
    CheckFactories.registerCheck<CSE3210HeaderGuardCheck>(
        "cse3210-header-guard");
  }
};

} // namespace

namespace clang::tidy {

// Register the module using this statically initialized variable.
static ClangTidyModuleRegistry::Add<::CSE3210CheckModule>
    cse3210HeaderGuardCheckInit("cse3210-module",
                                "Adds 'CSE3210 lint' checks.");

// This anchor is used to force the linker to link in the generated object file
// and thus register the module.
volatile int CSE3210CheckAnchorSource = 0;

} // namespace clang::tidy
