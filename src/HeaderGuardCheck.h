#ifndef CSE3210_HEADERGUARDCHECK_H
#define CSE3210_HEADERGUARDCHECK_H

#include "clang-tidy/ClangTidyModule.h"
#include "clang-tidy/utils/HeaderGuard.h"
#include <string>

using namespace clang;
using namespace clang::tidy;
using namespace clang::ast_matchers;

class HeaderGuardCheck : public utils::HeaderGuardCheck {
public:
  HeaderGuardCheck(StringRef Name, ClangTidyContext *Context);
  void storeOptions(ClangTidyOptions::OptionMap &Opts) override;
  bool shouldSuggestEndifComment(StringRef Filename) override { return false; }
  std::string getHeaderGuard(StringRef Filename, StringRef OldGuard) override;

private:
  const std::string HeaderGuardPrefix;
};

#endif
