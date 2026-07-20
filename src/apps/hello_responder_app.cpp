#include <M5Unified.h>

#include <responder/IResponderApp.hpp>
#include <responder/Permissions.hpp>

extern int16_t screenW;
extern int16_t screenH;
extern int16_t navH;
void drawAppTopBar(const char *title);
void drawNavBar();

namespace {

constexpr const char *kHelloPermissions[] = {
    responder::permissions::Display,
    responder::permissions::Input,
};

class HelloResponderApp final : public responder::IResponderApp {
public:
  const responder::AppMetadata &metadata() const override { return _metadata; }

  bool onStart(responder::AppContext &context) override {
    if (context.logger()) {
      context.logger()->info("APP", "Hello Responder start");
    }
    _counter = 0;
    return true;
  }

  void onUpdate(uint32_t nowMs) override {
    if (nowMs - _lastTickMs >= 1000) {
      _lastTickMs = nowMs;
      ++_counter;
    }
  }

  void onRender(responder::DisplayCanvas &canvas) override {
    (void)canvas;
    const uint16_t black = M5.Display.color565(6, 6, 6);
    const uint16_t amber = M5.Display.color565(255, 176, 0);
    const uint16_t amberDim = M5.Display.color565(160, 96, 0);

    drawAppTopBar("HELLO RESPONDER");
    int16_t top = 58;
    int16_t bottom = static_cast<int16_t>(screenH - navH - 8);
    M5.Display.fillRect(0, top, screenW, static_cast<int16_t>(bottom - top), black);
    M5.Display.setTextDatum(TL_DATUM);
    M5.Display.setTextSize(2);
    M5.Display.setTextColor(amber, black);
    M5.Display.drawString("Hello Responder", 18, static_cast<int16_t>(top + 12));
    M5.Display.setTextSize(1);
    M5.Display.setTextColor(amberDim, black);
    M5.Display.drawString("This is the first SDK-style sample app.", 18,
                          static_cast<int16_t>(top + 52));
    M5.Display.drawString("It is registered outside main.cpp.", 18,
                          static_cast<int16_t>(top + 76));
    M5.Display.drawString(String("Runtime ticks: ") + String(_counter), 18,
                          static_cast<int16_t>(top + 112));
    M5.Display.drawRect(14, static_cast<int16_t>(top + 8),
                        static_cast<int16_t>(screenW - 28),
                        static_cast<int16_t>(bottom - top - 16), amberDim);
    drawNavBar();
  }

private:
  uint32_t _lastTickMs = 0;
  uint32_t _counter = 0;
  responder::AppMetadata _metadata = {
      "com.responder.example.hello",
      "HELLO",
      "1.0.0",
      "Responder SDK",
      "Minimal sample app for built-in app developers.",
      "Developer",
      nullptr,
      "builtin:HelloResponderApp",
      RESPONDER_API_VERSION,
      true,
      false,
      false,
      19,
      kHelloPermissions,
      static_cast<uint8_t>(sizeof(kHelloPermissions) / sizeof(kHelloPermissions[0])),
      nullptr,
      0,
      nullptr,
      0,
      nullptr,
      0,
  };
};

HelloResponderApp gHelloResponderApp;

}  // namespace

responder::IResponderApp *helloResponderApp() {
  return &gHelloResponderApp;
}
