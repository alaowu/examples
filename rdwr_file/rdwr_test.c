#include <linux/types.h>
#include <linux/kernel.h>
#include <linux/fs.h>
#include <linux/init.h>
#include <linux/module.h>
#include <linux/device.h>
#include <linux/uaccess.h>
#include <linux/errno.h>
#include <linux/string.h>
#include <linux/sched.h>
#include <linux/slab.h>
#include <linux/vmalloc.h>
#include <asm/fcntl.h>
#include <asm/processor.h>
#include <asm/uaccess.h>

#define INPUT_FILE      "./file/input.bin"
#define OUTPUT_FILE     "./file/output.bin"
#define TEST_FILE       "./file/test.bin"

static int map_memory_to_user(const char *fn, char *buf, int size)
{
    struct file *fp;
    mm_segment_t fs;
    loff_t pos;
    int write_len;

    fp = filp_open(fn, O_RDWR | O_CREAT | O_SYNC | O_APPEND, 0644);
    if (!fp)
    {
        printk("flip_open %s failed\n", fn);
        return -1;
    }

    fs = get_fs();
    set_fs(get_ds());

    pos = fp->f_pos;
    write_len = vfs_write(fp, buf, size, &pos);
    fp->f_pos = pos;
    printk("fn: %s, fp: %px, data: %x %x %x %x, size: %d, pos: %llu\n", fn, fp, buf[0], buf[0], buf[0], buf[0], write_len, fp->f_pos);

    vfs_fsync(fp, 0);
    filp_close(fp, NULL);

    set_fs(fs);

    return write_len;
}

static int __init read_write_init(void)
{
    struct file *rdfp, *wrfp;
    mm_segment_t fs;
    char tmpbuf[256], strings[] = "this is write and read test for kernel";
    loff_t pos = 0;
    int read_len, write_len;
    char *ptr;

    printk("read_write_init!\n");

    /* vfs_read test code */
    rdfp = filp_open(INPUT_FILE, O_RDWR | O_CREAT | O_SYNC, 0644);
    if (!rdfp)
    {
        printk("flip_open %s failed\n", INPUT_FILE);
        return -1;
    }

    fs = get_fs();
    set_fs(get_ds());

    memset(tmpbuf, 0x0, sizeof(tmpbuf));
    read_len = vfs_read(rdfp, tmpbuf, sizeof(tmpbuf), &pos);
    rdfp->f_pos = pos;
    printk("vfs_read %s done: %s, size: %d\n", INPUT_FILE, tmpbuf, read_len);

    filp_close(rdfp, NULL);

    set_fs(fs);

    /* vfs_write test code */
    wrfp = filp_open(OUTPUT_FILE, O_RDWR | O_CREAT | O_SYNC | O_NONBLOCK, 0644);
    if (!wrfp)
    {
        printk("flip_open %s failed\n", OUTPUT_FILE);
        return -1;
    }

    fs = get_fs();
    set_fs(get_ds());

    pos = wrfp->f_pos;
    write_len = vfs_write(wrfp, strings, strlen(strings), &pos);
    wrfp->f_pos = pos;
    printk("vfs_write %s done: %s, size: %d\n", OUTPUT_FILE, strings, write_len);

    filp_close(wrfp, NULL);

    set_fs(fs);

    /* vfs_write open close test code */
    ptr = (char *)kmalloc(0x2000, GFP_KERNEL);
    if (!ptr)
    {
        printk("kzmalloc failed\n");
        return -1;
    }
    memset(ptr, 'A', 0x2000);
    map_memory_to_user(TEST_FILE, ptr, 0x2000);
    memset(ptr, 'B', 0x2000);
    map_memory_to_user(TEST_FILE, ptr, 0x2000);
    memset(ptr, 'C', 0x2000);
    map_memory_to_user(TEST_FILE, ptr, 0x2000);
    memset(ptr, 'D', 0x2000);
    map_memory_to_user(TEST_FILE, ptr, 0x2000);
    kfree(ptr);

    return 0;
}

static void __exit read_write_exit(void)
{
    printk("read_write_exit\n");
}

module_init(read_write_init);
module_exit(read_write_exit);

MODULE_LICENSE("GPL");
MODULE_AUTHOR("jeffrey.wu");
MODULE_DESCRIPTION("Kernel Write And Read Test");


