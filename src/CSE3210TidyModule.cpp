#include "clang-tidy/ClangTidy.h"
#include "clang-tidy/ClangTidyModule.h"
#include "clang-tidy/ClangTidyModuleRegistry.h"
#include "src/HeaderGuardCheck.h"

using namespace clang;
using namespace clang::tidy;

namespace {

class CSE3210CTidyModule : public ClangTidyModule {
public:
  void addCheckFactories(ClangTidyCheckFactories &CheckFactories) override {
    CheckFactories.registerCheck<HeaderGuardCheck>("cse3210-header-guard");
  }
};

} // namespace

namespace clang::tidy {

// Register the module using this statically initialized variable.
static ClangTidyModuleRegistry::Add<::CSE3210CTidyModule>
    CSE3210HeaderGuardCheckInit("cse3210-module",
                                "Adds 'CSE3210 lint' checks.");

// This anchor is used to force the linker to link in the generated object file
// and thus register the module.
// NOLINTNEXTLINE(misc-use-internal-linkage)
volatile int CSE3210CheckAnchorSource = 0;

} // namespace clang::tidy
