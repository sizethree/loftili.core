#ifndef _LOFTILI_ROOT_CONTROLLER_H
#define _LOFTILI_ROOT_CONTROLLER_H

#include "dispatch/controller.h"

namespace loftili {

class RootController : public Controller {

public:
  ~RootController();
  void initialize();

};

}

#endif