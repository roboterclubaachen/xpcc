mkdir $HOME/toolchain
# Download all tools
echo "Downloading AVR toolchain..."
(cd $HOME/toolchain && wget -q http://box.xpcc.io/avr-gcc.tar.bz2) &
echo "Downloading Cortex-M toolchain..."
(cd $HOME/toolchain && wget -q https://developer.arm.com/-/media/Files/downloads/gnu-rm/6-2017q2/gcc-arm-none-eabi-6-2017-q2-update-linux.tar.bz2 -O cortex-m.tar.bz2) &
echo "Downloading libboost package..."
(cd $HOME/toolchain && wget -q http://box.xpcc.io/boost.tar.bz2) &
wait
echo "Downloads done."

# unzip all toolchains
echo "Expanding AVR toolchain..."
(cd $HOME/toolchain && tar -xjf avr-gcc.tar.bz2) &
echo "Expanding Cortex-M toolchain..."
(cd $HOME/toolchain && tar -xjf cortex-m.tar.bz2) &
echo "Expanding boost package..."
(cd $HOME/toolchain && tar -xjf boost.tar.bz2) &
wait
echo "Expanding done."
