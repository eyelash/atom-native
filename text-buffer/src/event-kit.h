#ifndef EVENT_KIT_H_
#define EVENT_KIT_H_

#include <functional>
#include <vector>

template <class... A> class Emitter {
  std::vector<std::function<void(A...)>> handlers;
public:
  void on(std::function<void(A...)> handler, bool unshift = false) {
    if (unshift) {
      handlers.insert(handlers.begin(), handler);
    } else {
      handlers.push_back(handler);
    }
  }
  void emit(A... arguments) {
    for (const auto &handler : handlers) {
      handler(arguments...);
    }
  }
};

#endif // EVENT_KIT_H_
