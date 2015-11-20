Driver Binding

**驱动绑定**

Driver binding is the process of associating a device with a device
driver that can control it. Bus drivers have typically handled this
because there have been bus-specific structures to represent the
devices and the drivers. With generic device and device driver
structures, most of the binding can take place using common code.

**驱动绑定就是一个将能够控制某个设备的设备驱动和该设备联系起来的
过程。因为存在着总线定制结构为代表的设备和驱动，所以总线驱动有
自己的处理方法。而对于那些通用的设备和设备驱动，大部分的绑定操
作都可以使用共同的代码。**


Bus

**总线**

~~~

The bus type structure contains a list of all devices that are on that bus
type in the system. When device_register is called for a device, it is
inserted into the end of this list. The bus object also contains a
list of all drivers of that bus type. When driver_register is called
for a driver, it is inserted at the end of this list. These are the
two events which trigger driver binding.

**总线类型结构包含了一个在系统中这个总线类型上所有设备的列表。当
为一个设备调用 device_register 的时候，该设备将被插入到这个列表的
末端。总线对象还包含一个属于该总线类型的所有驱动的列表。当为一
个驱动调用 driver_register 的时候，该驱动将被插入到这个列表的末端。
这就是能够触发驱动绑定两个事件。**


device_register
~~~~~~~~~~~~~~~

When a new device is added, the bus's list of drivers is iterated over
to find one that supports it. In order to determine that, the device
ID of the device must match one of the device IDs that the driver
supports. The format and semantics for comparing IDs is bus-specific. 
Instead of trying to derive a complex state machine and matching
algorithm, it is up to the bus driver to provide a callback to compare
a device against the IDs of a driver. The bus returns 1 if a match was
found; 0 otherwise.
**
当添加一个新设备时，会迭代总线的驱动列表，以找到一个支持该设备
的驱动。为了确保这点，该设备的 ID 必须和那个驱动所支持的设备 ID
中的一个相匹配。 而比较 ID 的格式和语义则都是总线定制的。它是由
总线驱动提供的一个回调函数来比较设备和驱动，而不是试图推导出一
个复杂的状态机和匹配算法。**

    int match(struct device * dev, struct device_driver * drv);


If a match is found, the device's driver field is set to the driver
and the driver's probe callback is called. This gives the driver a
chance to verify that it really does support the hardware, and that
it's in a working state. 

**如果匹配成功，设备的 driver 字段会设置成指向该驱动，然后就调用该驱
动的 probe 回调函数。这将给这个驱动一个机会去确认它的确支持这个硬
件，并且该硬件正处于工作状态中。**

Device Class

**设备的 class**

~~~~~~~~~~~~

Upon the successful completion of probe, the device is registered with
the class to which it belongs. Device drivers belong to one and only one
class, and that is set in the driver's devclass field. 
devclass_add_device is called to enumerate the device within the class
and actually register it with the class, which happens with the
class's register_dev callback.

**在成功完成 probe 的时候，设备也与它所属的 class 注册完成。设备驱动属
于一个且唯一一个 class，它被设置在驱动的 devclass 字段。在调用 class 的
回调函数 register_dev 时，devclass_add_device 会被调用来枚举属于该 class
的设备，并真正的用 class 将设备进行注册。**


Driver

**驱动**

~~~~~~

When a driver is attached to a device, the device is inserted into the
driver's list of devices. 

**当一个驱动连接到一个设备时，该设备就被插入到这个驱动的设备列表里去。**


sysfs

~~~~~

A symlink is created in the bus's 'devices' directory that points to
the device's directory in the physical hierarchy.

**在总线的‘devices’目录中会创建一个符号链接，该链接指向在物理层次中的设
备目录。**

A symlink is created in the driver's 'devices' directory that points
to the device's directory in the physical hierarchy.

**在驱动的‘devices’目录中会创建一个符号链接，该链接指向在物理层次中的设
备目录。**

A directory for the device is created in the class's directory. A
symlink is created in that directory that points to the device's
physical location in the sysfs tree. 

**在 class 的目录中会为该设备创建一个目录。然后会在该目录中创建一个符号
链接，该链接指向这个设备在 sysfs 树中的物理位置。**

A symlink can be created (though this isn't done yet) in the device's
physical directory to either its class directory, or the class's
top-level directory. One can also be created to point to its driver's
directory also. 

**在设备的物理目录下，都可以创建一个符号链接（即使这还没有完成）来指向
它的 class 目录或 class 的顶层目录。同时也可以创建指向它的驱动目录的符号
链接。**


driver_register

~~~~~~~~~~~~~~~

The process is almost identical for when a new driver is added. 
The bus's list of devices is iterated over to find a match. Devices
that already have a driver are skipped. All the devices are iterated
over, to bind as many devices as possible to the driver.

**当添加一个新驱动时，也几乎是相同的流程。将迭代总线上的设备列表将用来找
到一个匹配。那些已经绑定的驱动的设备将被忽略。所有的设备都将参加迭代，
目的是为这个新驱动绑定尽量多的设备。**


Removal

**移除**

~~~~~~~

When a device is removed, the reference count for it will eventually
go to 0. When it does, the remove callback of the driver is called. It
is removed from the driver's list of devices and the reference count
of the driver is decremented. All symlinks between the two are removed.

**当移除一个设备时，它的引用计数也最终会变成 0。这时，驱动的回调函数 remove
会被调用。它将从驱动的设备列表里移除这个设备并递减该驱动的引用计数。最后，
这两者之间的符号链接都将被移除。**

When a driver is removed, the list of devices that it supports is
iterated over, and the driver's remove callback is called for each
one. The device is removed from that list and the symlinks removed. 

**当移除一个驱动时，将迭代它所支持的设备列表，并对每一个设备调用驱动的 remove
回调函数。设备将从那个列表中移除，然后符号链接也将被移除。**