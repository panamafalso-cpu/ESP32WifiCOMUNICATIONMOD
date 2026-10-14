# Standalone ESP32-C5 Wi-Fi 6 AP + web page

This is a separate, minimal ESP-IDF project. It does not build or link the parent repository's firmware.

## What it does
- Starts a WPA2-protected 2.4 GHz access point configured for 802.11b/g/n/ax.
- Serves a small status page at `http://192.168.4.1/`.
- Contains no packet sniffer, raw 802.11 frame injection, deauthentication, phishing, or credential-collection code.

The 5 GHz protocol flags are configured for the C5 driver, but this example pins the AP to channel 1 and therefore operates on 2.4 GHz. It does not claim a 5 GHz AP.

## Build
Requires ESP-IDF v5.5.2 or newer with ESP32-C5 support:

```sh
cd safe-c5-ap-web
idf.py set-target esp32c5
idf.py build
```

GitHub Actions builds the app and publishes the build output as an artifact.

## Flashing
Use the generated `firmware-merged.bin` with the ESP-IDF merged-image workflow or flash the individual bootloader, partition table, and app binaries using the offsets printed by `idf.py build`. Verify the exact board flash size and offsets before writing.

## Test credentials
SSID: `C5-WiFi6-Lab`
Password: `c5labwifi6`

Change the development password before using this AP outside a controlled test environment.
