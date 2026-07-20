#include <responder/AppRegistry.hpp>
#include <responder/Permissions.hpp>

responder::IResponderApp *systemHealthResponderApp();
responder::IResponderApp *fileTransferResponderApp();
responder::IResponderApp *weatherResponderApp();
responder::IResponderApp *notesResponderApp();
responder::IResponderApp *calendarResponderApp();

namespace {

constexpr const char *kDisplayPermission[] = {responder::permissions::Display};
constexpr const char *kNetworkPermission[] = {responder::permissions::Display,
                                              responder::permissions::NetworkLocal};
constexpr const char *kGpsPermission[] = {responder::permissions::Display,
                                          responder::permissions::GpsRead};
constexpr const char *kUsbPermission[] = {responder::permissions::Display,
                                          responder::permissions::UsbRequest};

responder::AppMetadata legacyMeta(uint8_t index, const char *id, const char *name,
                                  const char *category,
                                  const char *const *permissions = kDisplayPermission,
                                  uint8_t permissionCount = 1) {
  responder::AppMetadata metadata;
  metadata.id = id;
  metadata.name = name;
  metadata.version = "legacy";
  metadata.author = "ACH Industries";
  metadata.description = "Legacy built-in app awaiting modular migration.";
  metadata.category = category;
  metadata.entryPoint = "legacy";
  metadata.minimumApiVersion = RESPONDER_API_VERSION;
  metadata.builtIn = true;
  metadata.supportsBackground = false;
  metadata.legacy = true;
  metadata.legacyIndex = index;
  metadata.permissions = permissions;
  metadata.permissionCount = permissionCount;
  return metadata;
}

}  // namespace

void registerBuiltInResponderApps(responder::AppRegistry &registry) {
  registry.registerLegacy(legacyMeta(0, "com.achindustries.trips", "TRIPS/LOC", "Navigation", kGpsPermission, 2));
  registry.registerBuiltIn(calendarResponderApp());
  registry.registerLegacy(legacyMeta(2, "com.achindustries.camera", "CAMERA", "Media"));
  registry.registerLegacy(legacyMeta(3, "com.achindustries.gallery", "GALLERY", "Media"));
  registry.registerBuiltIn(weatherResponderApp());
  registry.registerLegacy(legacyMeta(5, "com.achindustries.airplanes", "AIRPLANES", "Radio", kUsbPermission, 2));
  registry.registerLegacy(legacyMeta(6, "com.achindustries.ghost", "GHOST", "Radio", kUsbPermission, 2));
  registry.registerLegacy(legacyMeta(7, "com.achindustries.rf-scan", "RF SCAN", "Radio", kUsbPermission, 2));
  registry.registerLegacy(legacyMeta(8, "com.achindustries.recorder", "RECORDER", "Audio"));
  registry.registerLegacy(legacyMeta(9, "com.achindustries.audio", "AUDIO", "Audio"));
  registry.registerLegacy(legacyMeta(10, "com.achindustries.meshcore", "MESHCORE", "Mesh"));
  registry.registerBuiltIn(notesResponderApp());
  registry.registerLegacy(legacyMeta(12, "com.achindustries.obd2", "CAR SCANNER", "Vehicle", kUsbPermission, 2));
  registry.registerLegacy(legacyMeta(13, "com.achindustries.rf-watch", "RF WATCH", "Radio", kNetworkPermission, 2));
  registry.registerLegacy(legacyMeta(15, "com.achindustries.ble-keyboard", "BLE KEYBOARD", "Bluetooth"));
  registry.registerLegacy(legacyMeta(16, "com.achindustries.games", "GAMES", "Games"));
  registry.registerLegacy(legacyMeta(17, "com.achindustries.deploy-cam", "DEPLOY CAM", "Camera", kNetworkPermission, 2));

  registry.registerBuiltIn(systemHealthResponderApp());
  registry.registerBuiltIn(fileTransferResponderApp());
}
