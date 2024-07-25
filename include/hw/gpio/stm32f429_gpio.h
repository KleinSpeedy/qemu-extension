#ifndef STM32F429_GPIO_H
#define STM32F429_GPIO_H

#include "hw/sysbus.h"
#include "qom/object.h"

/* GPIO Memory range */
#define STM32F429_GPIO_MMIO_SIZE 0x400

/**
 * @brief Type represents a GPIO module in STM32F429 microcontroller
 */
struct STM32F429GpioState
{
    /* <private> */
    SysBusDevice parent_obj;

    /* <public> */
    struct MemoryRegion mmio;

    /* Configuration registers */
    uint32_t moder;
    uint32_t otyper;
    uint32_t pupdr;
    uint32_t ospeedr;

    /* Data registers */
    uint32_t idr;
    uint32_t odr;

    /* Bit set/reset register */
    uint32_t bsrr;

    /* Locking register */
    uint32_t lckr;

    /* Alternate function (high/low) register */
    uint32_t afhr;
    uint32_t aflr;
};

#define TYPE_STM32F429_GPIO "stm32f429_gpio"
OBJECT_DECLARE_SIMPLE_TYPE(STM32F429GpioState, STM32F429_GPIO)

#endif // STM32F429_GPIO_H
