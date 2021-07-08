

#include<linux/init.h>
#include<linux/module.h>

#include<linux/fs.h>
#include<linux/slab.h>
#include<linux/uaccess.h>
#include <asm/uaccess.h>
#include<linux/slab.h>

#define BUFFER_SIZE 1024

MODULE_LICENSE("GPL");
MODULE_AUTHOR("LUKE WOODHEAD");
/* Define device_buffer and other global data structures you will need here */
char *device_buffer;
int file_opened = 0;
int file_closed = 0;



ssize_t pa2_char_driver_read (struct file *pfile, char __user *buffer, size_t length, loff_t *offset)
{
	if(*offset + length > BUFFER_SIZE){
		printk(KERN_ALERT "buffer overflow\n");
		return 0;
	}
	/* *buffer is the userspace buffer to where you are writing the data you want to be read from the device file*/
	/* length is the length of the userspace buffer*/
	/* offset will be set to current position of the opened file after read*/
	/* copy_to_user function: source is device_buffer and destination is the userspace buffer *buffer */

	/* copy information to user buffer*/
	copy_to_user(buffer,device_buffer+*offset,length);
	*offset = *offset+length;
	printk(KERN_ALERT "read %d bytes with a new offset of %d",(int)length,(int)*offset);
	return 0;
}



ssize_t pa2_char_driver_write (struct file *pfile, const char __user *buffer, size_t length, loff_t *offset)
{
	if(*offset+length > BUFFER_SIZE){
		printk(KERN_ALERT "buffer overflow\n");
		return length;
	}
	/* *buffer is the userspace buffer where you are writing the data you want to be written in the device file*/
	/* length is the length of the userspace buffer*/
	/* current position of the opened file*/
	/* copy_from_user function: destination is device_buffer and source is the userspace buffer *buffer */

	copy_from_user(device_buffer+*offset,buffer,length);
	*offset = *offset+length;
	printk(KERN_ALERT "wrote %d bytes with a new offset of %d",(int)length,(int)*offset);
	return length;
}


int pa2_char_driver_open (struct inode *pinode, struct file *pfile)
{
	/* print to the log file that the device is opened and also print the number of times this device has been opened until now*/
	file_opened++;
	printk(KERN_ALERT "device opened\n");
	printk(KERN_ALERT "device has been opened: %d times\n",file_opened);
	return 0;
}

int pa2_char_driver_close (struct inode *pinode, struct file *pfile)
{
	/* print to the log file that the device is closed and also print the number of times this device has been closed until now*/
	file_closed++;
	printk(KERN_ALERT "device closed mungo\n");
	printk(KERN_ALERT "device has been closed: %d times\n",file_closed);
	return 0;
}

loff_t pa2_char_driver_seek (struct file *pfile, loff_t offset, int whence)
{
	/* Update open file position according to the values of offset and whence */
	loff_t temp_offset = pfile->f_pos;
	if(whence == 0){
		if(offset <= BUFFER_SIZE){
			temp_offset = offset;
		}else{
			printk(KERN_ALERT "Invalid offset\n");
		}
	}
	if(whence == 1){
		if(temp_offset + offset <= BUFFER_SIZE){
			temp_offset = temp_offset + offset;
		}else{
			printk(KERN_ALERT "Invalid offset\n");
		}
	}
	if(whence == 2){
		if(offset <= BUFFER_SIZE){
			temp_offset = BUFFER_SIZE - offset;
		}else{
			printk(KERN_ALERT "Invalid offset\n");
		} 
	}
	else{
		printk(KERN_ALERT "invalid whence value\n");	
	}
	pfile->f_pos = temp_offset;
	printk(KERN_ALERT "new offset is %d\n",(int)temp_offset);
	return 0;
}

struct file_operations pa2_char_driver_file_operations = {

	.owner   = THIS_MODULE,
	.read  = &pa2_char_driver_read,
	.write = &pa2_char_driver_write, 
	.open  = &pa2_char_driver_open,
	.release = &pa2_char_driver_close,
	.llseek  = &pa2_char_driver_seek,
	/* add the function pointers to point to the corresponding file operations. look at the file fs.h in the linux souce code*/
};

static int pa2_char_driver_init(void)
{
	/* print to the log file that the init function is called.*/
	printk(KERN_ALERT "char_driver init called\n");
	device_buffer = kmalloc(BUFFER_SIZE,GFP_KERNEL);
	/* register the device */
	register_chrdev(245,"pa2_char_d",&pa2_char_driver_file_operations);
	return 0;
}

static void pa2_char_driver_exit(void)
{
	/* print to the log file that the exit function is called.*/
	printk(KERN_ALERT "char_driver exit called\n");
	kfree(device_buffer);
	/* unregister  the device using the register_chrdev() function. */
	unregister_chrdev(245,"pa2_char_d");

}

/* add module_init and module_exit to point to the corresponding init and exit function*/
module_init(pa2_char_driver_init);
module_exit(pa2_char_driver_exit);