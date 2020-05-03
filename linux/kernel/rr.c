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
	current->flags &= 0xfeffffff;
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
