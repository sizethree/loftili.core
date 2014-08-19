#ifndef _LOFTILI_PLAYER_CONTROLLER_H
#define _LOFTILI_PLAYER_CONTROLLER_H

#include "dispatch/controller.h"
#include "audio/player.h"
#include "audio/manager.h"

namespace loftili {

class PlayerController : public Controller {

public:
  PlayerController();
  ~PlayerController();
  int respondTo(Request* req, Response* res);

protected:
  std::string logName() { return "PlayerController"; }

private:
  int start(Request* req, Response* res);
  int stop(Request* req, Response* res);

private:
  AudioPlayer* player;
  AudioManager* manager;

};

}


#endif
