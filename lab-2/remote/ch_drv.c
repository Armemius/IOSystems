#include <linux/cdev.h>
#include <linux/device.h>
#include <linux/fs.h>
#include <linux/kdev_t.h>
#include <linux/kernel.h>
#include <linux/module.h>
#include <linux/mutex.h>
#include <linux/slab.h>
#include <linux/types.h>
#include <linux/uaccess.h>
#include <linux/version.h>

#define DEVICE_NAME "var4"
#define CLASS_NAME "chardrv_var4"

static dev_t dev_num;
static struct cdev var4_cdev;
static struct class *var4_class;

static DEFINE_MUTEX(results_lock);
static u32 *results;
static size_t results_count;
static size_t results_capacity;

static size_t u32_dec_len(u32 value) {
  size_t len = 1;

  while (value >= 10) {
    value /= 10;
    len++;
  }

  return len;
}

static int append_result(u32 value) {
  size_t new_capacity;
  u32 *new_results;

  if (results_count == results_capacity) {
    new_capacity = results_capacity ? (results_capacity * 2) : 16;
    new_results =
        krealloc(results, new_capacity * sizeof(*results), GFP_KERNEL);
    if (!new_results)
      return -ENOMEM;

    results = new_results;
    results_capacity = new_capacity;
  }

  results[results_count++] = value;
  return 0;
}

static int count_digits_in_user_buf(const char __user *buf, size_t count,
                                    u32 *out) {
  char chunk[256];
  size_t copied = 0;
  u32 digits = 0;
  size_t i;

  while (copied < count) {
    size_t to_copy = min(sizeof(chunk), count - copied);

    if (copy_from_user(chunk, buf + copied, to_copy))
      return -EFAULT;

    for (i = 0; i < to_copy; i++) {
      if (chunk[i] >= '0' && chunk[i] <= '9')
        digits++;
    }

    copied += to_copy;
  }

  *out = digits;
  return 0;
}

static int var4_open(struct inode *inode, struct file *file) { return 0; }

static int var4_release(struct inode *inode, struct file *file) { return 0; }

static ssize_t var4_write(struct file *file, const char __user *buf,
                          size_t count, loff_t *ppos) {
  u32 digits;
  int ret;

  ret = count_digits_in_user_buf(buf, count, &digits);
  if (ret)
    return ret;

  mutex_lock(&results_lock);
  ret = append_result(digits);
  mutex_unlock(&results_lock);
  if (ret)
    return ret;

  return count;
}

static ssize_t var4_read(struct file *file, char __user *buf, size_t count,
                         loff_t *ppos) {
  u32 *snapshot;
  size_t snapshot_count;
  size_t i;
  size_t text_len = 0;
  char *text;
  size_t pos = 0;
  ssize_t ret;

  mutex_lock(&results_lock);
  snapshot_count = results_count;
  if (snapshot_count == 0) {
    mutex_unlock(&results_lock);
    return 0;
  }

  snapshot = kmemdup(results, snapshot_count * sizeof(*results), GFP_KERNEL);
  mutex_unlock(&results_lock);
  if (!snapshot)
    return -ENOMEM;

  for (i = 0; i < snapshot_count; i++)
    text_len += u32_dec_len(snapshot[i]) + 1;

  text = kmalloc(text_len + 1, GFP_KERNEL);
  if (!text) {
    kfree(snapshot);
    return -ENOMEM;
  }

  for (i = 0; i < snapshot_count; i++)
    pos += scnprintf(text + pos, text_len + 1 - pos, "%u\n", snapshot[i]);

  ret = simple_read_from_buffer(buf, count, ppos, text, text_len);

  kfree(text);
  kfree(snapshot);
  return ret;
}

static const struct file_operations var4_fops = {
    .owner = THIS_MODULE,
    .open = var4_open,
    .release = var4_release,
    .read = var4_read,
    .write = var4_write,
    .llseek = no_llseek,
};

static int __init var4_init(void) {
  int ret;

  ret = alloc_chrdev_region(&dev_num, 0, 1, DEVICE_NAME);
  if (ret)
    return ret;

  cdev_init(&var4_cdev, &var4_fops);

  ret = cdev_add(&var4_cdev, dev_num, 1);
  if (ret)
    goto err_unregister;

  var4_class = class_create(THIS_MODULE, CLASS_NAME);

  if (IS_ERR(var4_class)) {
    ret = PTR_ERR(var4_class);
    goto err_cdev;
  }

  if (IS_ERR(device_create(var4_class, NULL, dev_num, NULL, DEVICE_NAME))) {
    ret = -ENOMEM;
    goto err_class;
  }

  pr_info("var4: loaded, major=%d minor=%d\n", MAJOR(dev_num), MINOR(dev_num));
  return 0;

err_class:
  class_destroy(var4_class);
err_cdev:
  cdev_del(&var4_cdev);
err_unregister:
  unregister_chrdev_region(dev_num, 1);
  return ret;
}

static void __exit var4_exit(void) {
  device_destroy(var4_class, dev_num);
  class_destroy(var4_class);
  cdev_del(&var4_cdev);
  unregister_chrdev_region(dev_num, 1);

  mutex_lock(&results_lock);
  kfree(results);
  results = NULL;
  results_count = 0;
  results_capacity = 0;
  mutex_unlock(&results_lock);

  pr_info("var4: unloaded\n");
}

module_init(var4_init);
module_exit(var4_exit);

MODULE_LICENSE("GPL");
MODULE_AUTHOR("Armemius");
MODULE_DESCRIPTION("IO Systems Lab 2: Variant 4");
