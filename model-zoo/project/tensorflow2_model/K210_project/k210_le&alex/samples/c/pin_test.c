#include <rtthread.h>
#include <rtdevice.h>
#include <string.h>
#include <stdio.h>
#include <stdlib.h>

#ifdef RT_USING_PIN
static void pinm(int argc, char **argv)
{
    int pin;
    int m = PIN_MODE_INPUT;

    if (argc < 3)
    {
        printf("pinm 0 o\n");
        printf("pinm 0 i\n");
        return;
    }

    pin = atoi(argv[1]);
    if (pin < 0)
    {
        printf("can't get pin(%s)\n", argv[1]);
        return;
    }

    if (strcmp(argv[2], "o") == 0)
    {
        m = PIN_MODE_OUTPUT;
    }

    rt_pin_mode(pin, m);
}
MSH_CMD_EXPORT(pinm, pin mode set);

static void pinw(int argc, char **argv)
{
    int pin;
    int v;

    if (argc < 3)
    {
        printf("pinw 0 0\n");
        printf("pinw 0 1\n");
        return;
    }

    pin = atoi(argv[1]);
    if (pin < 0)
    {
        printf("can't get pin(%s)\n", argv[1]);
        return;
    }

    v = argv[2][0] - '0';
    rt_pin_write(pin, v);
}
MSH_CMD_EXPORT(pinw, pin Write);

static void pinr(int argc, char **argv)
{
    int pin;
    int v;

    if (argc < 3)
    {
        printf("pinr 0\n");
        printf("pinr 0\n");
        return;
    }

    pin = atoi(argv[1]);
    if (pin < 0)
    {
        printf("can't get pin(%s)\r\n", argv[1]);
        return;
    }

    v = rt_pin_read(pin);
    printf("pin value %d\n", v);
}
MSH_CMD_EXPORT(pinr, pin read);
#endif
