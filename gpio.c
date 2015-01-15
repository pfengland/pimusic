#include <stdio.h>
#include "gpio.h"

int mem_fd = 0;
void* gpio_map = NULL;
volatile unsigned *gpio = NULL;

void setup_io(void) {

  // open memory device
  if ((mem_fd = open("/dev/mem", O_RDWR|O_SYNC)) < 0) {
    printf("can't open /dev/mem");
    exit(EXIT_FAILURE);
  }

  // map gpio memory
  gpio_map = mmap(NULL, BLOCK_SIZE, PROT_READ|PROT_WRITE, MAP_SHARED, mem_fd, GPIO_BASE);

  close(mem_fd);

  if (gpio_map == MAP_FAILED) {
    printf("mmap error %d\n", (int)gpio_map);
    perror("");
    exit(EXIT_FAILURE);
  }

  gpio = (volatile unsigned *)gpio_map;
}
