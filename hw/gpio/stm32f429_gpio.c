/*
 * STM32F429 GPIO Controller - Thesis implementation
 */

#include "qemu/osdep.h"
#include "qemu/log.h"
#include "hw/irq.h"
#include "hw/qdev-properties.h"
#include "hw/gpio/stm32f429_gpio.h"
#include "qom/object.h"
#include "qemu/bitops.h"

static const char * const type_desc = "STM32F420 Gpio module";

// register reset values

#define OTYPE_RESET_VALUE   0x00000000ul
#define OSPEEDR_RESET_VALUE 0x00000000ul
#define PUPDR_RESET_VALUE   0x00000000ul
#define ODR_RESET_VALUE     0x00000000ul
#define BSRR_RESET_VALUE    0x00000000ul
#define LCKR_RESET_VALUE    0x00000000ul
#define AFRH_RESET_VALUE    0x00000000ul
#define AFRL_RESET_VALUE    0x00000000ul

// Offsets of GPIO Module registers
typedef enum StmGpioRegOffset
{
    OFFSET_MODER = 0x00,
    OFFSET_OTYPER = 0x04,
    OFFSET_OSPEEDR = 0x08,
    OFFSET_PUPDR = 0x0C,
    OFFSET_IDR = 0x10,
    OFFSET_ODR = 0x14,
    OFFSET_BSRR = 0x18,
    OFFSET_LCKR = 0x1C,
    OFFSET_AFRL = 0x20,
    OFFSET_AFRH = 0x24
} StmGpioRegOffset;

// Update state of gpio module registers after write operation
static void update_gpio_state(STM32F429GpioState* s)
{

}

/* Read from the memory region. @addr is relative to @mr; @size is in bytes */
static uint64_t stm32f429_gpio_read(void *opaque, hwaddr offset, unsigned size)
{
    qemu_log("read - addr offset %zu size %u\n", offset, size);

    assert(offset <= STM32F429_GPIO_MMIO_SIZE);

    switch ((StmGpioRegOffset) offset)
    {
        case OFFSET_MODER ... OFFSET_PUPDR:
        {
            qemu_log("read MODER .. PUPDR\n");
            break;
        }
        case OFFSET_IDR:
        {
            qemu_log("Read idr\n");
            break;
        }
        case OFFSET_ODR:
        {
            qemu_log("Read odr\n");
            break;
        }
        case OFFSET_BSRR ... OFFSET_LCKR:
        {
            break;
        }
        case OFFSET_AFRL ... OFFSET_AFRH:
        {
            qemu_log("read BSRR .. AFRH\n");
            break;
        }
    }

    return 0;
}

/* Write to the memory region. @addr is relative to @mr; @size is in bytes */
static void stm32f429_gpio_write(void *opaque, hwaddr offset, uint64_t data,
                                 unsigned size)
{
    qemu_log("write - addr offset %zu data %zu size %u\n", offset, data, size);

    assert(offset <= STM32F429_GPIO_MMIO_SIZE);

    STM32F429GpioState *s = STM32F429_GPIO(opaque);

    switch ((StmGpioRegOffset) offset)
    {
        case OFFSET_MODER ... OFFSET_PUPDR:
        {
            qemu_log("write MODER .. PUPDR\n");
            update_gpio_state(s);
            break;
        }
        case OFFSET_IDR:
        {
            qemu_log("write idr\n");
            break;
        }
        case OFFSET_ODR:
        {
            qemu_log("write odr\n");
            break;
        }
        case OFFSET_LCKR:
        {
            if(size != 4)
            {
                // TODO: Not allowed
                break;
            }
            update_gpio_state(s);
            break;
        }
        case OFFSET_BSRR:
        case OFFSET_AFRL ... OFFSET_AFRH:
        {
            qemu_log("write BSRR .. AFRH\n");
            break;
        }
    }
}

// I/O operations on memory region
static const MemoryRegionOps memops = {
    .read = stm32f429_gpio_read,
    .write = stm32f429_gpio_write,
    .endianness = DEVICE_NATIVE_ENDIAN,
};

static void stm32f429_gpio_reset(DeviceState *ds)
{
    STM32F429GpioState *s = STM32F429_GPIO(ds);

    s->otyper = OTYPE_RESET_VALUE;
    s->lckr = LCKR_RESET_VALUE;

    s->odr = ODR_RESET_VALUE;
    // input data register reset value is undefined
}

static void stm32f429_gpio_init(Object *obj)
{
    STM32F429GpioState *s = STM32F429_GPIO(obj);

    memory_region_init_io(&s->mmio, OBJECT(s), &memops, s, TYPE_STM32F429_GPIO,
                          STM32F429_GPIO_MMIO_SIZE);
    sysbus_init_mmio(SYS_BUS_DEVICE(s), &s->mmio);
}

// TODO: Implement
static void stm32f429_gpio_realize(DeviceState *ds, Error **errp)
{
}

static void stm32f429_gpio_class_init(struct ObjectClass *klass, void *data)
{
    DeviceClass *dc = DEVICE_CLASS(klass);

    dc->realize = &stm32f429_gpio_realize;
    dc->reset = &stm32f429_gpio_reset;
    dc->desc = type_desc;
}

static const TypeInfo stm32f429_gpio_info = {
    .name          = TYPE_STM32F429_GPIO,
    .parent        = TYPE_SYS_BUS_DEVICE,
    .instance_size = sizeof(STM32F429GpioState),
    .instance_init = stm32f429_gpio_init,
    .class_init    = stm32f429_gpio_class_init,
};

static void stm32f429_gpio_register_types(void)
{
    type_register_static(&stm32f429_gpio_info);
}

type_init(stm32f429_gpio_register_types)
