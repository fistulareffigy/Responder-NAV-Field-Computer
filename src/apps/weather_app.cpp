#include <responder/IResponderApp.hpp>
#include <responder/Permissions.hpp>

void drawAppTopBar(const char *title);
void drawWeatherApp();
void drawNavBar();
void drawBusyIndicatorTopCenter(uint8_t textSize);

namespace {

constexpr const char *kWeatherPermissions[] = {
    responder::permissions::Display,
    responder::permissions::NetworkLocal,
    responder::permissions::GpsRead,
};

class WeatherResponderApp final : public responder::IResponderApp {
public:
  const responder::AppMetadata &metadata() const override { return _metadata; }

  void onRender(responder::DisplayCanvas &canvas) override {
    (void)canvas;
    drawAppTopBar("WEATHER");
    drawWeatherApp();
    drawNavBar();
    drawBusyIndicatorTopCenter(1);
  }

private:
  responder::AppMetadata _metadata = {
      "com.achindustries.weather",
      "WEATHER",
      "0.1.0",
      "ACH Industries",
      "Weather status using Wi-Fi geolocation, GPS location, and Open-Meteo.",
      "Network",
      nullptr,
      "weather",
      RESPONDER_API_VERSION,
      true,
      false,
      false,
      4,
      kWeatherPermissions,
      static_cast<uint8_t>(sizeof(kWeatherPermissions) / sizeof(kWeatherPermissions[0])),
      nullptr,
      0,
      nullptr,
      0,
      nullptr,
      0,
  };
};

WeatherResponderApp weatherApp;

}  // namespace

responder::IResponderApp *weatherResponderApp() {
  return &weatherApp;
}
