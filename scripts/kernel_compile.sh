WORKSPACE=/home/${USER}/keyboardinput-recordreplay
cd ${WORKSPACE}/linux
make ARCH=arm CROSS_COMPILE=arm-linux-gnueabihf- zImage modules dtbs -j16
make ARCH=arm CROSS_COMPILE=arm-linux-gnueabihf- INSTALL_MOD_PATH=${WORKSPACE}/modules modules_install -j16
