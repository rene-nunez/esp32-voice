#ifndef RING_BUFFER_H
#define RING_BUFFER_H

#include <Arduino.h>

template<typename T, int Capacity>

class RingBuffer {
public:
  void enqueue(T item) {
    int next = (tail + 1) % Capacity;
    if (next != head) {
      buf[tail] = item;
      tail = next;
    }
  }

  bool empty() const {
    return head == tail;
  }

  T dequeue() {
    if (empty()) return T();
    T item = buf[head];
    head = (head + 1) % Capacity;
    return item;
  }

private:
  T buf[Capacity];
  int head = 0;
  int tail = 0;
};

#endif
