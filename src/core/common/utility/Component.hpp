#pragma once
#include "core/common/utility/EventEmitter.hpp"

class Component {
  protected:
  EventEmitter& global;
  EventEmitter  local;

  public:
  Component(EventEmitter& emitter): global(emitter) {}
  virtual ~Component() = default;

  virtual void start() {}
  virtual void stop() {}
  virtual void tick() {}

  EventEmitter& get_global() { return global; }
  EventEmitter& get_local() { return local; }
};
