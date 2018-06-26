#include<linux/module.h>
#include<linux/init.h>
#include<linux/kernel.h>
#include<linux/fs.h>
#include<linux/cdev.h>
#include<linux/uaccess.h>
#include<linux/memory.h>

#define TEST_MAJOR 230
#define DEV_NAME "TESTMODULE"
char j='0';
static int test_open(struct inode *inode, struct file *file){
    printk(KERN_EMERG "Open Test.\n");
    return 0;
}
static ssize_t test_read(struct file *file,  char __user *buf,size_t len,loff_t *ppos){
	char data[28]="ABCDEFGHIJKLMNOPQRSTUVWXYZ\n";
	char kdata[29];
	memset(kdata,0,sizeof(kdata));
	if(j>'9')
	{
		j='0';
	}

	sprintf(kdata,"%c%s",j++,data);
	copy_to_user(buf,kdata,sizeof(kdata));
	memset(kdata,0,sizeof(kdata));

	printk("%s\n",__func__);
	return sizeof(kdata); 
}

static ssize_t test_write(struct file *file, const char __user *buf,size_t len,loff_t *ppos){
	char temp[64]={0};
    if(len>64)  
    {  
        len = 64;  
    }  
  
    if(copy_from_user(temp,buf,len))  
    {  
        return -EFAULT;  
    }  
    printk("write %s\n",temp);  
    return len; 
}

static struct file_operations test_flops = {
    .owner  =   THIS_MODULE,
    .open   =   test_open,   
	.read   =   test_read,
    .write  =   test_write,
};
static int __init test_init(void){

	register_chrdev(TEST_MAJOR,DEV_NAME, &test_flops);
	printk(KERN_INFO"DEV Install\n");
	return 0;
}

static void __exit test_exit(void){

	unregister_chrdev(TEST_MAJOR, DEV_NAME);
	printk(KERN_INFO"DEV Remove\n");

}

module_init(test_init);
module_exit(test_exit);


