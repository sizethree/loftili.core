#include "audio/player.h"

namespace loftili {

int AudioPlayer::onPlay(const void* input, void* output, FrameCount fpb, const TimeInfo* time_info, StreamFlags flags, void* data) {
  return ((AudioPlayer*)data)->whilePlaying(input, output, fpb, time_info, flags);
}

void AudioPlayer::onFinish(void* data) {
  return;
}

AudioPlayer::AudioPlayer() : playing(false), stream(0), ready(false), left_phase(0), device_info(0), right_phase(0), last_error(0) {
  log = new Logger(this);
  int success = Pa_Initialize();
  int devices = Pa_GetDeviceCount();

  if(success != paNoError || devices < 1)
    log->fatal("Unable to load portaudio library");
  else {
    log->info("Successfully loaded portaudio library");
    ready = prepare();
  }
}

bool AudioPlayer::prepare() {
  output_config.device = Pa_GetDefaultOutputDevice();
  if(output_config.device == paNoDevice) {
    log->fatal("Failed to get default output device");
    return false;
  } else
    log->info("Found default output device");

  device_info = Pa_GetDeviceInfo(output_config.device);
  if(device_info != 0) {
    log->info("Device name: ");
    log->info(device_info->name);
  }

  output_config.channelCount = 2;
  output_config.sampleFormat = paFloat32;
  output_config.suggestedLatency = device_info->defaultLowOutputLatency;
  output_config.hostApiSpecificStreamInfo = NULL;

  for(int i=0; i<TABLE_SIZE; i++ ) {
    sine[i] = (float) sin(((double)i/(double)TABLE_SIZE) * M_PI * 2.0);
  }

  return true;
}

AudioPlayer::~AudioPlayer() {
  Pa_Terminate();
  delete log;
}

int AudioPlayer::whilePlaying(const void* input, void* output, FrameCount fpb, const TimeInfo* time_info, StreamFlags flags) {
  float *out = (float*)output;
  unsigned long i;

  for(i=0; i < fpb; i++) {
    *out++ = sine[left_phase];
    *out++ = sine[right_phase];

    left_phase += 1;
    if(left_phase >= TABLE_SIZE)
      left_phase -= TABLE_SIZE;

    right_phase += 3;

    if( right_phase >= TABLE_SIZE )
      right_phase -= TABLE_SIZE;
  }

  return paContinue; 
}

int AudioPlayer::onFinished() {
  return 1;
}

bool AudioPlayer::readFile() {
  log->info("reading file!");
  std::string file_name = "/Users/dadley/src/loftili/core/some_nights.mp3";
  mpg123_init();


  std::ifstream f(file_name.c_str());
  if (f.good()) {
    log->info("file exsists, opening with mpg123");
  } else {
    log->info("file does not exist, cannot play");
  }   
  f.close();

  file_handle = mpg123_new(NULL, NULL);
  if(!file_handle) {
    log->info("unable to create an mpg123 handle");
    return false;
  }

  mpg123_open(file_handle, file_name.c_str());
  mpg123_close(file_handle);
  /*
  mpg123_open(file_handle, file_name.c_str());
  mpg123_close(file_handle);
  */

  return false;
}

void AudioPlayer::start() {
  if(playing || !ready)
    return;

  readFile();

  unsigned long fpb = paFramesPerBufferUnspecified;
  last_error = Pa_OpenStream(&stream, NULL, &output_config, SAMPLE_RATE, fpb, paClipOff, &AudioPlayer::onPlay, this);

  if (last_error != paNoError) {
    log->fatal("UNABLE TO OPEN STREAM");
    return;
  }

  last_error = Pa_SetStreamFinishedCallback(stream, &AudioPlayer::onFinish);

  if(last_error != paNoError) {
    Pa_CloseStream(stream);
    log->fatal("UNABLE TO CLOSE STREAM");
  }

  last_error = Pa_StartStream(stream);
  if(last_error != paNoError) {
    log->fatal("NOT ABLE TO OPEN");
  }

  playing = true;
}

bool AudioPlayer::isPlaying() {
  return playing;
}

void AudioPlayer::stop() {
  if(!playing)
    return;

  playing = false;
  Pa_StopStream(stream);
  Pa_CloseStream(stream);
  stream = 0;
  log->info("Closing stream");
}

}
