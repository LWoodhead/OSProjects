#include <linux/kernel.h>
#include <linux/linkage.h>
#include <linux/uaccess.h>

asmlinkage long sys_cs3753_add(int number1, int number2, int *result)
{
	int temp = number1 + number2;
	printk(KERN_ALERT "before add\n");
	printk(KERN_ALERT "before result\n");
	copy_to_user(result,&temp,sizeof(int));
	printk(KERN_ALERT "after result");
	printk(KERN_ALERT "Adding %d and %d\n",number1,number2);
 	printk(KERN_ALERT "%d + %d = %d\n",number1,number2,temp);
 	return 0;
} 