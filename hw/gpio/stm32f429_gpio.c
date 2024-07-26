/*
 * STM32F429 GPIO Controller - Thesis implementation
 */

#include "qemu/osdep.h"
#include "qemu/log.h"
#include "hw/gpio/stm32f429_gpio.h"
#include "migration/vmstate.h"
#include "hw/qdev-properties.h"

static const char * const type_desc = "STM32F420 Gpio module";

// register reset value unless otherwise specified by ref-manual
#define DEFAULT_RESET_VALUE 0x00000000ul

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

/* Read from the memory region. @addr is relative to @mr; @size is in bytes */
static uint64_t stm32f429_gpio_read(void *opaque, hwaddr offset, unsigned size)
{
    qemu_log("read - addr offset %zu size %u\n", offset, size);

    if(offset <= STM32F429_GPIO_MMIO_SIZE)
    {
        qemu_log_mask(LOG_GUEST_ERROR, "GPIO: offset out of bounds");
        return 0;
    }

    STM32F429GpioState *s = opaque;

    switch((StmGpioRegOffset) offset)
    {
        case OFFSET_MODER:
        {
            return s->moder;
        }
        case OFFSET_OTYPER:
        {
            return s->otyper;
        }
        case OFFSET_OSPEEDR:
        {
            return s->ospeedr;
        }
        case OFFSET_PUPDR:
        {
            return s->pupdr;
        }
        case OFFSET_IDR:
        {
            return s->idr;
        }
        case OFFSET_ODR:
        {
            return s->odr;
        }
        case OFFSET_BSRR:
        {
            return s->bsrr;
        }
        case OFFSET_LCKR:
        {
            qemu_log_mask(LOG_UNIMP, "GPIO: Locking registers not supported");
            return s->bsrr;
        }
        case OFFSET_AFRL:
        {
            qemu_log_mask(LOG_UNIMP,
                    "GPIO: Alternate functions (low) not supported");
            return s->aflr;
        }
        case OFFSET_AFRH:
        {
            qemu_log_mask(LOG_UNIMP,
                    "GPIO: Alternate functions (high) not supported");
            return s->afhr;
        }
    }

    return 0;
}

// Update state of gpio module registers after write operation
static void update_gpio_state(STM32F429GpioState* s)
{

}

/* Write to the memory region. @addr is relative to @mr; @size is in bytes */
static void stm32f429_gpio_write(void *opaque, hwaddr offset, uint64_t data,
                                 unsigned size)
{
    qemu_log("write - addr offset %zu data %zu size %u\n", offset, data, size);

    if(offset <= STM32F429_GPIO_MMIO_SIZE)
    {
        qemu_log_mask(LOG_GUEST_ERROR, "GPIO: offset out of bounds");
        return;
    }

    STM32F429GpioState *s = opaque;

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
    .impl = {
        .min_access_size = 1,
        .max_access_size = 4,
        .unaligned = false
    },
    .valid = {
        .min_access_size = 1,
        .max_access_size = 4,
        .unaligned = false
    }
};

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
    (void)ds;
    (void)errp;
}

static const VMStateDescription vmstate_stm32f429_gpio = {
    .name = TYPE_STM32F429_GPIO,
    .version_id = 1,
    .minimum_version_id = 1,
    .fields = (const VMStateField[]) {
        VMSTATE_UINT32(moder_reset_val,     STM32F429GpioState),
        VMSTATE_UINT32(ospeedr_reset_val,     STM32F429GpioState),
        VMSTATE_UINT32(pupdr_reset_val,     STM32F429GpioState),
        VMSTATE_END_OF_LIST()
    }
};

// Device properties holding specific reset values
static Property properties_stm32f42_gpio[] = {
    DEFINE_PROP_UINT32("moder_reset_val", STM32F429GpioState,
            moder_reset_val, DEFAULT_RESET_VALUE),
    DEFINE_PROP_UINT32("ospeedr_reset_val", STM32F429GpioState,
            ospeedr_reset_val, DEFAULT_RESET_VALUE),
    DEFINE_PROP_UINT32("pupdr_reset_val", STM32F429GpioState,
            pupdr_reset_val, DEFAULT_RESET_VALUE),
    DEFINE_PROP_END_OF_LIST()
};

static void stm32f429_gpio_class_init(struct ObjectClass *klass, void *data)
{
    DeviceClass *dc = DEVICE_CLASS(klass);

    device_class_set_props(dc, properties_stm32f42_gpio);
    dc->realize = &stm32f429_gpio_realize;
    dc->desc = type_desc;
    dc->vmsd = &vmstate_stm32f429_gpio;
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
