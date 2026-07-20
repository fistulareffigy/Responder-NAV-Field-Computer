#include <Arduino.h>
#include <M5Unified.h>
#include <stdlib.h>
extern "C" {
#include "esp_sccb_i2c.h"
#include "esp_sccb_io_interface.h"
}

struct sccb_m5_i2c_io_t {
  esp_sccb_io_t base;
  m5::I2C_Class *i2c;
  uint8_t i2c_port;
  uint16_t device_address;
  uint32_t freq;
};

static m5::I2C_Class *sccb_pick_i2c(uint8_t port) {
  auto *in = &m5::In_I2C;
  auto *ex = &m5::Ex_I2C;
  if (ex->isEnabled() && port == ex->getPort()) {
    return ex;
  }
  if (in->isEnabled()) {
    return in;
  }
  if (ex->isEnabled()) {
    return ex;
  }
  return in;
}

static esp_err_t sccb_write(esp_sccb_io_t *io_handle, const uint8_t *write_buffer, size_t write_size,
                            int xfer_timeout_ms) {
  if (!io_handle || !write_buffer || write_size == 0) {
    return ESP_ERR_INVALID_ARG;
  }
  auto *io = reinterpret_cast<sccb_m5_i2c_io_t *>(io_handle);
  if (!io->i2c) {
    return ESP_ERR_INVALID_STATE;
  }
  (void)xfer_timeout_ms;
  bool ok = io->i2c->start(io->device_address, false, io->freq);
  if (ok) {
    ok = io->i2c->write(write_buffer, write_size);
  }
  if (!io->i2c->stop()) {
    ok = false;
  }
  return ok ? ESP_OK : ESP_FAIL;
}

static esp_err_t sccb_write_read(esp_sccb_io_t *io_handle, const uint8_t *write_buffer, size_t write_size,
                                 uint8_t *read_buffer, size_t read_size, int xfer_timeout_ms) {
  if (!io_handle || !write_buffer || write_size == 0 || !read_buffer || read_size == 0) {
    return ESP_ERR_INVALID_ARG;
  }
  auto *io = reinterpret_cast<sccb_m5_i2c_io_t *>(io_handle);
  if (!io->i2c) {
    return ESP_ERR_INVALID_STATE;
  }
  (void)xfer_timeout_ms;
  bool ok = io->i2c->start(io->device_address, false, io->freq);
  if (ok) {
    ok = io->i2c->write(write_buffer, write_size);
  }
  if (ok) {
    ok = io->i2c->restart(io->device_address, true, io->freq);
  }
  if (ok) {
    ok = io->i2c->read(read_buffer, read_size, true);
  }
  if (!io->i2c->stop()) {
    ok = false;
  }
  return ok ? ESP_OK : ESP_FAIL;
}

static esp_err_t sccb_delete(esp_sccb_io_t *io_handle) {
  if (!io_handle) {
    return ESP_ERR_INVALID_ARG;
  }
  free(io_handle);
  return ESP_OK;
}

extern "C" esp_err_t sccb_new_i2c_io(i2c_master_bus_handle_t bus_handle, const sccb_i2c_config_t *config,
                                     esp_sccb_io_handle_t *io_handle) {
  if (!config || !io_handle) {
    return ESP_ERR_INVALID_ARG;
  }
  auto *io = reinterpret_cast<sccb_m5_i2c_io_t *>(calloc(1, sizeof(sccb_m5_i2c_io_t)));
  if (!io) {
    return ESP_ERR_NO_MEM;
  }
  uint8_t port = bus_handle ? static_cast<uint8_t>(reinterpret_cast<uintptr_t>(bus_handle)) : 1;
  io->i2c_port = port;
  io->i2c = sccb_pick_i2c(port);
  io->device_address = config->device_address;
  io->freq = config->scl_speed_hz ? config->scl_speed_hz : 400000;

  io->base.transmit_reg_a8v8 = sccb_write;
  io->base.transmit_reg_a16v8 = sccb_write;
  io->base.transmit_reg_a8v16 = sccb_write;
  io->base.transmit_reg_a16v16 = sccb_write;

  io->base.transmit_receive_reg_a8v8 = sccb_write_read;
  io->base.transmit_receive_reg_a16v8 = sccb_write_read;
  io->base.transmit_receive_reg_a8v16 = sccb_write_read;
  io->base.transmit_receive_reg_a16v16 = sccb_write_read;
  io->base.del = sccb_delete;

  *io_handle = &io->base;
  return ESP_OK;
}
