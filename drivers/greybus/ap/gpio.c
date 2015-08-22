#include <phabos/greybus.h>
#include <phabos/gpio.h>

#include "../gpio-gb.h"

#include <errno.h>

static struct gb_device *gb_device;

static struct gpio_ops gb_gpio_device_ops = {
};

void gb_gpio_line_count_cb(struct gb_operation *op)
{
    struct gb_gpio_line_count_response *resp;
    struct gpio_device *gpio;
    int retval;

    dev_info(&op->greybus->device, "line count received: %hhu\n",
             gb_operation_get_request_result(op));

    if (gb_operation_get_request_result(op) != GB_OP_SUCCESS) {
        retval = -EPROTO; // TODO write gb_op_result_to_errno
        goto error_operation_failed;
    }

    dev_info(&op->greybus->device, "line count received OK\n");

    resp = gb_operation_get_request_payload(op->response);

    gpio = kzalloc(sizeof(*gpio), MM_KERNEL);
    if (!gpio) {
        retval = -ENOMEM;
        goto error_alloc_gpio_device;
    }

    gpio->base = -1;
    gpio->count = resp->count + 1;
    gpio->ops = &gb_gpio_device_ops;
    gpio->device.priv = op->greybus;

    device_register(&gpio->device);
    gpio_device_register(gpio);

    dev_info(&op->greybus->device, "new GPIO device: GPIO%d to GPIO%d\n",
             gpio->base, gpio->base + gpio->count);

error_alloc_gpio_device:
error_operation_failed:
    return;
}

int gb_gpio_connected(struct greybus *bus, unsigned cport) // TODO static
{
    struct gb_operation *op;

    op = gb_operation_create(bus, cport, GB_GPIO_TYPE_LINE_COUNT, 0);
    if (!op)
        return -ENOMEM;

    dev_debug(&bus->device, "gpio line count sent\n");

    gb_operation_send_request(op, gb_gpio_line_count_cb, true);

    gb_operation_destroy(op);
    return 0;
}

static uint8_t gb_gpio_irq_event(struct gb_operation *op)
{
    return GB_OP_SUCCESS;
}

static struct gb_operation_handler gb_gpio_handlers[] = {
    GB_HANDLER(GB_GPIO_TYPE_IRQ_EVENT, gb_gpio_irq_event),
};

struct gb_driver gpio_driver = {
    .op_handlers = (struct gb_operation_handler*) gb_gpio_handlers,
    .op_handlers_count = ARRAY_SIZE(gb_gpio_handlers),
};

static int gb_gpio_probe(struct device *device)
{
    gb_device = containerof(device, struct gb_device, device);
    return gb_register_driver(gb_device->bus, 2, &gpio_driver);
}

__driver__ struct driver gb_gpio_driver = {
    .name = "gb-ap-gpio-phy",
    .probe = gb_gpio_probe,
};