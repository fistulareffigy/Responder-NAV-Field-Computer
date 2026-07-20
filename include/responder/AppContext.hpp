#pragma once

#include <Arduino.h>

#include "Services.hpp"

namespace responder {

class ILogger {
public:
  virtual ~ILogger() = default;
  virtual void info(const char *tag, const char *message) = 0;
  virtual void warn(const char *tag, const char *message) = 0;
  virtual void error(const char *tag, const char *message) = 0;
};

class AppContext {
public:
  AppContext() = default;
  explicit AppContext(ILogger *logger) : _logger(logger) {}

  ILogger *logger() const { return _logger; }
  void setLogger(ILogger *logger) { _logger = logger; }
  AppServices &services() { return _services; }
  const AppServices &services() const { return _services; }
  void setServices(const AppServices &services) { _services = services; }

private:
  ILogger *_logger = nullptr;
  AppServices _services;
};

class DisplayCanvas {
public:
  DisplayCanvas(int16_t width = 0, int16_t height = 0) : _width(width), _height(height) {}

  int16_t width() const { return _width; }
  int16_t height() const { return _height; }
  void setSize(int16_t width, int16_t height) {
    _width = width;
    _height = height;
  }

private:
  int16_t _width = 0;
  int16_t _height = 0;
};

}  // namespace responder
