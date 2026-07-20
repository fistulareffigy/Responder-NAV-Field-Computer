#include <responder/IResponderApp.hpp>
#include <responder/Permissions.hpp>

enum NotesView : uint8_t;
extern NotesView notesView;
extern bool waypointsDirty;

void drawNotesApp();
void handleNotesAppTouch(int16_t x, int16_t y);
bool refreshNoteFileList();

namespace {

constexpr const char *kNotesPermissions[] = {
    responder::permissions::Display,
    responder::permissions::Input,
    responder::permissions::StorageRead,
    responder::permissions::StorageWrite,
};

class NotesResponderApp final : public responder::IResponderApp {
public:
  const responder::AppMetadata &metadata() const override { return _metadata; }

  bool onStart(responder::AppContext &context) override {
    (void)context;
    notesView = static_cast<NotesView>(0);
    refreshNoteFileList();
    waypointsDirty = true;
    return true;
  }

  void onRender(responder::DisplayCanvas &canvas) override {
    (void)canvas;
    drawNotesApp();
  }

  bool onInput(const responder::InputEvent &event) override {
    if (event.type != responder::InputEventType::TouchDown) {
      return false;
    }
    handleNotesAppTouch(event.x, event.y);
    return true;
  }

private:
  responder::AppMetadata _metadata = {
      "com.achindustries.notes",
      "NOTES",
      "0.1.0",
      "ACH Industries",
      "SD-card notes file browser and editor.",
      "Utilities",
      nullptr,
      "notes",
      RESPONDER_API_VERSION,
      true,
      false,
      false,
      11,
      kNotesPermissions,
      static_cast<uint8_t>(sizeof(kNotesPermissions) / sizeof(kNotesPermissions[0])),
      nullptr,
      0,
      nullptr,
      0,
      nullptr,
      0,
  };
};

NotesResponderApp notesApp;

}  // namespace

responder::IResponderApp *notesResponderApp() {
  return &notesApp;
}
