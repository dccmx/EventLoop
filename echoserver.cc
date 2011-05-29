#include "eventloop.h"
#include "util.h"

using namespace eventloop;

class AcceptEvent: public FileEvent {
  public:
    virtual void Process(uint32_t events) {
    }
};

int main(int argc, char **argv) {
  int fd;
  EvengLoop el;
  AcceptEvent e;

  fd = bind_addr("0.0.0.0", 12345);
  if (fd == -1) {
    log_fatal("binding address", "%s", strerror(errno));
  }

  exit(EXIT_SUCCESS);
}

