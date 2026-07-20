#include <Arduino.h>
#include "esp_hosted_transport_config.h"

extern "C" esp_hosted_sdio_config __real_esp_hosted_get_default_sdio_config(void);

enum HostedTransportProfile : uint8_t {
  HOSTED_TRANSPORT_WIFI = 0,
  HOSTED_TRANSPORT_BLE = 1,
};

static volatile uint8_t hostedTransportProfile = HOSTED_TRANSPORT_WIFI;

extern "C" void tab5_hosted_set_wifi_profile(void) {
  hostedTransportProfile = HOSTED_TRANSPORT_WIFI;
}

extern "C" void tab5_hosted_set_ble_profile(void) {
  hostedTransportProfile = HOSTED_TRANSPORT_BLE;
}

// Wi-Fi and BLE stress different parts of ESP-Hosted. RX streaming trips
// sdio_rx_get_buffer asserts on this P4+C6 build, so all profiles force
// RX_NONE. Wi-Fi keeps a small queue to preserve heap. BLE HCI needs more
// credits, so isolated BLE boots use a larger TX queue while keeping RX at 4;
// raising RX to 20 still trips sdio_rx_get_buffer asserts.
extern "C" esp_hosted_sdio_config __wrap_esp_hosted_get_default_sdio_config(void) {
  esp_hosted_sdio_config config = __real_esp_hosted_get_default_sdio_config();
  if (hostedTransportProfile == HOSTED_TRANSPORT_BLE) {
    config.tx_queue_size = 20;
    config.rx_queue_size = 4;
    config.rx_mode = 3;  // H_SDIO_OPTIMIZATION_RX_NONE
    return config;
  }
  config.tx_queue_size = 1;
  config.rx_queue_size = 1;
  config.rx_mode = 3;  // H_SDIO_OPTIMIZATION_RX_NONE
  log_w("HOSTEDCFG: wifi txq=%u rxq=%u rxmode=%u",
        static_cast<unsigned>(config.tx_queue_size),
        static_cast<unsigned>(config.rx_queue_size),
        static_cast<unsigned>(config.rx_mode));
  return config;
}
