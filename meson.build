project(
    'cse3210-header-guard-tidy',
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
    'header-guard',
    ['src/CSE3210HeaderGuardCheck.cpp'],
    dependencies: [llvm_dep, clang_dep],
    name_prefix: '',
)
