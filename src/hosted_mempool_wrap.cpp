#include <Arduino.h>
#include "esp_heap_caps.h"
#include "esp_hosted_os_abstraction.h"

extern "C" void *__real_hosted_mempool_alloc(void *pool, uint32_t nbytes, uint8_t need_memset);
extern "C" void __real_hosted_mempool_free(void *pool, void *buf);

namespace {
constexpr uint32_t kHostedFallbackMagic = 0x54424642;  // "TBFB"

struct alignas(16) HostedFallbackHeader {
  uint32_t magic;
  uint32_t size;
  void *pool;
  uint32_t reserved;
};

static uint32_t fallbackAllocCount = 0;
static uint32_t fallbackFreeCount = 0;
static uint32_t fallbackFailCount = 0;
static void *(*realHostedMallocAlign)(size_t size, size_t align) = nullptr;
static void (*realHostedFreeAlign)(void *ptr) = nullptr;
static void (*realHostedFree)(void *ptr) = nullptr;
static uint32_t osiFallbackAllocCount = 0;
static uint32_t osiFallbackFreeCount = 0;
static uint32_t osiFallbackFailCount = 0;

static void *tab5HostedAllocFallback(size_t size, size_t align, uint32_t &allocCount,
                                     uint32_t &failCount, const char *tag) {
  if (align < 4) align = 4;
  const size_t total = sizeof(HostedFallbackHeader) + size + align;
  void *raw = heap_caps_malloc(total, MALLOC_CAP_INTERNAL | MALLOC_CAP_DMA | MALLOC_CAP_8BIT);
  if (!raw) {
    raw = heap_caps_malloc(total, MALLOC_CAP_INTERNAL | MALLOC_CAP_8BIT);
  }
  if (!raw) {
    failCount++;
    if ((failCount <= 4) || ((failCount & 0x0F) == 0)) {
      log_e("%s: fallback alloc failed n=%u align=%u internal=%u dma=%u largest=%u fails=%u",
            tag, static_cast<unsigned>(size), static_cast<unsigned>(align),
            static_cast<unsigned>(heap_caps_get_free_size(MALLOC_CAP_INTERNAL | MALLOC_CAP_8BIT)),
            static_cast<unsigned>(heap_caps_get_free_size(MALLOC_CAP_INTERNAL | MALLOC_CAP_DMA)),
            static_cast<unsigned>(heap_caps_get_largest_free_block(MALLOC_CAP_INTERNAL | MALLOC_CAP_8BIT)),
            static_cast<unsigned>(failCount));
    }
    return nullptr;
  }
  uintptr_t start = reinterpret_cast<uintptr_t>(raw) + sizeof(HostedFallbackHeader);
  uintptr_t aligned = (start + align - 1) & ~(static_cast<uintptr_t>(align) - 1);
  auto *hdr = reinterpret_cast<HostedFallbackHeader *>(aligned) - 1;
  hdr->magic = kHostedFallbackMagic;
  hdr->size = size;
  hdr->pool = raw;
  hdr->reserved = 0;
  allocCount++;
  if ((allocCount <= 8) || ((allocCount & 0x1F) == 0)) {
    log_w("%s: fallback alloc n=%u align=%u count=%u internal=%u dma=%u",
          tag, static_cast<unsigned>(size), static_cast<unsigned>(align),
          static_cast<unsigned>(allocCount),
          static_cast<unsigned>(heap_caps_get_free_size(MALLOC_CAP_INTERNAL | MALLOC_CAP_8BIT)),
          static_cast<unsigned>(heap_caps_get_free_size(MALLOC_CAP_INTERNAL | MALLOC_CAP_DMA)));
  }
  return reinterpret_cast<void *>(aligned);
}

static bool tab5HostedFreeFallback(void *ptr, uint32_t &freeCount, const char *tag) {
  if (!ptr) return true;
  auto *hdr = reinterpret_cast<HostedFallbackHeader *>(ptr) - 1;
  if (hdr->magic != kHostedFallbackMagic) return false;
  void *raw = hdr->pool;
  hdr->magic = 0;
  free(raw);
  freeCount++;
  if ((freeCount <= 8) || ((freeCount & 0x1F) == 0)) {
    log_w("%s: fallback free count=%u", tag, static_cast<unsigned>(freeCount));
  }
  return true;
}

static void *tab5HostedMallocAlign(size_t size, size_t align) {
  void *ptr = realHostedMallocAlign ? realHostedMallocAlign(size, align) : nullptr;
  if (ptr) return ptr;
  return tab5HostedAllocFallback(size, align, osiFallbackAllocCount, osiFallbackFailCount, "HOSTEDOSI");
}

static void tab5HostedFreeAlign(void *ptr) {
  if (tab5HostedFreeFallback(ptr, osiFallbackFreeCount, "HOSTEDOSI")) return;
  if (realHostedFreeAlign) {
    realHostedFreeAlign(ptr);
  } else {
    free(ptr);
  }
}

static void tab5HostedFree(void *ptr) {
  if (tab5HostedFreeFallback(ptr, osiFallbackFreeCount, "HOSTEDOSI")) return;
  if (realHostedFree) {
    realHostedFree(ptr);
  } else {
    free(ptr);
  }
}
}

extern "C" void *__wrap_hosted_mempool_alloc(void *pool, uint32_t nbytes, uint8_t need_memset) {
  void *buf = __real_hosted_mempool_alloc(pool, nbytes, need_memset);
  if (buf) return buf;

  void *payload = tab5HostedAllocFallback(nbytes, 16, fallbackAllocCount, fallbackFailCount, "HOSTEDMP");
  if (!payload) return nullptr;
  if (need_memset) memset(payload, 0, nbytes);
  (void)pool;
  return payload;
}

extern "C" void __wrap_hosted_mempool_free(void *pool, void *buf) {
  if (!buf) return;
  if (tab5HostedFreeFallback(buf, fallbackFreeCount, "HOSTEDMP")) {
    (void)pool;
    return;
  }
  __real_hosted_mempool_free(pool, buf);
}

extern "C" void tab5_patch_hosted_osi_allocators(void) {
  if (!g_h.funcs) return;
  if (g_h.funcs->_h_malloc_align == tab5HostedMallocAlign) return;
  realHostedMallocAlign = g_h.funcs->_h_malloc_align;
  realHostedFreeAlign = g_h.funcs->_h_free_align;
  realHostedFree = g_h.funcs->_h_free;
  g_h.funcs->_h_malloc_align = tab5HostedMallocAlign;
  g_h.funcs->_h_free_align = tab5HostedFreeAlign;
  g_h.funcs->_h_free = tab5HostedFree;
  log_w("HOSTEDOSI: patched aligned allocator real=%p free=%p freeRaw=%p",
        realHostedMallocAlign, realHostedFreeAlign, realHostedFree);
}
