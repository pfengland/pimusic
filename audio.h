#include <alsa/asoundlib.h>

#define PCM_DEVICE "default"

void init_audio(void);

extern int buff_size;
extern char *buff;
extern snd_pcm_t *pcm_handle;
extern snd_pcm_uframes_t frames;
extern int rate;
