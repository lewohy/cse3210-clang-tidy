#ifndef CSE3210_LICENSECHECK_H
#define CSE3210_LICENSECHECK_H

#include "clang-tidy/ClangTidyCheck.h"
#include "clang-tidy/ClangTidyModule.h"
#include <string>

using namespace clang;
using namespace clang::tidy;
using namespace clang::ast_matchers;

class LicenseCheck : public ClangTidyCheck {
public:
  LicenseCheck(StringRef Name, ClangTidyContext *Context);
  void storeOptions(ClangTidyOptions::OptionMap &Opts) override;
  void registerPPCallbacks(const SourceManager &SM, Preprocessor *PP,
                           Preprocessor *ModuleExpanderPP) override;

private:
  const std::string RequiredText;
  const std::string AllowFilter;
  const std::string DenyFilter;
};

#endif
