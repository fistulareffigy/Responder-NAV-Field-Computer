#include <time.h>

#include <responder/IResponderApp.hpp>
#include <responder/Permissions.hpp>

extern bool waypointsDirty;
extern int calendarDisplayYear;
extern int calendarDisplayMonth;
extern int calendarSelectedYear;
extern int calendarSelectedMonth;
extern int calendarSelectedDay;

void drawAppTopBar(const char *title);
void drawCalendarApp();
void drawNavBar();
void drawBusyIndicatorTopCenter(uint8_t textSize);
void handleCalendarAppTouch(int16_t x, int16_t y);
bool loadCalendarEvents();
bool getLocalTimeTm(tm &out);

namespace {

constexpr const char *kCalendarPermissions[] = {
    responder::permissions::Display,
    responder::permissions::Input,
    responder::permissions::StorageRead,
    responder::permissions::StorageWrite,
};

class CalendarResponderApp final : public responder::IResponderApp {
public:
  const responder::AppMetadata &metadata() const override { return _metadata; }

  bool onStart(responder::AppContext &context) override {
    (void)context;
    loadCalendarEvents();
    tm nowTm;
    if (getLocalTimeTm(nowTm)) {
      calendarDisplayYear = nowTm.tm_year + 1900;
      calendarDisplayMonth = nowTm.tm_mon + 1;
      calendarSelectedYear = calendarDisplayYear;
      calendarSelectedMonth = calendarDisplayMonth;
      calendarSelectedDay = nowTm.tm_mday;
    }
    waypointsDirty = true;
    return true;
  }

  void onRender(responder::DisplayCanvas &canvas) override {
    (void)canvas;
    drawAppTopBar("CALENDAR");
    drawCalendarApp();
    drawNavBar();
    drawBusyIndicatorTopCenter(1);
  }

  bool onInput(const responder::InputEvent &event) override {
    if (event.type != responder::InputEventType::TouchDown) {
      return false;
    }
    handleCalendarAppTouch(event.x, event.y);
    return true;
  }

private:
  responder::AppMetadata _metadata = {
      "com.achindustries.calendar",
      "CALENDAR",
      "0.1.0",
      "ACH Industries",
      "Local calendar and reminder app.",
      "Utilities",
      nullptr,
      "calendar",
      RESPONDER_API_VERSION,
      true,
      false,
      false,
      1,
      kCalendarPermissions,
      static_cast<uint8_t>(sizeof(kCalendarPermissions) / sizeof(kCalendarPermissions[0])),
      nullptr,
      0,
      nullptr,
      0,
      nullptr,
      0,
  };
};

CalendarResponderApp calendarApp;

}  // namespace

responder::IResponderApp *calendarResponderApp() {
  return &calendarApp;
}
