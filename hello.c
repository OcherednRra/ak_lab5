#include <linux/init.h>
#include <linux/module.h>
#include <linux/printk.h>
#include <linux/moduleparam.h>
#include <linux/ktime.h>
#include <linux/slab.h>
#include <linux/list.h>
#include <linux/types.h>

MODULE_AUTHOR("Poliakov Valentyn");
MODULE_DESCRIPTION("Hello, world in Linux Kernel Training lab5");
MODULE_LICENSE("Dual BSD/GPL");

struct list_item {
	struct list_head next;
	ktime_t start_time;
	ktime_t finish_time;
};

static LIST_HEAD(head);

static uint counter = 1;
module_param(counter, uint, 0444);
MODULE_PARM_DESC(counter, "mycounter");

static int __init hello_init(void)
{
	uint i = 0;
	struct list_item *tail;

	pr_info("Count: %d\n", counter);

	if (!counter)
		pr_warn("The parameter = 0");
	if (counter >= 5 && counter <= 10)
		pr_warn("The parameter = %d, enter number less than 5", counter);
	BUG_ON(counter > 10);

	for (i = 0; i < counter; i++) {
		tail = kmalloc(sizeof(struct list_item), GFP_KERNEL);

		if (i == 5)
			tail = 0;

		if (ZERO_OR_NULL_PTR(tail))
			goto exception;

		tail->start_time = ktime_get();
		pr_info("Hello, World!\n");
		tail->finish_time = ktime_get();

		list_add_tail(&(tail->next), &head);
	}

	return 0;

exception:
	{
		struct list_item *md, *tmp;

		pr_err("Memory exeption...\n");

		list_for_each_entry_safe(md, tmp, &head, next) {
			list_del(&md->next);
			kfree(md);
		}

		BUG();
		return -ENOMEM;
	}
}


static void __exit hello_exit(void)
{
	struct list_item *md, *tmp;

	list_for_each_entry_safe(md, tmp, &head, next) {
		pr_info("Time: %lld",
		md->finish_time - md->start_time);

		list_del(&md->next);
		kfree(md);
	}

	BUG_ON(!list_empty(&head));
}

module_init(hello_init);
module_exit(hello_exit);