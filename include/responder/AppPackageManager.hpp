#pragma once

#include <Arduino.h>
#include <SdFat.h>

#include "AppPackage.hpp"

namespace responder {

void clearExternalAppManifests(ExternalAppManifest *manifests, uint8_t maxCount);

bool loadExternalAppManifest(SdFs &sd, const String &packagePath, const String &manifestPath,
                             ExternalAppManifest &out);

}  // namespace responder
