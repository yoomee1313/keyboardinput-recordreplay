TARGETIP=192.168.0.2

ssh pi@${TARGETIP} "cd UserProgram; gcc -o goalprogram goalprogram.c; gcc -o rrdebug rrdebug.c"
