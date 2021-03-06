lmake_compatibility_version(1)

COMPILER = "/bin/clang"
LINKER = "/bin/ld.lld"
CXX_FLAGS = "-Wall -nostdlib -ffreestanding -Isrc -Isrc/klib -Isrc/kernel -mgeneral-regs-only"
ASM_FLAGS = "-Isrc"

ARCH = "aarch64"
CLANG_TARGET = ""
LLD_TARGET = ""

if ARCH == "aarch64" then
    CLANG_TARGET = "--target=aarch64 "
    LLD_TARGET = "-m aarch64elf "
end

OBJECT_FILES = ""

function build()
    lmake_set_compiler(COMPILER)
    compile_hal()
    compile_klib()
    compile_kernel()

    lmake_set_linker(LINKER)
    lmake_set_linker_flags(LLD_TARGET .. "-T linker/linker.ld")
    lmake_set_linker_out("build/kernel8.elf")
    lmake_link(OBJECT_FILES)
end

function compile_kernel()
    lmake_set_compiler_flags(CLANG_TARGET ..  CXX_FLAGS)
    lmake_set_compiler_out("build/%.o")
    lmake_compile("src/kernel/kernel.cc src/kernel/mini_uart.cc")
    OBJECT_FILES = OBJECT_FILES .. "build/kernel.cc.o build/mini_uart.cc.o "

    lmake_set_compiler_flags(CLANG_TARGET .. ASM_FLAGS)
    lmake_compile("src/kernel/boot.S src/kernel/pre_kernel.S src/kernel/utils.S")
    OBJECT_FILES = OBJECT_FILES .. "build/boot.S.o build/pre_kernel.S.o build/utils.S.o "

end

function compile_klib()
    lmake_set_compiler_flags(CLANG_TARGET .. CXX_FLAGS)
    lmake_set_compiler_out("build/%.o")
    lmake_compile("src/klib/cstring.cc src/klib/klib.cc src/klib/printf.cc")
    OBJECT_FILES = OBJECT_FILES .. "build/cstring.cc.o build/klib.cc.o build/printf.cc.o "
end

function compile_hal()
    lmake_set_compiler_flags(CLANG_TARGET .. CXX_FLAGS)
    lmake_set_compiler_out("build/%.o")
    lmake_compile("src/hal/cpu.cc src/hal/excpt.cc src/hal/interrupts.cc")
    OBJECT_FILES = OBJECT_FILES .. "build/cpu.cc.o build/excpt.cc.o build/interrupts.cc.o "

    if ARCH == "aarch64" then
        lmake_compile("src/hal/aarch64/aarch64_excpt.c")
        lmake_set_compiler_flags(CLANG_TARGET .. ASM_FLAGS)
        lmake_compile("src/hal/aarch64/aarch64_cpu.S src/hal/aarch64/aarch64_excpt.S src/hal/aarch64/aarch64_sysregs.S")
        OBJECT_FILES = OBJECT_FILES .. "build/aarch64_cpu.S.o build/aarch64_excpt.S.o build/aarch64_sysregs.S.o build/aarch64_excpt.c.o "
    end
end

function clean()
    lmake_exec("rm build/aarch64_cpu.S.o build/aarch64_excpt.c.o build/boot.S.o build/cstring.cc.o build/interrupts.cc.o build/kernel8.elf build/mini_uart.cc.o build/printf.cc.o build/aarch64_excpt.S.o build/aarch64_sysregs.S.o build/cpu.cc.o build/excpt.cc.o build/kernel.cc.o build/klib.cc.o build/pre_kernel.S.o build/utils.S.o")
end