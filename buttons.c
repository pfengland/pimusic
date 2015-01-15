// gcc -lasound -lpthread -std=gnu11 -o buttons buttons.c gpio.c audio.c

#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <stdlib.h>
#include <fcntl.h>
#include <sys/mman.h>
#include <unistd.h>
#include <pthread.h>

#include "gpio.h"
#include "audio.h"

// save button states
int buttonState[5] = {1,1,1,1,1};

void buttonChanged(int g, int state) {
  if (state) {
    printf("Button %d Released!\n", g - 1);    
  } else {
    printf("Button %d Pressed!\n", g - 1);
  }
}

// audio thread function
void* audio_main(void *arg) {

  int ret;

  init_audio();

  while (1) {
    for (int i=0; i<buff_size; i++) {
            if (buttonState[2])
      	buff[i] = 0;
            else
	buff[i] = rand();
    }

    if (ret = snd_pcm_writei(pcm_handle, buff, frames) == -EPIPE) {
      printf("xrun\n");
      snd_pcm_prepare(pcm_handle);
    } else if (ret < 0) {
      printf("error writing to pcm device - %s\n", snd_strerror(ret));
      exit(EXIT_FAILURE);
    }
  }
}

int main(int argc, char *argv[]) {

  int g;
  int ret;

  // set up gpio pointer for direct access
  setup_io();

  // set gpio pins 234 to input
  for (g=2; g<5; g++) {
    INP_GPIO(g);
  }

  // create the audio thread
  pthread_t audio_thread;
  ret = pthread_create(&audio_thread, NULL, audio_main, NULL);
  if (ret) {
    printf("failed to create audio thread - return code %d\n", ret);
    perror("");
    exit(EXIT_FAILURE);
  }
  
  init_audio();

  // cleanup
  //  snd_pcm_drain(pcm_handle);
  //  snd_pcm_close(pcm_handle);
  //  free(buff);
  
  // main loop - check for button state changes

  int state = 0;
  
  while (1) {

    for (g=2; g<5; g++) {

      state = GET_GPIO(g) ? 1 : 0;

      if (state != buttonState[g]) {
	//	buttonChanged(g, state);
	buttonState[g] = state;
      }
    }
  }

  pthread_join(audio_thread, NULL);
  
  return EXIT_SUCCESS;
}

