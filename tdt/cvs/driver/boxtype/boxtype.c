/*
 * boxtype.c
 * 
 *  captaintrip 12.07.2008
 *
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation; either version 2 of the License, or 
 * (at your option) any later version.
 * 
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 * 
 * You should have received a copy of the GNU General Public License
 * along with this program; if not, write to the Free Software
 * Foundation, Inc., 59 Temple Place, Suite 330, Boston, MA 02111-1307 USA
 *
 */

/*
 *  Description:
 *
 *  in /proc/boxtype
 *
 *  14w returns 1 or 3
 *  01w returns 0
 *
 */

#include <linux/module.h>
#include <linux/init.h>
#include <linux/kernel.h>
#include <linux/version.h>
#include <linux/proc_fs.h>
#include <linux/platform_device.h>

#include "boxtype.h"
#include "pio.h"

#define procfs_name "boxtype"
#define DEVICENAME "boxtype"

int boxtype = 0;
struct proc_dir_entry *BT_Proc_File;

/************************************************************************
*
* Unfortunately there is no generic mechanism to unambiguously determine
* STBs from different manufactureres. Since each hardware platform needs
* special I/O pin handling it also requires a different kernel image.
* Setting platform device configuration in the kernel helps to roughly
* determine the STB type. Further identification can be based on reading
* an EPLD or I/O pins.
* To provide a platform identification data add a platform device
* to the board setup.c file as follows:
*
*   static struct platform_device boxtype_device = {
*        .name = "boxtype",
*        .dev.platform_data = (void*)NEW_ID
*   };
*
*   static struct platform_device *plat_devices[] __initdata = {
*           &boxtype_device,
*           &st40_ohci_devices,
*           &st40_ehci_devices,
*           &ssc_device,
*   ...
*
* Where NEW_ID is a unique integer identifying the platform and
* plat_devices is provided to the platform_add_devices() function
* during initialization of hardware resources.
*
************************************************************************/
static int boxtype_probe (struct device *dev)
{
  struct platform_device *pdev = to_platform_device (dev);

  if(pdev != NULL)
    boxtype = (int)pdev->dev.platform_data;

  return 0;
}

static int boxtype_remove (struct device *dev)
{
  return 0;
}

static struct device_driver boxtype_driver = {
  .name = DEVICENAME,
  .bus = &platform_bus_type,
  .owner = THIS_MODULE,
  .probe = boxtype_probe,
  .remove = boxtype_remove,
};

int procfile_read(char *buffer, char **buffer_location,
	          off_t offset, int buffer_length, int *eof, void *data)
{
	int ret;

	if (offset > 0) {
		ret  = 0;
	} else {
		ret = sprintf(buffer, "%d\n", boxtype);
	}
	return ret;
}

int __init boxtype_init(void)
{

	dprintk("[BOXTYPE] initializing ...\n");

// TODO: FIX THIS
#if LINUX_VERSION_CODE < KERNEL_VERSION(2,6,30)
	if (driver_register (&boxtype_driver) < 0)
	{
	  printk ("%s(): error registering device driver\n", __func__);
	}
#endif

	if(boxtype == 0)
	{
	  /* no platform data found, assume ufs910 */
	  boxtype = (STPIO_GET_PIN(PIO_PORT(4),5) << 1) | STPIO_GET_PIN(PIO_PORT(4), 4);
	}

	dprintk("[BOXTYPE] boxtype = %d\n", boxtype);

	BT_Proc_File = create_proc_read_entry(procfs_name, 0644, NULL, procfile_read, NULL);

	return 0;
}

void __exit boxtype_exit(void)
{
	dprintk("[BOXTYPE] unloading ...\n");
// TODO: FIX THIS
#if LINUX_VERSION_CODE < KERNEL_VERSION(2,6,30)
	driver_unregister (&boxtype_driver);
#endif
#if LINUX_VERSION_CODE < KERNEL_VERSION(2,6,26)
	remove_proc_entry(procfs_name, &proc_root);
#else
	remove_proc_entry(procfs_name, NULL);
#endif
}

module_init(boxtype_init);
module_exit(boxtype_exit);

MODULE_DESCRIPTION("Provides the type of an STB based on STb710x");
MODULE_AUTHOR("Team Ducktales");
MODULE_LICENSE("GPL");
