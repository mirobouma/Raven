
include (CheckFunctionExists)
include (CheckIncludeFiles)
include (CheckSymbolExists)

add_library(libcoro STATIC coro.c coro.h)
target_include_directories(libcoro INTERFACE ${CMAKE_CURRENT_SOURCE_DIR})

check_include_files (ucontext.h HAVE_UCONTEXT_H)
check_include_files (setjmp.h HAVE_SETJMP_H)
check_symbol_exists (sigaltstack "signal.h" HAVE_SIGALTSTACK)

configure_file(${CMAKE_CURRENT_LIST_DIR}/config.h.in ${CMAKE_CURRENT_LIST_DIR}/config.h)
#set_target_properties(coro PROPERTIES COMPILE_DEFINITIONS "CORO_ASM=1")

