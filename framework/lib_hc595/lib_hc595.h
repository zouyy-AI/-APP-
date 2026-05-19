
//这个结构体用于引脚配置
typedef struct{
    uint8_t sda;
    uint8_t sck;
    uint8_t ud;
    uint8_t ccs;
    
}hc595_pin_t;


//这个结构体用于操作总线
typedef struct{
    void (*init)(hc595_pin_t* bus);
    void (*write)(hc595_pin_t* bus,uint8_t* buf,uint8_t len);
    void (*update)(hc595_pin_t* bus);
}hc595_opt_t;

extern hc595_opt_t* hc595_opt;
