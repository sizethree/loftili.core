#include "audio/stream.h"

namespace loftili {

int AudioStream::playback(const void* in, void* out, FrameCount fpb, const TimeInfo* ti, StreamFlags f, void* data) {
  AudioStream* stream = static_cast<AudioStream*>(data);

  if(!stream || stream->buffer == 0)
    return 0;

  stream->playback(out);

  return 0;
}

AudioStream::AudioStream(std::string fname) : 
  p_stream(0), m_handle(0), filename(fname), 
  streaming(0), rate(0), channels(0), encoding(0),
  stream_size(0), buffer(0), ready(false) {

  log = new Logger(this);
  log->info("new audio stream created");

  if(initialize())
    prepare();
}

AudioStream::~AudioStream() {
  log->info("audio stream being deleted!");

  if(p_stream) {
    Pa_StopStream(p_stream);
    Pa_CloseStream(p_stream);
  }

  mpg123_close(m_handle);
  mpg123_delete(m_handle);

  if(buffer) {
    free(buffer);
    buffer = 0;
  }

  delete log;
}

bool AudioStream::initialize() {
  bool ok = true;
  log->info("initializing audio stream - opening mpg handle and portaudio stream");

  m_handle = mpg123_new(NULL, NULL);
  int m_error = mpg123_open(m_handle, filename.c_str());
  if(m_error == MPG123_ERR) {
    log->fatal("failed opening the audio stream\'s file");
    ok = false;
  }
  log->info("successfully opened file, proceeding to portaudio init");

  m_error = Pa_OpenDefaultStream(&p_stream, 0, 2, paFloat32, 44100, FRAME_PER_BUFFER, &AudioStream::playback, (void*)this);
  if(m_error != paNoError) {
    log->fatal("failed opening port audio\'s default stream");
    ok = false;
  }
  log->info("successfully opened a portaudio stream");

  return ok;
}

int AudioStream::prepare() {
  int m_error;
  mpg123_param(m_handle, MPG123_ADD_FLAGS, MPG123_FORCE_FLOAT, 0.);

  m_error = mpg123_getformat(m_handle, &rate, &channels, &encoding);
  if(m_error != MPG123_OK) {
    log->fatal("bad file format!");
    return 1;
  }

  log->info("file format checked out, prepping buffer");
  mpg123_format_none(m_handle);
  mpg123_format(m_handle, rate, channels, encoding);
  stream_size = FRAME_PER_BUFFER * 2.0;
  buffer = (float*) malloc(sizeof(float) * stream_size);
  ready = true;
}

int AudioStream::start() {
  if(!ready || streaming == 1) 
    return 0;

  streaming = 1;
  log->info("starting port audio stream");
  int m_error = Pa_StartStream(p_stream);
  return 0;
}

void AudioStream::playback(void* output_buffer) {
  size_t done = 0;
  mpg123_read(m_handle, (unsigned char*)buffer, stream_size * sizeof(float), &done);
  memcpy(output_buffer, buffer, sizeof(float) * stream_size);
}

}
