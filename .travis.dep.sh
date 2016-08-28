# http://stackoverflow.com/questions/26082444/how-to-work-around-travis-cis-4mb-output-limit/26082445#26082445

# Abort on Error
set -e

export PING_SLEEP=30s
export WORKDIR="$( cd "$( dirname "${BASH_SOURCE[0]}" )" && pwd )"
export BUILD_OUTPUT=$WORKDIR/build.out

touch $BUILD_OUTPUT

dump_output() {
   echo Tailing the last 500 lines of output:
   tail -500 $BUILD_OUTPUT
}

error_handler() {
  echo ERROR: An error was encountered with the build.
  dump_output
  exit 1
}

# If an error occurs, run our error handler to output a tail of the build
trap 'error_handler' ERR

# Set up a repeating loop to send some output to Travis.

bash -c "while true; do echo \$(date) - building ...; sleep $PING_SLEEP; done" &
PING_LOOP_PID=$!

# Preparing the cache

cd $HOME/cache
if [ ! -d "$HOME/cache/avr8-gnu-toolchain-linux_x86" ]; then
	echo "Downloading AVR toolchain..."
	wget http://www.atmel.com/images/avr8-gnu-toolchain-3.4.5.1522-linux.any.x86.tar.gz -O avr8.tar.gz
	tar -xzf avr8.tar.gz
	rm avr8.tar.gz
fi

# ARM GCC
if [ x$ARMGCCVERSION == x"4_8-2014q1" ]; then
	INSTALLPATH="$HOME/cache/gcc-arm-none-eabi-4_8-2014q1"
	DOWNLOADURI="https://launchpad.net/gcc-arm-embedded/4.8/4.8-2014-q1-update/+download/gcc-arm-none-eabi-4_8-2014q1-20140314-linux.tar.bz2"
fi
if [ x$ARMGCCVERSION == x"4_8-2014q2" ]; then
	INSTALLPATH="$HOME/cache/gcc-arm-none-eabi-4_8-2014q2"
	DOWNLOADURI="https://launchpad.net/gcc-arm-embedded/4.8/4.8-2014-q2-update/+download/gcc-arm-none-eabi-4_8-2014q2-20140609-linux.tar.bz2"
fi
if [ x$ARMGCCVERSION == x"4_8-2014q3" ]; then
	INSTALLPATH="$HOME/cache/gcc-arm-none-eabi-4_8-2014q3"
	DOWNLOADURI="https://launchpad.net/gcc-arm-embedded/4.8/4.8-2014-q3-update/+download/gcc-arm-none-eabi-4_8-2014q3-20140805-linux.tar.bz2"
fi
if [ x$ARMGCCVERSION == x"4_9-2014q4" ]; then
	INSTALLPATH="$HOME/cache/gcc-arm-none-eabi-4_9-2014q4"
	DOWNLOADURI="https://launchpad.net/gcc-arm-embedded/4.9/4.9-2014-q4-major/+download/gcc-arm-none-eabi-4_9-2014q4-20141203-linux.tar.bz2"
fi
if [ x$ARMGCCVERSION == x"4_9-2015q1" ]; then
	INSTALLPATH="$HOME/cache/gcc-arm-none-eabi-4_9-2015q1"
	DOWNLOADURI="https://launchpad.net/gcc-arm-embedded/4.9/4.9-2015-q1-update/+download/gcc-arm-none-eabi-4_9-2015q1-20150306-linux.tar.bz2"
fi
if [ x$ARMGCCVERSION == x"4_9-2015q2" ]; then
	INSTALLPATH="$HOME/cache/gcc-arm-none-eabi-4_9-2015q2"
	DOWNLOADURI="https://launchpad.net/gcc-arm-embedded/4.9/4.9-2015-q2-update/+download/gcc-arm-none-eabi-4_9-2015q2-20150609-linux.tar.bz2"
fi
if [ x$ARMGCCVERSION == x"4_9-2015q3" ]; then
	INSTALLPATH="$HOME/cache/gcc-arm-none-eabi-4_9-2015q3"
	DOWNLOADURI="https://launchpad.net/gcc-arm-embedded/4.9/4.9-2015-q3-update/+download/gcc-arm-none-eabi-4_9-2015q3-20150921-linux.tar.bz2"
fi
if [ x$ARMGCCVERSION == x"5_2-2015q4" ]; then
	INSTALLPATH="$HOME/cache/gcc-arm-none-eabi-5_2-2015q4"
	DOWNLOADURI="https://launchpad.net/gcc-arm-embedded/5.0/5-2015-q4-major/+download/gcc-arm-none-eabi-5_2-2015q4-20151219-linux.tar.bz2"
fi
if [ x$ARMGCCVERSION == x"5_3-2016q1" ]; then
	INSTALLPATH="$HOME/cache/gcc-arm-none-eabi-5_3-2016q1"
	DOWNLOADURI="https://launchpad.net/gcc-arm-embedded/5.0/5-2016-q1-update/+download/gcc-arm-none-eabi-5_3-2016q1-20160330-linux.tar.bz2"
fi
if [ x$ARMGCCVERSION == x"5_4-2016q2" ]; then
	INSTALLPATH="$HOME/cache/gcc-arm-none-eabi-5_4-2016q2"
	DOWNLOADURI="https://launchpad.net/gcc-arm-embedded/5.0/5-2016-q2-update/+download/gcc-arm-none-eabi-5_4-2016q2-20160622-linux.tar.bz2"
fi

if [ ! -d $INSTALLPATH ]; then
	echo "Downloading Cortex-M toolchain $ARMGCCVERSION ..."
	wget $DOWNLOADURI -O cortex_m.tar.bz2
	tar -xjf cortex_m.tar.bz2
	rm cortex_m.tar.bz2
fi

# Boost
if [ ! -d "$HOME/cache/boost1.54-1.54.0" ]; then
	echo "Downloading and building libboost1.54 package..."
	wget http://box.xpcc.io/libboost1.54-dev-src.tar.gz
	tar -xzf libboost1.54-dev-src.tar.gz
	rm libboost1.54-dev-src.tar.gz
	cd boost1.54-1.54.0
	./bootstrap.sh >> $BUILD_OUTPUT 2>&1
	./b2 >> $BUILD_OUTPUT 2>&1
fi
if [ ! -d "$HOME/cache/boost" ]; then
	echo "Installing libboost1.54 package..."
	mkdir $HOME/cache/boost
	cd $HOME/cache/boost1.54-1.54.0
	./b2 --prefix=$HOME/cache/boost install >> $BUILD_OUTPUT 2>&1
fi

# The build finished without returning an error so dump a tail of the output
dump_output

# nicely terminate the ping output loop
kill $PING_LOOP_PID

cd $TRAVIS_BUILD_DIR
