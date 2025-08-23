#include <linux/module.h>
#include <linux/init.h>
#include <linux/fs.h>
#include <linux/cdev.h>
#include <linux/device.h>
#include <linux/uaccess.h>
#include <linux/mutex.h>


#include <uapi/msg_board_uapi.h>

#define DRIVER_NAME "msg_board"
#define DEVICE_NAME "msgboard"
#define CLASS_NAME  "msgboard"
#define BUFFER_SIZE 256

static dev_t dev_num;
static struct cdev mb_cdev;
static struct class *mb_class;
static struct device *mb_device;

static char board[BUFFER_SIZE];
static int open_count;
static DEFINE_MUTEX(board_lock);

/* File Ops  */
static int mb_open(struct inode *inode, struct file *file)
{
    mutex_lock(&board_lock);
    open_count++;
    mutex_unlock(&board_lock);

    pr_info(DRIVER_NAME ": Device opened, open_count = %d\n", open_count);
    return 0;
}

static int mb_release(struct inode *inode, struct file *file)
{
    pr_info(DRIVER_NAME ": Device released\n");
    return 0;
}

static ssize_t mb_read(struct file *file, char __user *buf, size_t len, loff_t *ppos)
{
    ssize_t size, to_copy;

    mutex_lock(&board_lock);
    size = strnlen(board, BUFFER_SIZE);

    if (*ppos >= size) {
        mutex_unlock(&board_lock);
        return 0;  /* EOF */
    }

    to_copy = min_t(ssize_t, len, size - *ppos);

    if (copy_to_user(buf, board + *ppos, to_copy)) {
        mutex_unlock(&board_lock);
        return -EFAULT;
    }

    *ppos += to_copy;
    mutex_unlock(&board_lock);

    return to_copy;
}

static ssize_t mb_write(struct file *file, const char __user *buf, size_t len, loff_t *ppos)
{
    size_t to_copy = min(len, (size_t)BUFFER_SIZE - 1);

    if (to_copy == 0)
        return -ENOSPC;

    if (copy_from_user(board, buf, to_copy))
        return -EFAULT;

    board[to_copy] = '\0';

    if (ppos)
        *ppos = 0;  /* Reset offset */

    return to_copy;
}

static long mb_ioctl(struct file *file, unsigned int cmd, unsigned long arg)
{
    switch (cmd) {
        case MSGB_GET_OPEN_COUNT:
            if (copy_to_user((int __user *)arg, &open_count, sizeof(open_count)))
                return -EFAULT;
            return 0;

        case MSGB_CLEAR_BOARD:
            mutex_lock(&board_lock);
            memset(board, 0, sizeof(board));
            mutex_unlock(&board_lock);
            return 0;

        case MSGB_SET_MESSAGE: {
            size_t max = BUFFER_SIZE - 1;

            if (copy_from_user(board, (void __user *)arg, max))
                return -EFAULT;

            board[max] = '\0';
            return 0;
        }

        default:
            return -ENOTTY;
    }
}

/* File Ops */
static const struct file_operations mb_fops = {
    .owner          = THIS_MODULE,
    .open           = mb_open,
    .release        = mb_release,
    .read           = mb_read,
    .write          = mb_write,
    .unlocked_ioctl = mb_ioctl,
};

/* Module Init */
static int __init mb_init(void)
{
    int ret;

    /* Allocate device numbers */
    ret = alloc_chrdev_region(&dev_num, 0, 1, DRIVER_NAME);
    if (ret < 0) {
        pr_err(DRIVER_NAME ": Failed to allocate chrdev region\n");
        return ret;
    }

    /* Create device class */
    mb_class = class_create(CLASS_NAME);
    if (IS_ERR(mb_class)) {
        pr_err(DRIVER_NAME ": Failed to create class\n");
        ret = PTR_ERR(mb_class);
        goto err_unregister_region;
    }

    /* Create device */
    mb_device = device_create(mb_class, NULL, dev_num, NULL, DEVICE_NAME);
    if (IS_ERR(mb_device)) {
        pr_err(DRIVER_NAME ": Failed to create device\n");
        ret = PTR_ERR(mb_device);
        goto err_destroy_class;
    }

    /* Initialize cdev */
    cdev_init(&mb_cdev, &mb_fops);
    mb_cdev.owner = THIS_MODULE;

    ret = cdev_add(&mb_cdev, dev_num, 1);
    if (ret < 0) {
        pr_err(DRIVER_NAME ": Failed to add cdev\n");
        goto err_destroy_device;
    }

    pr_info(DRIVER_NAME ": Module loaded successfully\n");
    return 0;

/* Error Handling */
err_destroy_device:
    device_destroy(mb_class, dev_num);
err_destroy_class:
    class_destroy(mb_class);
err_unregister_region:
    unregister_chrdev_region(dev_num, 1);

    return ret;
}

/* Module Exit */
static void __exit mb_exit(void)
{
    cdev_del(&mb_cdev);
    device_destroy(mb_class, dev_num);
    class_destroy(mb_class);
    unregister_chrdev_region(dev_num, 1);

    pr_info(DRIVER_NAME ": Module unloaded successfully\n");
}

module_init(mb_init);
module_exit(mb_exit);

MODULE_LICENSE("GPL");
MODULE_AUTHOR("Bharath Reddappa");
MODULE_DESCRIPTION("Message Board Character Device Driver");
MODULE_VERSION("1.0");
