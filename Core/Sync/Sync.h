#pragma once

#include "../Definitions.h"

struct Packet {
  DataElement* e;
  int id;

  void send(Network* net) {

  }
};

/*class PacketStream {
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

class PacketStream {

  std::mutex mtx;
  std::condition_variable cv;

  list<Packet> packets_buffer[2];

  int currentOwner = -1;
  list<Packet>* packets;
  list<Packet>* readPacket;

  bool running = true;

  thread sender;

public:
  PacketStream(Network* to) {
    swapBuffers();
    sender = thread(to);
  }

  void shutDown() {
    {
      std::unique_lock<std::mutex> lck(mtx);
      running = false;
    }
  }

  void pushPacket(Packet in) {
    std::unique_lock<std::mutex> lck(mtx);
    packets->push_back(in);
    cv.notify_one();
  }

  bool packetsAvailable() { return packets->size != 0; }

  void swapBuffers() {
    std::unique_lock<std::mutex> lck(mtx);
    readPacket = packets;
    if (currentOwner == 0) {
      currentOwner = 1;
    } else {
      currentOwner = 0;
    }
    packets = &packets_buffer[currentOwner];
  }

  void keepSending(Network* to) {
    while (running) {
      {
        std::unique_lock<std::mutex> lck(mtx);
        cv.wait(lck, packetsAvailable);
        swapBuffers();
      }
      while (readPacket->size()) {
        readPacket->front().send(to);
        readPacket->pop_front();
      }
    }
  }
};
