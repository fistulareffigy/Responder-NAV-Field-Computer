# Hello Responder package

This is a minimal SD app package used to verify external package discovery.

It intentionally contains only metadata. The current firmware scans and validates `app.json` manifests from `/apps`, then reports the discovered package count in System Health and serial logs:

```text
APPREG: SD package id=com.achindustries.example.hello_responder ...
APPREG: SD app scan reason=boot root=/apps packages=1
```

Executable SD app loading is planned separately from the current native app registry.
