# ESPHome PCM60X MPPT Battery Charge Controller Integration

This is a custom ESPHome component for MPPT-3000 / PCM60X-style battery charge controllers with RS232 communication.

## Features
- Reads MPPT status over UART (`QPIGS`, `QPIRI`, etc.)
- Supports integration with Home Assistant via ESPHome
- Works with many Chinese-branded MPPTs using the "Standard Protocol"

## Usage

In your ESPHome YAML config:

```yaml
external_components:
  - source:
      type: git
      url: https://github.com/janisbeikm/esphome-pcm60x
      ref: main
    components: [pcm60x]

uart:
  tx_pin: GPIO17
  rx_pin: GPIO16
  baud_rate: 2400

pcm60x:
