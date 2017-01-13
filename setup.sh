GCC_NAME=gcc-6.2.0
GCC_URL=http://www.netgull.com/gcc/releases/gcc-6.2.0/$GCC_NAME.tar.bz2
GMP_NAME=gmp-6.1.1
GMP_URL=ftp://ftp.gnu.org/gnu/gmp/gmp-6.1.1.tar.bz2
MPFR_NAME=mpfr-3.1.5
MPFR_URL=ftp://ftp.gnu.org/gnu/mpfr/mpfr-3.1.5.tar.bz2
MPC_NAME=mpc-1.0.3
MPC_URL=ftp://ftp.gnu.org/gnu/mpc/mpc-1.0.3.tar.gz
BINUTILS_NAME=binutils-2.27
BINUTILS_URL=http://ftp.gnu.org/gnu/binutils/$BINUTILS_NAME.tar.bz2
GDB_NAME=gdb-7.12
GDB_URL=http://ftp.gnu.org/gnu/gdb/gdb-7.12.tar.xz
TOOLS=$(pwd)/tools

if [ ! -d "$TOOLS" ]; then
    mkdir tools
fi
pushd $TOOLS/src
if [ ! -d "$GCC_NAME" ]; then
    if [ ! -f "$GCC_NAME.tar.bz2" ]; then
        wget $GCC_URL -P $TOOLS/src
    fi
    tar xvf $GCC_NAME.tar.bz2
fi
if [ ! -d "$GCC_NAME/gmp" ]; then
    if [ ! -f "$GMP_NAME.tar.bz2" ]; then
        wget $GMP_URL -P $TOOLS/src
    fi
    tar xvf $GMP_NAME.tar.bz2
    mv $GMP_NAME $GCC_NAME/gmp
fi
if [ ! -d "$GCC_NAME/mpfr" ]; then
    if [ ! -f "$MPFR_NAME.tar.bz2" ]; then
        wget $MPFR_URL -P $TOOLS/src
    fi
    tar xvf $MPFR_NAME.tar.bz2
    mv $MPFR_NAME $GCC_NAME/mpfr
fi
if [ ! -d "$GCC_NAME/mpc" ]; then
    if [ ! -f "$MPC_NAME.tar.gz" ]; then
        wget $MPC_URL -P $TOOLS/src
    fi
    tar xvf $MPC_NAME.tar.gz
    mv $MPC_NAME $GCC_NAME/mpc
fi
if [ ! -d "$BINUTILS_NAME" ]; then
    if [ ! -f "$BINUTILS_NAME.tar.bz2" ]; then
        wget $BINUTILS_URL -P $TOOLS/src
    fi
    tar xvf $BINUTILS_NAME.tar.bz2
fi
if [ ! -d "$GDB_NAME" ]; then
    if [ ! -f "$GDB_NAME.tar.xz" ]; then
        wget $GDB_URL -P $TOOLS/src
    fi
    tar xvf $GDB_NAME.tar.xz
fi

export PREFIX=$TOOLS
export TARGET=i686-elf
export PATH="$PREFIX/bin:$PATH"

#build binutils
pushd $TOOLS/src
if [ ! -d "build-binutils" ]; then
    mkdir build-binutils
    pushd build-binutils
    ../$BINUTILS_NAME/configure --target=$TARGET --prefix="$PREFIX" --with-sysroot --disable-nls --disable-werror
    make
    make install
    popd
fi
popd

#build GCC
# The $PREFIX/bin dir _must_ be in the PATH. We did that above.
which -- $TARGET-as || echo $TARGET-as is not in the PATH

if [ ! -d "build-gcc" ]; then
    mkdir build-gcc
    pushd build-gcc
    ../$GCC_NAME/configure --target=$TARGET --prefix="$PREFIX" --disable-nls --enable-languages=c,c++ --without-headers
    make all-gcc
    make all-target-libgcc
    make install-gcc
    make install-target-libgcc
    popd
fi

if [ ! -d "build-gdb" ]; then
    mkdir build-gdb
    pushd build-gdb
    ../$GDB_NAME/configure --target=$TARGET --prefix="$PREFIX" --disable-nls --disable-werror --enable-tui
    make
    make install
    popd
fi



popd
# # #Using the new Compiler 
# # $HOME/opt/cross/bin/$TARGET-gcc --version
# # export PATH="$HOME/opt/cross/bin:$PATH"