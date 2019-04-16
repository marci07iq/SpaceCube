#pragma once

#include "../Definitions.h"

/*struct Packet {
  DataElement* e;
  int id;

  void send(Network* net) {

  }
};

class PacketStream {
  std::mutex m;
  std::condition_variable cv;

  list<Packet> packets_buffer[2];

  int currentOwner = -1;
  list<Packet>* packets;

  bool running = true;
  void shutDown() {
    {
      std::lock_guard<std::mutex> lock(m);
      running = false;
    }
  }

  void pushPacket(Packet in) {
    std::lock_guard<std::mutex> lock(m);
    packets->push_back(in);
    cv.notify_all();
  }
  

  void swapBuffers() {
    std::lock_guard<std::mutex> lock(m);
    if (currentOwner == 0) {
      currentOwner = 1;
    } else {
      currentOwner = 0;
    }
    packets = &packets_buffer[currentOwner];
  }

  void keepSending() {
    while (running) {
      m.lock();
      if (packets->size() == 0) {
        m.unlock();
        std::unique_lock<std::mutex> lk(m);
        cv.wait(lk)
      }
      swapBuffers();
      m.unlock();
    }
  }
};*/