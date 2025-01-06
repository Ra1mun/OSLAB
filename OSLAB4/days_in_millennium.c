#include <linux/kernel.h>
#include <linux/module.h>
#include <linux/init.h>
#include <linux/printk.h>
#include <linux/proc_fs.h>
#include <linux/uaccess.h>
#include <linux/version.h>
#include <linux/time.h>

MODULE_DESCRIPTION("Tomsk State University Kernel Module");
MODULE_AUTHOR("jeffers1n");

#define PROCFS_NAME "days_in_millennium"

static struct proc_dir_entry *proc_file = NULL;
static struct timespec64 start_time;

// Функция для вычисления количества дней, прошедших с начала тысячелетия (2001-01-01)
static long days_since_2001(void) {
    struct timespec64 current_time;
    long long diff_sec;
    long days;

    ktime_get_real_ts64(&current_time);

    diff_sec = current_time.tv_sec - start_time.tv_sec;
    days = diff_sec / (24 * 60 * 60); // Преобразуем секунды в дни

    return days;
}

static ssize_t proc_read(struct file *file, char __user *buffer, size_t len, loff_t *offset)
{
    char output[32];
    size_t output_len;
    long days;

    if (*offset > 0)
        return 0;

    days = days_since_2001();

    output_len = snprintf(output, sizeof(output), "%ld\n", days);
    if (copy_to_user(buffer, output, output_len))
        return -EFAULT;

    *offset += output_len;
    return output_len;
}

#if LINUX_VERSION_CODE >= KERNEL_VERSION(5, 6, 0)
static const struct proc_ops proc_file_ops = {
    .proc_read = proc_read,
};
#else
static const struct file_operations proc_file_ops = {
    .read = proc_read,
};
#endif

static int __init tsu_init(void)
{
    struct tm start_tm;
    time64_t start_time_sec;

    // Устанавливаем дату начала тысячелетия (2001-01-01 00:00:00)
    start_tm.tm_year = 2001 - 1900;
    start_tm.tm_mon = 0;
    start_tm.tm_mday = 1;
    start_tm.tm_hour = 0;
    start_tm.tm_min = 0;
    start_tm.tm_sec = 0;

    start_time_sec = mktime64(start_tm.tm_year + 1900, start_tm.tm_mon + 1, start_tm.tm_mday,
                              start_tm.tm_hour, start_tm.tm_min, start_tm.tm_sec);

    start_time.tv_sec = start_time_sec;
    start_time.tv_nsec = 0;

    proc_file = proc_create(PROCFS_NAME, 0644, NULL, &proc_file_ops);
    if (!proc_file) {
        pr_err("Failed to create /proc/%s\n", PROCFS_NAME);
        return -ENOMEM;
    }

    pr_info("/proc/%s created\n", PROCFS_NAME);
    return 0;
}

static void __exit tsu_exit(void)
{
    proc_remove(proc_file);
    pr_info("/proc/%s removed\n", PROCFS_NAME);
}

module_init(tsu_init);
module_exit(tsu_exit);

MODULE_LICENSE("GPL");