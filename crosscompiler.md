# GCC Cross Compiler Installation for Linux
###### Source: [OSDev GCC Cross Compiler](https://wiki.osdev.org/GCC_Cross-Compiler)

Installation of the Cross Compiler is an essential step, even if a compiler is already insalled on the system.
<br><b>Note:</b> The making process might take a while (minutes).
<br><b>Note:</b> For Prebuilt Toolchains (including `x86_64-elf` target) see: [OSDev GCC Cross Compiler - Prebuilt Toolchains](https://wiki.osdev.org/GCC_Cross-Compiler#Prebuilt_Toolchains)
<br><b>Note:</b> For how to use see: [OSDev GCC Cross Compiler - Using the new Compiler](https://wiki.osdev.org/GCC_Cross-Compiler#Using_the_new_Compiler)

```bash
# Preparation
mkdir $HOME/opt
mkdir $HOME/opt/cross
mkdir $HOME/src

export PREFIX="$HOME/opt/cross"
export TARGET=i686-elf
export PATH="$PREFIX/bin:$PATH"
```

---

## Binutils

```bash
# Download
cd $HOME/src
curl -O https://ftp.gnu.org/gnu/binutils/binutils-2.42.tar.gz
tar xf binutils-2.42.tar.gz
```
```bash
# Make
mkdir build-binutils
cd build-binutils
../binutils-2.42/configure --target=$TARGET --prefix="$PREFIX" --with-sysroot --disable-nls --disable-werror
make
make install
```

---

## GCC

```bash
# The $PREFIX/bin dir _must_ be in the PATH. We did that above.
which -- $TARGET-as || echo $TARGET-as is not in the PATH
```
```bash
# Download
cd $HOME/src
curl -O https://ftp.gnu.org/gnu/gcc/gcc-14.1.0/gcc-14.1.0.tar.gz
tar xf gcc-14.1.0.tar.gz
```
```bash
# Make
mkdir build-gcc
cd build-gcc
# For 32bit:
../gcc-14.1.0/configure --target=$TARGET --prefix="$PREFIX" --disable-nls --enable-languages=c,c++ --without-headers
make all-gcc
make all-target-libgcc
make install-gcc
make install-target-libgcc
```


