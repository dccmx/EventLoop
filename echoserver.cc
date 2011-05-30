#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>
#include <sys/socket.h>
#include <sys/types.h>
#include <arpa/inet.h>
#include <netinet/in.h>
#include <errno.h>

#include "eventloop.h"

using namespace eventloop;

class AcceptEvent: public FileEvent {
 public:
  void Process(uint32_t events) {
    if (events & FileEvent::READ) {
      uint32_t size = 0;
      struct sockaddr_in addr;

      int fd = accept(this->GetFile(), (struct sockaddr*)&addr, &size);
      printf("read\n");
    }

    if (events & FileEvent::ERROR) {
      close(this->GetFile());
      printf("close\n");
    }
  }
};

int main(int argc, char **argv) {
  int fd;
  EventLoop el;
  AcceptEvent e;

  e.SetType(FileEvent::READ | FileEvent::ERROR);

  fd = BindTo("0.0.0.0", 12345);
  if (fd == -1) {
    printf("binding address %s", strerror(errno));
    return -1;
  }

  e.SetFile(fd);

  el.AddEvent(&e);

  el.Loop();

  exit(EXIT_SUCCESS);
}

