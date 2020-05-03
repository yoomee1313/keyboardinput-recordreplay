# Record keyboard input from user and replay it


## 1. development platform
cross compile host pc: virtualbox of Intel I7-4790
cross compile host os: ubuntu 16.04

Target Platform: raspberry pi3 b+
Target os: Raspbian Stretch September 2017
kernel version: raspbian linux 4.9 (rpi-4.9)

Router: iptime

### A. install raspbian
install raspbian whatever you want at raspberry pi3
after installation, connect raspbian to the network

### C. install git and clone the repository
make or choose a directory
here, the directory is home(/home/<username>)
```
$ sudo apt install git
$ git clone https://github.com/yoomee1313/keyboardinput-recordreplay.git
```

### D. install dependencies and config kernel
install arm cross compiler
```
$ sudo apt install gcc-arm-linux-gnueabihf
$ sudo apt install make ncurses-dev build-essential
```
configurate kernel
```
$ cd ~/keyboardinput-recordreplay/linux
$ KERNEL=kernel7
$ make ARCH=arm CROSS_COMPILE=arm-linux-gnueabihf- bcm2709_defconfig
$ make ARCH=arm menuconfig  #save the config and exit
```

### E. compile kernel at host pc
compilation, if you want to power up, modify j4
```
$ cd ~/keyboardinput-recordreplay/linux
$ make ARCH=arm CROSS_COMPILE=arm-linux-gnueabihf- zImage modules dtbs -j16
$ make ARCH=arm CROSS_COMPILE=arm-linux-gnueabihf- INSTALL_MOD_PATH=~/keyboardinput-recordreplay/modules modules_install -j16
```

### F. copy kernel files to raspberry pi
WORKSPACE=/home/${USER}/keyboardinput-recordreplay
TARGETIP=<your raspberry pi ip>
cd ${WORKSPACE}/linux
```
# i. cp zImage
scp arch/arm/boot/zImage pi@${TARGETIP}:/home/pi
ssh pi@${TARGETIP} "sudo cp zImage /boot/kernel7.img"
# ii. cp modules
rm ${WORKSPACE}/modules/lib/modules/4.9.80-v7/build
rm ${WORKSPACE}/modules/lib/modules/4.9.80-v7/source
scp -r ${WORKSPACE}/modules/lib/modules/4.9.80-v7/ pi@${TARGETIP}:/home/pi
ssh pi@${TARGETIP} "sudo cp -rf 4.9.80-v7/ /lib/modules/"
# iii. cp .dtb files
scp arch/arm/boot/dts/*.dtb pi@${TARGETIP}:/home/pi
ssh pi@${TARGETIP} "sudo cp *.dtb /boot; rm *.dtb"
# iv. cp overlays/.dtb files
scp arch/arm/boot/dts/overlays/*.dtb* pi@${TARGETIP}:/home/pi
scp arch/arm/boot/dts/overlays/README pi@${TARGETIP}:/home/pi
ssh pi@${TARGETIP} "sudo cp *.dtb* /boot/overlays/; rm *.dtb*"
ssh pi@${TARGETIP} "sudo cp README /boot/overlays/; rm README"
```

## 2. Modification (already modified, this if for explanation)

