WORKSPACE=/home/${USER}/keyboardinput-recordreplay
cd ${WORKSPACE}/linux
KERNEL=kernel7
make ARCH=arm CROSS_COMPILE=arm-linux-gnueabihf- bcm2709_defconfig
make ARCH=arm menuconfig
