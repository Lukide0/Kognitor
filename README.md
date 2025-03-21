# Kognitor

A sensor monitoring application.

## Features

- Easy sensor management
- Measurement at a configurable interval (1s - 99h)
- Export/import configuration
- Configuration options:
  - Enable/disable sensors
  - Set measurement intervals
  - Configure sensor watchers
- Read individual sensor measurements

## Requirements

- cmake 3.23 or newer
- avrdude
- avr-gcc with c++20 support

## Build

```
mkdir build
cd build
cmake -DCMAKE_BUILD_TYPE=Release ..
cmake --build .
```

### Upload

For easier firmware upload, use the `tools/upload.py` script.

#### Arguments:

- `--mcu` Microcontroller model (e.g. `atmega328p`)
- `--port` Serial port (e.g. `COM3`, `/dev/ttyUSB0`)
- `--baud` Baud rate
- `--programmer` Programmer (e.g. `arduino`)
- `--file`, `-f` Hex file to upload

#### Example

`tools/upload.py --mcu m328p --port /dev/ttyACM0 --programmer arduino --file build/kognitor-atmega328p.hex`

## Communication

Communication is handled via USART.

### Legend

| Symbol | Meaning                                                |
| ------ | ------------------------------------------------------ |
| `X`    | ASCII digit                                            |
| `D`    | Time (`S`=`s`=seconds, `M`=`m`=minutes, `H`=`h`=hours) |

| Command            | Description                                                 | Format           | Output            |
| ------------------ | ----------------------------------------------------------- | ---------------- | ----------------- |
| Enable sensor      | Enables the sensor and starts measuring                     | `eXXX`           | `OK` or `EX`      |
| Disable sensor     | Disables the sensor and stops measuring                     | `dXXX`           | `OK` or `EX`      |
| Set interval       | Sets the measurement interval for all sensors               | `sXXD`           | `OK` or `EX`      |
| Set sensor watch   | The sensor triggers a watch event after each measurement    | `wXXX`           | `OK` or `EX`      |
| Clear sensor watch | Disables the watch event for the sensor                     | `cXXX`           | `OK` or `EX`      |
| List sensor state  | Sends the state of all sensors (enabled/disabled, watching) | `l`              | See below         |
| Sensor read        | Reads the last measurement from the sensor                  | `rXXX`           | depends on sensor |
| Sensor read all    | Reads all measurements from the sensor                      | `RXXX`           | depends on sensor |
| Export config      | Exports the sensor configuration (enable, watch, interval)  | `E`              | See below         |
| Import config      | Imports a sensor configuration                              | `I...` See Below | `OK` or `EX`      |

### Error codes

| Code | Name                   | Description                                                             |
| ---- | ---------------------- | ----------------------------------------------------------------------- |
| 0    | Invalid sensor number  | The sensor number is out of range or the message is in the wrong format |
| 1    | Invalid interval       | The inteval message is in the wrong format                              |
| 2    | Invalid interval value | The interval value is in the wrong format                               |
| 3    | Invalid interval unit  | The interval unit is in the wrong format                                |
| 4    | Invalid config         | The config message is in the wrong format                               |
| 5    | Config checksum failed | The config checksum does not match                                      |
| 6    | Unknown command        | The command is not recognized                                           |

### Sensor State Format

The `l` (list) command returns sensor states in the following format:

1. **First byte**: Number of sensors
2. **Second byte**: Number of chunks per sensor state
3. **Enable state bytes**: Each bit represents a sensor (0 = disabled, 1 = enabled)
4. **Watch state bytes**: Each bit represents whether the sensor has a watch event (0 = no, 1 = watch)
5. `OK`

### Import/Export Configuration

The `E` (export) command returns a sequence of bytes used for the `I` (import) command. Both commands responds with `OK` or `EX`.

#### Export/Import Format

1. Enable state (same format as `l` command)
2. Watch state (same format as `l` command)
3. Measurement interval (4 bytes, little-endian format: lowest byte first)
4. Checksum (sum of all bytes)

### Adding New Sensors

For more details, refer to the [README documentation](doc/README.md).
