#include <rtthread.h>
#include <rtdevice.h>
#include <stdio.h>
#include <stdint.h>
#include <stdlib.h>

#ifdef RT_USING_I2C

static int i2cmemrd(int argc, char **argv)
{
    uint8_t rbuf[64];
    uint8_t wbuf[1];
    struct rt_i2c_msg msg[2];
    struct rt_i2c_bus_device *bus;
    uint16_t slave;
    uint16_t size;

    if (argc != 5)
    {
        printf("Usage: i2cmemrd BUSNAME SLAVE POS SIZE\n");
        return -1;
    }

    bus = rt_i2c_bus_device_find(argv[1]);
    if (!bus)
    {
        printf("i2cbus: %s not found\n", argv[1]);
        return -1;
    }

    slave = (uint16_t)atoi(argv[2]);
    wbuf[0] = (uint8_t)atoi(argv[3]);
    size = (uint16_t)atoi(argv[4]);

    if (size > sizeof(rbuf) || size == 0)
    {
        printf("SIZE 1~64\n");
        return -1;
    }

    msg[0].flags = RT_I2C_WR;
    msg[0].addr = slave;
    msg[0].buf = wbuf;
    msg[0].len = 1;

    msg[1].flags = RT_I2C_RD;
    msg[1].addr = slave;
    msg[1].buf = rbuf;
    msg[1].len = size;

    if (rt_i2c_transfer(bus, msg, 2) == 2)
    {
        int i;

        for (i = 0; i < size; i++)
        {
            if (i % 16 == 0)
                printf("\n");
            printf("%02X ", rbuf[i]);
        }
        printf("\n");
    }
    else
    {
        printf("read fail\n");
    }

    return 0;
}
MSH_CMD_EXPORT(i2cmemrd, i2cmemrd BUSNAME SLAVE POS SIZE);

static int i2cmemwr(int argc, char **argv)
{
    uint8_t buf[64];
    uint8_t pos[1];
    struct rt_i2c_msg msg[2];
    struct rt_i2c_bus_device *bus;
    uint16_t slave;

    if (argc != 5)
    {
        printf("Usage: i2cmemwr BUSNAME SLAVE POS VALUE\n");
        return -1;
    }

    bus = rt_i2c_bus_device_find(argv[1]);
    if (!bus)
    {
        printf("i2cbus: %s not found\n", argv[1]);
        return -1;
    }

    slave = (uint16_t)atoi(argv[2]);
    pos[0] = (uint8_t)atoi(argv[3]);
    buf[0] = (uint8_t)atoi(argv[4]);

    msg[0].flags = RT_I2C_WR;
    msg[0].addr = slave;
    msg[0].buf = pos;
    msg[0].len = 1;

    msg[1].flags = RT_I2C_WR;
    msg[1].addr = slave;
    msg[1].buf = buf;
    msg[1].len = 1;

    if (rt_i2c_transfer(bus, msg, 2) != 2)
    {
        printf("write fail\n");
    }

    return 0;
}
MSH_CMD_EXPORT(i2cmemwr, i2cmemwr BUSNAME SLAVE POS VALUE);

#endif