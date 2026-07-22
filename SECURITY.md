# Security policy

Responder Nav is an actively developed field-terminal project. File Transfer
and Deploy Cam are intended for trusted local networks and must not be exposed
directly to the Internet.

File Transfer generates a new 64-bit random access token whenever the app opens.
The token is shown only on the Tab5, is accepted once through the protected URL,
and is then held in an HttpOnly, SameSite=Strict session cookie. Requests without
the token or cookie are rejected. Traffic is still plain HTTP, so another device
with network-capture access could observe it.

## Reporting a vulnerability

Do not publish credentials, API keys, precise location history, recordings, or
a working exploit in a public issue. Contact the repository owner privately
through the security-reporting method configured on the GitHub repository.

Include the affected firmware version, hardware, reproduction steps, impact,
and any proposed mitigation. Remove unrelated personal data from logs.

## Operational guidance

- Do not expose File Transfer or Deploy Cam ports directly to the Internet.
- Use the services only on a trusted LAN or isolated camera access point.
- Close File Transfer when the transfer is complete; this stops the server and invalidates the session token.
- Remove secrets before sharing SD-card images or serial logs.
- Install optional app packages only from trusted sources.
- API keys entered in Utilities are masked and excluded from logs, but the standard public build does not enable flash encryption. Treat physical access to the device as access to locally stored credentials.

## Public-release secret controls

- No Wi-Fi password, API key, SSID, user directory, fixed COM port, private key, serial log, crash dump, or development font is included in the release tree.
- Map and location request URLs are redacted from serial diagnostics.
- Connected and saved Wi-Fi network names, waypoint names, and precise coordinates are excluded from public-build diagnostics.
- The Deploy Cam access point uses the documented `deploycam` setup password in v0.71 Beta. Treat it as an isolated setup network, do not expose its services to the Internet, and avoid using it for sensitive scenes.
- The sample `tile_key.example.txt` is a placeholder only.
- Release binaries are published with SHA-256 checksums.
