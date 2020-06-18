#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdint.h>
#include <errno.h>
#include <unistd.h>
#include <sys/fcntl.h>
#include <sys/mman.h>

#define GPIO_6     0x6000d500

struct GPIO_mem {
    uint32_t CNF[4];
    uint32_t OE[4];
    uint32_t OUT[4];
    uint32_t IN[4];
    uint32_t INT_STA[4];
    uint32_t INT_ENB[4];
    uint32_t INT_LVL[4];
    uint32_t INT_CLR[4];
};

int main(void)
{
    int fd = open("/dev/mem", O_RDWR | O_SYNC);
    if (fd < 0) {
        perror("/dev/mem");
        fprintf(stderr, "please run this program as root\n");
        exit(1);
    }

    int pagesize = getpagesize();
    int pagemask = pagesize-1;
	
    void *base = mmap(0, pagesize, PROT_READ | PROT_WRITE, MAP_SHARED, fd, (GPIO_6 & ~pagemask));
    if (base == NULL) {
        perror("mmap()");
        exit(1);
    }

    GPIO_mem volatile *pin = (GPIO_mem volatile *)((char *)base + (GPIO_6 & pagemask));

    pin->CNF[1] = 0x00ff;
    pin->OE[1] = 0xff;
    pin->OUT[1] = 0xff;
    pin->INT_ENB[1] = 0x00;

    uint8_t val = 0xff;
    while (true) {
        sleep(1);
        val = val ^ 0xff;
        pin->OUT[1] = val;
    }

    return 0 ;
}
