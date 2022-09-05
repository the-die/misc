#include "msgqueue.h"

#include <cassert>
#include <cstddef>
#include <iostream>
#include <type_traits>

struct Msg1 {
  char m0;
  short m1;
  int m2;
  double m3;
  char m4[64];
  void* link;
};

struct Msg2 {
  char m0;
  short m1;
  int m2;
  double m3;

 private:
  char m4[64];
  void* link;
};

#define PRINT_LAYOUT_INFO(Type)                                    \
  do {                                                             \
    if (std::is_standard_layout<Type>::value)                      \
      std::cout << #Type << " is standard layout" << std::endl;    \
    else                                                           \
      std::cout << #Type << " isn't standard layout" << std::endl; \
  } while (false);

void PrintMsg(const Msg1* msg) {
  if (!msg) {
    std::cout << "nothing!" << std::endl;
    return;
  }

  std::cout << "m0: " << msg->m0 << ", m1: " << msg->m1 << ", m2: " << msg->m2
            << ", m3: " << msg->m3 << ", m4: " << msg->m4 << std::endl;
}

int main() {
  PRINT_LAYOUT_INFO(Msg1);
  PRINT_LAYOUT_INFO(Msg2);

  size_t linkoff = offsetof(Msg1, link);
  MsgQueue mq(10, linkoff);

  auto msg_in1 = new Msg1;
  msg_in1->m0 = 'A';
  msg_in1->m1 = 2022;
  msg_in1->m2 = 1000000;
  msg_in1->m3 = 3.14;
  strcpy(msg_in1->m4, "hello, world");
  mq.Put(msg_in1);

  auto msg_out1 = reinterpret_cast<Msg1*>(mq.Get());
  PrintMsg(msg_out1);
  assert(msg_in1 == msg_out1);

  mq.SetNonblock();
  auto msg_out2 = reinterpret_cast<Msg1*>(mq.Get());
  PrintMsg(msg_out2);
  assert(msg_out2 == nullptr);

  return 0;
}
