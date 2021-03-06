

#include <linux/module.h>
#include <linux/i2o.h>
#include <linux/dcache.h>
#include <linux/namei.h>
#include <linux/fs.h>

#include <asm/uaccess.h>

#define OSM_NAME	"config-osm"
#define OSM_VERSION	"1.323"
#define OSM_DESCRIPTION	"I2O Configuration OSM"

/* access mode user rw */
#define S_IWRSR (S_IRUSR | S_IWUSR)

static struct i2o_driver i2o_config_driver;

/* Config OSM driver struct */
static struct i2o_driver i2o_config_driver = {
	.name = OSM_NAME,
};

#ifdef CONFIG_I2O_CONFIG_OLD_IOCTL
#include "i2o_config.c"
#endif

static int __init i2o_config_init(void)
{
	printk(KERN_INFO OSM_DESCRIPTION " v" OSM_VERSION "\n");

	if (i2o_driver_register(&i2o_config_driver)) {
		osm_err("handler register failed.\n");
		return -EBUSY;
	}
#ifdef CONFIG_I2O_CONFIG_OLD_IOCTL
	if (i2o_config_old_init()) {
		osm_err("old config handler initialization failed\n");
		i2o_driver_unregister(&i2o_config_driver);
		return -EBUSY;
	}
#endif

	return 0;
}

static void i2o_config_exit(void)
{
#ifdef CONFIG_I2O_CONFIG_OLD_IOCTL
	i2o_config_old_exit();
#endif

	i2o_driver_unregister(&i2o_config_driver);
}

MODULE_AUTHOR("Markus Lidel <Markus.Lidel@shadowconnect.com>");
MODULE_LICENSE("GPL");
MODULE_DESCRIPTION(OSM_DESCRIPTION);
MODULE_VERSION(OSM_VERSION);

module_init(i2o_config_init);
module_exit(i2o_config_exit);
