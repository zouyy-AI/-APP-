#include "framework.h"

#define rck_delay 50
#define sck_delay 20

static void init(hc595_pin_t* bus)
{
    gpio->set_mode(bus->sda,PIN_MODE_OUTPUT);
    gpio->set_mode(bus->sck,PIN_MODE_OUTPUT);
    gpio->set_mode(bus->ud,PIN_MODE_OUTPUT);
    gpio->set_mode(bus->ccs,PIN_MODE_OUTPUT);
}

static void write_byte(hc595_pin_t* bus,uint8_t dat)
{
    volatile uint32_t delay = 0;
    
    for(uint8_t i = 0;i < 8;i ++)
    {
        if(dat & 0x80)
            gpio->write(bus->sda , PIN_HIGH);
        else
            gpio->write(bus->sda , PIN_LOW);
        
        dat<<=1;
        
        gpio->write(bus->sck , PIN_LOW);
        delay = sck_delay;while(delay--);
        gpio->write(bus->sck , PIN_HIGH);
        delay = sck_delay;while(delay--);         
    }
    
}
static void write(hc595_pin_t* bus,uint8_t* buf,uint8_t len)
{
    for(uint8_t i = 0;i < len;i ++)
    {
        write_byte(bus , buf[len - 1 - i]);//ÏÈËÍÔ¶¶ËÊý¾Ý
    }
    
}
static void update(hc595_pin_t* bus)
{
    volatile uint32_t delay = 0;
    
    gpio->write(bus->ud , PIN_LOW);
    delay = rck_delay;while(delay--);
    gpio->write(bus->ud , PIN_HIGH);
    delay = rck_delay;while(delay--);  

}
static hc595_opt_t do_hc595_opt = {
    .init = init,
    .write = write,
    .update = update,
};

hc595_opt_t* hc595_opt = &do_hc595_opt;
