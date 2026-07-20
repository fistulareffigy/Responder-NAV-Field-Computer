#include <responder/IResponderApp.hpp>
#include <responder/Permissions.hpp>

extern bool systemHealthNeedsFullRedraw;
extern bool waypointsDirty;
void drawSystemHealthApp();

namespace {

constexpr const char *kSystemHealthPermissions[] = {
    responder::permissions::Display,
};

class SystemHealthResponderApp final : public responder::IResponderApp {
public:
  const responder::AppMetadata &metadata() const override { return _metadata; }

  bool onStart(responder::AppContext &context) override {
    (void)context;
    systemHealthNeedsFullRedraw = true;
    waypointsDirty = true;
    return true;
  }

  void onUpdate(uint32_t nowMs) override {
    (void)nowMs;
  }

  void onRender(responder::DisplayCanvas &canvas) override {
    (void)canvas;
    drawSystemHealthApp();
  }

private:
  responder::AppMetadata _metadata = {
      "com.achindustries.system-health",
      "SYS HEALTH",
      "1.0.0",
      "ACH Industries",
      "Shows memory, Wi-Fi/BLE, USB host, SD card, GPS, and crash status.",
      "Utilities",
      nullptr,
      "builtin:SystemHealthResponderApp",
      RESPONDER_API_VERSION,
      true,
      false,
      false,
      18,
      kSystemHealthPermissions,
      static_cast<uint8_t>(sizeof(kSystemHealthPermissions) / sizeof(kSystemHealthPermissions[0])),
      nullptr,
      0,
      nullptr,
      0,
      nullptr,
      0,
  };
};

SystemHealthResponderApp gSystemHealthResponderApp;

}  // namespace

responder::IResponderApp *systemHealthResponderApp() {
  return &gSystemHealthResponderApp;
}
