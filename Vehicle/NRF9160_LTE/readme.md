# HyTech NRF9160 LTE

## Environment Setup

Setup steps are similar to the [Sparkfun guide](https://learn.sparkfun.com/tutorials/nrf9160-thing-plus-hookup-guide/all). 

- Use nRF Connect SDK v2.1.1 instead of their provided version
- Don't bother with getting their examples either

Everything else should be the same
    
## Build Commands

cd into this directory from the zephyr terminal

To build,

```
west build -b circuitdojo_feather_nrf9160_ns -- -DOVERLAY_CONFIG="overlay-hytech.conf"
```

to build from scratch

```
west build -b circuitdojo_feather_nrf9160_ns --pristine -- -DOVERLAY_CONFIG="overlay-hytech.conf"
```

## Pin configurations

Pinouts have to be configured in devicetree, not in the code.

Pin configuration overlays are located in boards/circuitdojo_feather_nrf9160_ns.overlay

After building, the final devicetree file will be located in build\zephyr\zephyr.dts

## Other configurations

Zephyr has configuration options specified in the overlay-hytech.conf and prj.conf files

The Kconfig file specifies constants that can be used in the code

## Todo

- Currently the uart is interrupt driven, I have no idea whether this could cause issues if it interrupts while the modem is sending out stuff.