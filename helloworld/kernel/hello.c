#include <linux/types.h>
#include <linux/kernel.h>
#include <linux/fs.h>
#include <linux/init.h>
#include <linux/module.h>
#include <linux/device.h>
#include <linux/uaccess.h>

#include "hello.h"

#define CHAR_NAME   "demo"
#define CLASS_NAME  "demo"
#define DEV_NAME    "demo"

static int process_count; /* record the count of user process call this module */
static int major = -1;
static struct class *pclass = NULL;
static struct device *pcdev = NULL;

static int demo_test_open(struct inode *node, struct file *filp)
{
    process_count ++;
    printk("connect %s count %d\n", DEV_NAME, process_count);
    return 0;
}

static int demo_test_release(struct inode *node, struct file *filp)
{
    process_count --;
    printk("disconnect %s\n", DEV_NAME);
    return 0;
}

static ssize_t demo_test_write(struct file *filp, const char __user *buf, size_t cnt, loff_t *offt)
{
    printk("call demo_test_write\n");
    return 0;
}

static ssize_t demo_test_read(struct file *filp, char __user *buf, size_t cnt, loff_t *offt)
{
    printk("call demo_test_read\n");
    return 0;
}

static long demo_test_ioctl(struct file *filp, unsigned int cmd, unsigned long args)
{
    int ret = 0;
    char demo_info[] = "ioctl test for demo!";
    char tmp_buf[128];

    /* 检测命令的有效性 */
    if (_IOC_TYPE(cmd) != DEMO_TYPE) {
        return -EINVAL;
    }

    if (_IOC_NR(cmd) >= E_DEMO_CMD_MAX) {
        return -EINVAL;
    }

    /* 根据命令类型，检测参数空间是否可以访问 */
    if (_IOC_DIR(cmd) & _IOC_READ) {
        ret = !access_ok(VERIFY_WRITE, (void *)args, _IOC_SIZE(cmd));
    }
    else if (_IOC_DIR(cmd) & _IOC_WRITE) {
        ret = !access_ok(VERIFY_READ, (void *)args, _IOC_SIZE(cmd));
    }

    if (ret > 0) {
        return -EFAULT;
    }

    /* 根据命令，执行相应的操作 */
    switch (cmd)
    {
        case IOCTL_DEMO_CMD_OPEN: {
            printk("demo_test_ioctl: open\n");
        }
        break;

        case IOCTL_DEMO_CMD_CLOSE: {
            printk("demo_test_ioctl: close\n");
        }
        break;

        case IOCTL_DEMO_CMD_GET: {
            /* __copy_to_user(to, from, n) */
            ret = __copy_to_user((char *)args, demo_info, sizeof(demo_info));
            printk("demo_test_ioctl: get\n");
        }
        break;

        case IOCTL_DEMO_CMD_PUT: {
            memset(tmp_buf, 0x0, sizeof(tmp_buf));
            /* __copy_from_user(to, from, n) */
            ret = __copy_from_user(tmp_buf, (char *)args, sizeof(tmp_buf));
            printk("demo_test_ioctl: put [%s]\n", tmp_buf);
        }
        break;

        default : break;
    }

    return ret;
}

static struct file_operations demo_fops =
{
    .owner = THIS_MODULE,
    .open = demo_test_open,
    .release = demo_test_release,
    .write = demo_test_write,
    .read = demo_test_read,
    .unlocked_ioctl = demo_test_ioctl,
};

static int __init hello_init(void)
{
    major = register_chrdev(0, CHAR_NAME, &demo_fops);
    if (major < 0)
    {
        printk("register_chrdev %s failed\n", CHAR_NAME);
        return -1;
    }

    pclass = class_create(THIS_MODULE, CLASS_NAME);
    if (!pclass)
    {
        printk("class_create %s failed\n", CLASS_NAME);
        return -1;
    }

    pcdev = device_create(pclass, NULL, MKDEV(major, 0), NULL, DEV_NAME);
    if (!pcdev)
    {
        printk("device_create %s failed\n", DEV_NAME);
        return -1;
    }

    process_count = 0;

    printk("major %d, hello_init success!\n", major);

    return 0;
}

static void __exit hello_exit(void)
{
    device_unregister(pcdev);
    class_destroy(pclass);
    unregister_chrdev(major, CHAR_NAME);

    major = -1;
    pclass = NULL;
    pcdev = NULL;

    printk("hello_exit done\n");
}

module_init(hello_init);
module_exit(hello_exit);
MODULE_LICENSE("GPL");
MODULE_AUTHOR("jeffrey.wu");
MODULE_DESCRIPTION("This Is A Demo");
