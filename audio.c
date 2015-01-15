#include <stdio.h>
#include "audio.h"

snd_pcm_t *pcm_handle;
char *buff;
int buff_size;
snd_pcm_uframes_t frames;

void init_audio(void) {

  // alsa setup
  unsigned int pcm, tmp, dir;
  int rate=44100, channels=2, seconds=1;

  snd_pcm_hw_params_t *params;

  // open pcm device in playback mode
  if (pcm = snd_pcm_open(&pcm_handle, PCM_DEVICE, SND_PCM_STREAM_PLAYBACK, 0) < 0) {
    printf("error opening pcm device %s : %s\n", PCM_DEVICE, snd_strerror(pcm));
    exit(EXIT_FAILURE);
  }
  snd_pcm_hw_params_alloca(&params);
  snd_pcm_hw_params_any(pcm_handle, params);
  // set parameters
  if (pcm = snd_pcm_hw_params_set_access(pcm_handle, params, SND_PCM_ACCESS_RW_INTERLEAVED) < 0) {
    printf("error setting pcm interleaved mode - %s\n", snd_strerror(pcm));
    exit(EXIT_FAILURE);
  }
  if (pcm = snd_pcm_hw_params_set_format(pcm_handle, params, SND_PCM_FORMAT_S16_LE) < 0) {
    printf("error setting pcm format - %s\n", snd_strerror(pcm));
    exit(EXIT_FAILURE);
  }
  if (pcm = snd_pcm_hw_params_set_channels(pcm_handle, params, channels) < 0) {
    printf("error setting pcm channels - %s\n", snd_strerror(pcm));
    exit(EXIT_FAILURE);
  }
  if (pcm = snd_pcm_hw_params_set_rate_near(pcm_handle, params, &rate, 0) < 0) {
    printf("error setting pcm rate - %s\n", snd_strerror(pcm));
    exit(EXIT_FAILURE);
  }

  // set number/size of periods (latency)
  int periods = 2;
  if (pcm = snd_pcm_hw_params_set_periods(pcm_handle, params, periods, 0) < 0) {
    printf("error setting number of periods - %s\n", snd_strerror(pcm));
    exit(EXIT_FAILURE);
  }
  //  int periodsize = 8192;
  int periodsize = 4096;
  if (pcm = snd_pcm_hw_params_set_buffer_size(pcm_handle, params, (periodsize * periods) >> 2) < 0) {
    printf("error setting buffer size - %s\n", snd_strerror(pcm));
    exit(EXIT_FAILURE);
  }
  
  // write parameters
  if (pcm = snd_pcm_hw_params(pcm_handle, params) < 0) {
    printf("error writing pcm parameters - %s\n", snd_strerror(pcm));
    exit(EXIT_FAILURE);
  }
  
  // report settings
  printf("pcm name = %s\n", snd_pcm_name(pcm_handle));
  printf("pcm state = %s\n", snd_pcm_state_name(snd_pcm_state(pcm_handle)));

  // allocate buffer to hold a period
  snd_pcm_hw_params_get_period_size(params, &frames, 0);
  buff_size = frames * channels * 2;
  buff = malloc(buff_size);
  printf("buffer allocated\n");

  // write samples to audio device
  snd_pcm_hw_params_get_period_time(params, &tmp, NULL);
  printf("got period time\n");
}
