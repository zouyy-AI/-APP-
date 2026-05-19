

typedef struct {
    unsigned char* buffer;
    unsigned int size;
    unsigned int head;
    unsigned int tail;
} loopbuf_t;

extern loopbuf_t* lb_uart0;

extern void cmd_line_work( void );
extern loopbuf_t* loopbuf_init(unsigned int size);
extern void loopbuf_free(loopbuf_t* lb);
extern unsigned int loopbuf_write(loopbuf_t* lb, const unsigned char* data, unsigned int n);
extern unsigned int loopbuf_read(loopbuf_t* lb, unsigned char* data, unsigned int n);
void loopbuf_reset( loopbuf_t* lb );

