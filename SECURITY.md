# Security policy

Responder Nav is an actively developed field-terminal project. File Transfer
and Deploy Cam are intended for trusted local networks and must not be exposed
directly to the Internet.

## Security scope

The public firmware deliberately does not enable Secure Boot, flash encryption,
or burn security eFuses. This keeps installation, recovery, and reflashing
reversible and avoids device-specific provisioning that can permanently lock a
board. As a result, physical possession of a device or its SD card must be
treated as access to locally stored data and credentials. The project does not
claim resistance to a determined physical attacker.

Weather, the default map-tile service, and Wi-Fi aircraft requests authenticate
their HTTPS servers against the bundled ISRG Root X1 certificate. Certificate
validation is not disabled. The legacy Wi-Fi geolocation lookup remains plain
HTTP in v0.73 Beta to preserve the currently validated provider behavior; its
coarse location result can be observed or altered by a hostile network and must
not be treated as trusted security data.

File Transfer generates a new 128-bit one-time bootstrap token whenever the app
opens. The token is shown only on the Tab5 and is exchanged once for a separate
128-bit session token held in an HttpOnly, SameSite=Strict cookie. The bootstrap
token is then invalidated. Requests require the exact session cookie and the
expected local Host header; state-changing operations use POST. Browser responses
also disable framing and referrer forwarding. Request lines and headers have
strict size, count, and time limits, and leaving the app closes its listening
socket and invalidates both tokens. Traffic is still plain HTTP, so another
device with network-capture access could observe an active transfer.

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
- Use a trusted Wi-Fi network. File Transfer and legacy Wi-Fi geolocation are not end-to-end encrypted in v0.73 Beta.

## Public-release secret controls

- No Wi-Fi password, API key, SSID, user directory, fixed COM port, private key, serial log, crash dump, or development font is included in the release tree.
- The designer name `Alberto Cajiao Hernandez`, organization `ACH Industries`, and public GitHub account `fistulareffigy` are intentional public attribution, not runtime user data.
- Map and location request URLs are redacted from serial diagnostics.
- Connected and saved Wi-Fi network names, waypoint names, and precise coordinates are excluded from public-build diagnostics.
- The Deploy Cam access point uses the documented `deploycam` setup password in v0.73 Beta. It is intentionally not presented as a security boundary. Treat it as an isolated setup network, do not expose its services to the Internet, and avoid using it for sensitive scenes.
- Deploy Cam v0.73 Beta does not yet authenticate its LAN stream and control endpoints. Do not join it to an untrusted or shared network. A paired-token protocol is required before Deploy Cam can be described as secure for general LAN use.
- The sample `tile_key.example.txt` is a placeholder only.
- Release binaries are published with SHA-256 checksums.
