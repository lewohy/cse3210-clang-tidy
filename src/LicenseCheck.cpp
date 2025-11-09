#include "LicenseCheck.h"
#include "clang/Lex/PPCallbacks.h"
#include "clang/Lex/Preprocessor.h"
#include <string>

using namespace clang;
using namespace clang::tidy;
using namespace clang::ast_matchers;

const static char DefaultRequiredText[] = "";
const static char DefaultAllowFilter[] = "^(src|include)/";
const static char DefaultDenyFilter[] =
    "(^|/)(third_party|external|build)(/|$)";

struct LicensePP : PPCallbacks {
  const clang::SourceManager &SM;
  clang::tidy::ClangTidyCheck &Owner;
  llvm::StringRef Required;
  llvm::Regex Allow;
  llvm::Regex Deny;
  llvm::StringSet<> Seen; // 중복 방지

  LicensePP(const clang::SourceManager &SM, clang::tidy::ClangTidyCheck &Owner,
            llvm::StringRef RequiredText, llvm::StringRef AllowRegex,
            llvm::StringRef DenyRegex)
      : SM(SM), Owner(Owner), Required(RequiredText), Allow(AllowRegex),
        Deny(DenyRegex) {}

  void FileChanged(clang::SourceLocation Loc, FileChangeReason Reason,
                   clang::SrcMgr::CharacteristicKind FileKind,
                   clang::FileID) override {
    if (Reason != EnterFile)
      return;

    // 시스템 헤더 제외
    if (SM.isInSystemHeader(Loc) || SM.isInSystemMacro(Loc))
      return;

    if (FileKind == clang::SrcMgr::C_System ||
        FileKind == clang::SrcMgr::C_ExternCSystem)
      return;

    // 파일 경로
    llvm::StringRef Path = SM.getFilename(Loc);
    if (Path.empty())
      return;

    // Allow/Deny 필터
    if (!Allow.match(Path) || Deny.match(Path))
      return;

    // 중복 방지
    if (!Seen.insert(Path.str()).second)
      return;

    // 검사
    clang::FileID FID = SM.getFileID(Loc);
    llvm::StringRef B = SM.getBufferData(FID);
    if (!B.starts_with(Required)) {
      auto Diag = Owner.diag(SM.getLocForStartOfFile(FID),
                             "파일 상단에 라이센스 텍스트가 없음");
      Diag << clang::FixItHint::CreateInsertion(SM.getLocForStartOfFile(FID),
                                                Required.str() + "\n");
    }
  }
};

LicenseCheck::LicenseCheck(StringRef Name, ClangTidyContext *Context)
    : ClangTidyCheck(Name, Context),
      RequiredText(Options.get("RequiredText", DefaultRequiredText)),
      AllowFilter(Options.get("AllowFilter", DefaultAllowFilter)),
      DenyFilter(Options.get("DenyFilter", DefaultDenyFilter)) {}

void LicenseCheck::storeOptions(ClangTidyOptions::OptionMap &Opts) {
  Options.store(Opts, "RequiredText", RequiredText);
  Options.store(Opts, "AllowFilter", AllowFilter);
  Options.store(Opts, "DenyFilter", DenyFilter);
}

void LicenseCheck::registerPPCallbacks(const SourceManager &SM,
                                       Preprocessor *PP,
                                       Preprocessor *ModuleExpanderPP) {
  PP->addPPCallbacks(std::make_unique<LicensePP>(SM, *this, RequiredText,
                                                 AllowFilter, DenyFilter));
}
