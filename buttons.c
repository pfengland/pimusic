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


int sample = 0;
int sample_pressed;
int direction = 0;


// save button states
int buttonState[5] = {1,1,1,1,1};



// audio thread function
void* audio_main(void *arg) {

  int ret;
  double decay = 2;
  double amp = 1;
  // maximum freq
  double freq = 200;

  init_audio();

  // calculate decay
  // how many samples it will take to decay
  int decay_samples = decay * rate;
  double sample_val = 0;
  
  printf("decay_samples: %d\n", decay_samples);

  while (1) {
    
    for (int i=0; i<buff_size; i++) {

      // calculate decay
      int cur_sample = sample % decay_samples;
      // percentage of the decay time we've reached
      double ratio = (double)cur_sample / (double)decay_samples;
      // amplitude - 1 - ratio
      if (direction)
	amp = 1.0 - ratio;
      else
	amp = ratio;


      int period_samples = (1.0/(freq*(amp*0.2+0.8))) * rate;

      
      // current sample from oscillator (-1 to 1)
      if ((sample % period_samples) > (period_samples / 2))
	sample_val = 1;
      else
	sample_val = -1;

     
      

      buff[i] = sample_val * amp * 0.0001 * 32767;
      
      sample++;
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
	buttonState[g] = state;

	// record the sample the button was pressed at
	if (g == 2 && !state)
	  direction = !direction;
      }
    }
  }

  pthread_join(audio_thread, NULL);
  
  return EXIT_SUCCESS;
}

