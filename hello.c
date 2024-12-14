/*
 * Copyright (c) 2017, GlobalLogic Ukraine LLC
 * All rights reserved.
 *
 * Redistribution and use in source and binary forms, with or without
 * modification, are permitted provided that the following conditions are met:
 * 1. Redistributions of source code must retain the above copyright
 *    notice, this list of conditions and the following disclaimer.
 * 2. Redistributions in binary form must reproduce the above copyright
 *    notice, this list of conditions and the following disclaimer in the
 *    documentation and/or other materials provided with the distribution.
 * 3. All advertising materials mentioning features or use of this software
 *    must display the following acknowledgement:
 *    This product includes software developed by the GlobalLogic.
 * 4. Neither the name of the GlobalLogic nor the
 *    names of its contributors may be used to endorse or promote products
 *    derived from this software without specific prior written permission.
 *
 * THIS SOFTWARE IS PROVIDED BY GLOBALLOGIC UKRAINE LLC ``AS IS`` AND ANY
 * EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE IMPLIED
 * WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE
 * DISCLAIMED. IN NO EVENT SHALL GLOBALLOGIC UKRAINE LLC BE LIABLE FOR ANY
 * DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES
 * (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES;
 * LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND
 * ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT
 * (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF THIS
 * SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
 */
#include <linux/init.h>
#include <linux/module.h>
#include <linux/printk.h>
#include <linux/types.h>
#include <linux/list.h>
#include <linux/ktime.h>
#include <linux/slab.h>

MODULE_AUTHOR("Bohdan Krokhun <krokhun.bohdan@lll.kpi.ua>");
MODULE_DESCRIPTION("Hello, world in Linux Kernel Training");
MODULE_LICENSE("Dual BSD/GPL");

static unsigned int num = 1; // Default value
module_param(num, uint, S_IRUGO);
MODULE_PARM_DESC(num, "Number of times \"Hello, world!\" will be printed");

struct event_list {
	struct list_head list;
	ktime_t event_ktime;
};

int i;

static LIST_HEAD(event_list_head);

static int __init hello_init(void) {
	struct event_list *event;

	if (num == 0 || (num >= 5 && num <= 10)) {
		printk(KERN_WARNING "Parameter is 0 or is in 5 to 10 range.\n");
	}
	
	if (num > 10) {
		printk(KERN_ERR "Parameter should be less than 10.\n");
		return -EINVAL;
	}
	
	for (i = 0; i < num; i++) {
		event = kmalloc(sizeof(*event), GFP_KERNEL);
		if (!event) {
			printk(KERN_ERR "Failed to allocate memory for the event.\n");
			return -ENOMEM;
		}

		event->event_ktime = ktime_get();
		list_add_tail(&event->list, &event_list_head);

		printk(KERN_INFO "Hello, world!\n");
	}
	
	return 0;
}

static void __exit hello_exit(void) {
	struct event_list *event, *tmp;

	list_for_each_entry_safe(event, tmp, &event_list_head, list) {
		printk(KERN_INFO "Event time: %lld\n", ktime_to_ns(event->event_ktime));
		list_del(&event->list);
		kfree(event);
	}

	printk(KERN_INFO "Goodbye, world!\n");
}

module_init(hello_init);
module_exit(hello_exit);
