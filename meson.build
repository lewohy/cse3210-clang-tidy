project(
    'cse3210-clang-tidy',
    ['c', 'cpp'],
    version: '0.1.0',
    default_options: ['cpp_std=c++17', 'buildtype=release'],
)

cpp = meson.get_compiler('cpp')

llvm_dep = dependency('LLVM', method: 'cmake', required: true)
clang_dep = cpp.find_library(
    'clang',
    dirs: ['/usr/lib/llvm-21/lib'],
    required: true,
)

shared_module(
    'cse3210-clang-tidy',
    [
        'src/CSE3210TidyModule.cpp',
        'src/HeaderGuardCheck.cpp',
        'src/LicenseCheck.cpp',
    ],
    dependencies: [llvm_dep, clang_dep],
    name_prefix: '',
)
