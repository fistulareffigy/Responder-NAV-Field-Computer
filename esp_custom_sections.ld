SECTIONS
{
  .esp_app_custom_desc : ALIGN(4)
  {
    __esp_cam_sensor_detect_fn_array_start = ABSOLUTE(.);
    KEEP(*(.esp_cam_sensor_detect_fn))
    __esp_cam_sensor_detect_fn_array_end = ABSOLUTE(.);

    __esp_ipa_detect_array_start = ABSOLUTE(.);
    KEEP(*(.esp_ipa_detect))
    __esp_ipa_detect_array_end = ABSOLUTE(.);
  }
}
INSERT AFTER .rodata;
