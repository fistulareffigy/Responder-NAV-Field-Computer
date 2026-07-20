#pragma once

#include "AppContext.hpp"
#include "AppMetadata.hpp"
#include "Events.hpp"

namespace responder {

class IResponderApp {
public:
  virtual ~IResponderApp() = default;

  virtual const AppMetadata &metadata() const = 0;
  virtual bool onInstall(AppContext &context) { (void)context; return true; }
  virtual bool onStart(AppContext &context) { (void)context; return true; }
  virtual void onResume() {}
  virtual void onSuspend() {}
  virtual void onUpdate(uint32_t nowMs) { (void)nowMs; }
  virtual void onRender(DisplayCanvas &canvas) { (void)canvas; }
  virtual bool onInput(const InputEvent &event) { (void)event; return false; }
  virtual void onEvent(const SystemEvent &event) { (void)event; }
  virtual void onStop() {}
  virtual void onUninstall() {}
};

}  // namespace responder
