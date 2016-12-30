# MOARstack LORA Interface layer
### Description
This project contains MOARstack interface layer for LORA interface. Layer support only Semtech SX1276/7/8/9 and Raspberry Pi.

### Dependency 
* MOARStack main repository
* [wiringPi](http://wiringpi.com/)

### Build
* Build and install wiringPi
* Copy this repository inside `layers` subdirectory of main stack repository. Rebuild whole stack, it will include LORA interface layer.

To avoid build errors, path to wiring pi can be specified `-D WIRING_PI_INC_PATH=<path>`

### Usage
By default this layer is not added to layers loaded by stack. There are two ways to add them:
* Copy `interface_lora_rpi.conf` from `config/moarstack/layers_available` to `config/moarstack/layers_enabled` directory 
* Add `-D IFACE_LORA_RPI_ENABLED=1` key to cmake while building whole stack

Run stack with `moard` and check output,that layer should be loaded and started.

### Copyright

(c) 2016 MOARstack
