#include "commands/audio/start.h"
#include "engine.h"

namespace loftili {

namespace commands {

namespace audio {

void Start::Execute(loftili::Engine *engine) {
  loftili::audio::Playback *playback = engine->Get<loftili::audio::Playback>();
  loftili::api::Registration *registration = engine->Get<loftili::api::Registration>();

  if(!playback) return;

  playback->Start();
}

void Start::operator()(loftili::Engine *engine) {
  return Execute(engine);
}

}

}

}
