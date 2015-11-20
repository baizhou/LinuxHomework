Bus Types 

**总线类型**

Definition

**定义**

~~~~~~~~~~

See the kerneldoc for the struct bus_type.

    int bus_register(struct bus_type * bus);



Declaration

**声明**

~~~~~~~~~~~

Each bus type in the kernel (PCI, USB, etc) should declare one static object of this type. They must initialize the name field, and may optionally initialize the match callback.

**内核中每个总线类型（PCI、USB 等等）都应该声明一个此类型的静态对象。它们必须初始化该对象的 name 字段，然后可选的初始化 match 回调函数。**

    struct bus_type pci_bus_type = {
        .name = "pci",
        .match = pci_bus_match,
    };


The structure should be exported to drivers in a header file:

**这个结构体应该在头文件中向驱动程序导出：**

    extern struct bus_type pci_bus_type;



Registration

**注册**

~~~~~~~~~~~~

When a bus driver is initialized, it calls bus_register. This initializes the rest of the fields in the bus object and inserts it into a global list of bus types. Once the bus object is registered, the fields in it are usable by the bus driver. 

**当初始化一个总线驱动时，将会调用 bus_register。这时这个总线对象剩下的字段将被初始化，然后这个对象会被插入到总线类型的一个全局列表里去。一旦完成一个总线对象的注册，那么对于总线驱动来说它里面的字段就已经可用了。**


Callbacks

**回调函数**

~~~~~~~~~


match(): Attaching Drivers to Devices

**match()：给设备加载驱动**

~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~

The format of device ID structures and the semantics for comparing them are inherently bus-specific. Drivers typically declare an array of device IDs of devices they support that reside in a bus-specific driver structure.
 
**设备 ID 的结构和比较这些 ID 的语义本质上讲都是总线定制的。驱动通常会定义一个它所支持的设备 ID 的数组，并将这个数组驻留在总线定制的驱动结构中。**

The purpose of the match callback is provide the bus an opportunity to determine if a particular driver supports a particular device by comparing the device IDs the driver supports with the device ID of a particular device, without sacrificing bus-specific functionality or type-safety. 

**回调函数 match 的目的就是在不牺牲总线定制功能或类型安全的情况下，通过比较驱动所支持的 ID 与特定设备的 ID，给总线提供一个确定特定驱动是否支持特定设备的机会。**

When a driver is registered with the bus, the bus's list of devices is iterated over, and the match callback is called for each device that does not have a driver associated with it. 

**当一个驱动注册到总线后，将会遍历总线的设备列表，然后会为每个还没有和任何一个驱动关联的设备调用回调函数 match。**



Device and Driver Lists

**设备和驱动列表**

~~~~~~~~~~~~~~~~~~~~~~~


The lists of devices and drivers are intended to replace the local lists that many buses keep. They are lists of struct devices and struct device_drivers, respectively. Bus drivers are free to use the lists as they please, but conversion to the bus-specific type may be necessary. 

**设备和驱动列表是为了取代那些总线自己保存的本地列表。它们分别是 struct devices 和 struct device_drivers 的列表。总线驱动可以随意的使用这些列表，但有可能需要将其转换成总线定制类型。**

The LDM core provides helper functions for iterating over each list.

**LDM 核心提供了遍历各个列表的辅助函数。**

    int bus_for_each_dev(struct bus_type * bus, struct device * start, void * data,
                         int (*fn)(struct device *, void *));

    int bus_for_each_drv(struct bus_type * bus, struct device_driver * start,
                         void * data, int (*fn)(struct device_driver *, void *));


These helpers iterate over the respective list, and call the callback for each device or driver in the list. All list accesses are synchronized by taking the bus's lock (read currently). The reference count on each object in the list is incremented before the callback is called; it is decremented after the next object has been obtained. The lock is not held when calling the callback. 

**这些辅助函数迭代各个列表，然后为这些列表里的设备或驱动调用回调函数。所有列表的访问都是通过取得总线锁（实时读取）而保持同步的。列表中每个对象的引用计数都会在调用回调函数前递增；并且会在取得下一个对象之后递减。总线锁并不在回调函数调用期间继续保持。**


sysfs

~~~~~~~~

There is a top-level directory named 'bus'.

**这里有一个顶层目录：‘bus’。**

Each bus gets a directory in the bus directory, along with two default
directories:

**每个总线在 bus 目录下都有自己的目录，其中有两个默认目录：**

    /sys/bus/pci/
    |-- devices
    `-- drivers


Drivers registered with the bus get a directory in the bus's drivers
directory:

**在总线注册的驱动会得到一个该总线 drivers 目录的子目录：**

    /sys/bus/pci/
    |-- devices
    `-- drivers
       |-- Intel ICH
       |-- Intel ICH Joystick
       |-- agpgart
       `-- e100


Each device that is discovered on a bus of that type gets a symlink in the bus's devices directory to the device's directory in the physical hierarchy:

**在这种类型总线上发现的每个设备都会在总线的 deivces 目录下获得一个符号链接，该链接指向这个设备在物理层次中的目录。**

    /sys/bus/pci/
    |-- devices
    |  |-- 00:00.0 -> ../../../root/pci0/00:00.0
    |  |-- 00:01.0 -> ../../../root/pci0/00:01.0
    |  `-- 00:02.0 -> ../../../root/pci0/00:02.0
    `-- drivers


Exporting Attributes

**属性导出**

~~~~~~~~~~~~~~~~~~~~


    struct bus_attribute {
        struct attribute    attr;
        ssize_t (*show)(struct bus_type *, char * buf);
        ssize_t (*store)(struct bus_type *, const char * buf, size_t count);
    };


Bus drivers can export attributes using the BUS_ATTR macro that works similarly to the DEVICE_ATTR macro for devices. For example, a definition like this:

**总线驱动可以使用宏 BUS_ATTR 导出属性，它的工作方式就像 device 里的 DEVICE_ATTR 一样。例如，像这样定义：**

    static BUS_ATTR(debug,0644,show_debug,store_debug);


is equivalent to declaring:
**
这等于是定义一个这样的东西：**

    static bus_attribute bus_attr_debug;


This can then be used to add and remove the attribute from the bus's
sysfs directory using:

**使用以下函数，可以用来从总线的 sysfs 目录中添加或删除属性：**

    int bus_create_file(struct bus_type *, struct bus_attribute *);
    void bus_remove_file(struct bus_type *, struct bus_attribute *);


