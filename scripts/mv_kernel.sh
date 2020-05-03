WORKSPACE=/home/${USER}/keyboardinput-recordreplay
TARGETIP=192.168.0.2
cd ${WORKSPACE}/linux

# 1. cp zImage
scp arch/arm/boot/zImage pi@${TARGETIP}:/home/pi
ssh pi@${TARGETIP} "sudo cp zImage /boot/kernel7.img"

# 2. cp modules
rm ${WORKSPACE}/modules/lib/modules/4.9.80-v7/build
rm ${WORKSPACE}/modules/lib/modules/4.9.80-v7/source
scp -r ${WORKSPACE}/modules/lib/modules/4.9.80-v7/ pi@${TARGETIP}:/home/pi
ssh pi@${TARGETIP} "sudo cp -rf 4.9.80-v7/ /lib/modules/"

# 3. cp .dtb files
scp arch/arm/boot/dts/*.dtb pi@${TARGETIP}:/home/pi
ssh pi@${TARGETIP} "sudo cp *.dtb /boot; rm *.dtb"

# 4. cp overlays/.dtb files
scp arch/arm/boot/dts/overlays/*.dtb* pi@${TARGETIP}:/home/pi
scp arch/arm/boot/dts/overlays/README pi@${TARGETIP}:/home/pi
ssh pi@${TARGETIP} "sudo cp *.dtb* /boot/overlays/; rm *.dtb*"
ssh pi@${TARGETIP} "sudo cp README /boot/overlays/; rm README"
