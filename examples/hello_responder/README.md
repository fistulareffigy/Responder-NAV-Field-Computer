# Hello Responder sample

This folder is the first placeholder for third-party app packaging.

The firmware does not load external packages yet. For now, this sample mirrors
the built-in `src/apps/hello_responder_app.cpp` app and documents the metadata
shape future SD-card apps should provide.

When external loading is implemented, this package should live at:

```text
/apps/com.responder.example.hello/app.json
```

Next phases should add:

- Package discovery.
- Signature/checksum validation.
- Permission review UI.
- Runtime service handles.

Built-in apps can already receive service handles through `AppContext::services()`.
External apps should use the same service model when loading is enabled.