### A. add rr_value and rr_count member to task_struct
linux/include/linux/sched.h
```
struct task_struct {
    ...
    unsigned int flags;
    unsigned int ptrace;

    void *rr_value;
    int *rr_count;
    ...
}
```
### B. add system call
location: linux/arch/arm/include/uapi/asm/unistd.h
```
#define __NR_pcbflagtorecord            (__NR_SYSCALL_BASE+397)
#define __NR_pcbflagtoreplay            (__NR_SYSCALL_BASE+398)
#define __NR_endrrdebug                 (__NR_SYSCALL_BASE+399)
```
location: linux/arch/arm/kernel/calls.S
```
CALL(sys_pcbflagtorecord)
CALL(sys_pcbflagtoreplay)
CALL(sys_endrrdebug)
```
implement system call handler functions
location: linux/kernel/rr.c (create new file)
```
#include <linux/kernel.h>
#include <linux/slab.h>

asmlinkage long sys_pcbflagtorecord(void)
{
        current->flags |= 0x01000000;
        current->rr_value = kmalloc(4096, GFP_KERNEL);
        current->rr_count = kmalloc(4096, GFP_KERNEL);
        printk("[RR] system call 397 is called\n");
        return 1;
}
asmlinkage long sys_pcbflagtoreplay(void)
{
        current->flags |= 0x02000000;
        current->falgs &= 0xfeffffff;
        printk("[RR] system call 398 is called\n");
        return 1;
}
asmlinkage long sys_endrrdebug(void)
{
        void *tmp1;
        int *tmp2;
        tmp1 = current->rr_value;
        kfree(tmp1);
        tmp2 = current->rr_count;
        kfree(tmp2);
        current->rr_value = NULL;
        current->rr_count = NULL;
        printk("[RR] system call 399 is called\n");
        return 1;
}

```
add rr.o to makefile
location: linux/kernel/makefile
```
obj-y     = fork.o exec_domain.o panic.o \
            cpu.o exit.o softirq.o resource.o \
            sysctl.o sysctl_binary.o capability.o ptrace.o user.o \
            signal.o sys.o kmod.o workqueue.o pid.o task_work.o \
            extable.o params.o \
            kthread.o sys_ni.o nsproxy.o \
            notifier.o ksysfs.o cred.o reboot.o \
            async.o range.o smpboot.o ucount.o rr.o
```
### C. record replay code
location: linux/drivers/tty/n_tty.call
```
static int tty_copy_to_user(struct tty_struct *tty, void __user *to,
                            size_t tail, size_t n)
{
        struct n_tty_data *ldata = tty->disc_data;
        size_t size = N_TTY_BUF_SIZE - tail;
        const void *from = read_buf_addr(ldata, tail);
        int uncopied;

        // int i;
        //RRDEBUG-record
        if(((current->flags) & 0x01000000) == 0x01000000){
                // printk("<<<recording>>>\n");
                // printk("n: [%d],\t size: [%d],\t tail: [%d]\n", n, size, tail);
                memcpy(current->rr_value, (void __force *)from, n);
                // printk("current->rr_debug values: ");
                // for(i=0; i<n-1; i++)
                //      printk("[%d th]: %c ",i,*((char *)(current->rr_value+i)));
                current->rr_value+=n;
                // printk("[current->rr_count]:%p, [n]:%d",current->rr_count,n);
                *(current->rr_count)=n;
                current->rr_count+=1;
        }
        //RRDEBUG-replay
        if(((current->flags) & 0x02000000) == 0x02000000){
                // printk("<<<replaying>>>\n");
                // printk("BEFORE CHANGED n: [%d]\n", n);
                // printk("n: %d\n", n);
                memcpy((void __force*)from, current->rr_value, n);
                // printk("from values: ");
                // for(i=0; i<n-1; i++)
                //      printk("[%d th]: %c ",i,*((char *)(from+i)));
                current->rr_value+=n;
        }

        if (n > size) {
                tty_audit_add_data(tty, from, size);
                uncopied = copy_to_user(to, from, size);
                if (uncopied)
                        return uncopied;
                to += size;
                n -= size;
                from = ldata->read_buf;
        }

        tty_audit_add_data(tty, from, n);
        return copy_to_user(to, from, n);
}
```
## 3. scripts
sample scripts are provided.
### A. install_deps.sh
install dependencies for cross compiling
### B. conf_kernel.sh
configuring rpi-4.9.80-v7 kernel
### C. kernel_compile.sh
it compiles kernel and make modules
### D. mv_kernel.sh
it moves kernel7 image and kernel modules to the raspberry pi
