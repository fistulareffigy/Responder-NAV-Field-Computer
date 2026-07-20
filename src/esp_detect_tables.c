#include "esp_cam_sensor_detect.h"
#include "esp_ipa_detect.h"
#include "sc202cs.h"

esp_ipa_t *__esp_ipa_detect_fn_awb_gray_world(void *config);
esp_ipa_t *__esp_ipa_detect_fn_agc_threshold(void *config);
esp_ipa_t *__esp_ipa_detect_fn_denoising_gain_feedback(void *config);
esp_ipa_t *__esp_ipa_detect_fn_sharpen_freq_feedback(void *config);
esp_ipa_t *__esp_ipa_detect_fn_gamma_lumma_feedback(void *config);
esp_ipa_t *__esp_ipa_detect_fn_cc_linear(void *config);

#define DETECT_TABLE_ATTR(section_name) \
    __attribute__((used, no_reorder, section(section_name), aligned(4)))

DETECT_TABLE_ATTR(".esp_cam_sensor_detect_fn")
esp_cam_sensor_detect_fn_t __esp_cam_sensor_detect_fn_array_start = {
    (esp_cam_sensor_device_t * (*)(void *))sc202cs_detect,
    ESP_CAM_SENSOR_MIPI_CSI,
    SC202CS_SCCB_ADDR,
};

DETECT_TABLE_ATTR(".esp_cam_sensor_detect_fn")
esp_cam_sensor_detect_fn_t __esp_cam_sensor_detect_fn_array_end = {0};

DETECT_TABLE_ATTR(".esp_ipa_detect")
esp_ipa_detect_t __esp_ipa_detect_array_start = {"awb.gray", __esp_ipa_detect_fn_awb_gray_world};

DETECT_TABLE_ATTR(".esp_ipa_detect")
static esp_ipa_detect_t esp_ipa_detect_agc_threshold = {"agc.threshold", __esp_ipa_detect_fn_agc_threshold};

DETECT_TABLE_ATTR(".esp_ipa_detect")
static esp_ipa_detect_t esp_ipa_detect_denoising_gain_feedback = {"denoising.gf",
                                                                  __esp_ipa_detect_fn_denoising_gain_feedback};

DETECT_TABLE_ATTR(".esp_ipa_detect")
static esp_ipa_detect_t esp_ipa_detect_sharpen_freq_feedback = {"sharpen.ff",
                                                                __esp_ipa_detect_fn_sharpen_freq_feedback};

DETECT_TABLE_ATTR(".esp_ipa_detect")
static esp_ipa_detect_t esp_ipa_detect_gamma_lumma_feedback = {"gamma.lf", __esp_ipa_detect_fn_gamma_lumma_feedback};

DETECT_TABLE_ATTR(".esp_ipa_detect")
static esp_ipa_detect_t esp_ipa_detect_cc_linear = {"cc.linear", __esp_ipa_detect_fn_cc_linear};

DETECT_TABLE_ATTR(".esp_ipa_detect")
esp_ipa_detect_t __esp_ipa_detect_array_end = {0};
