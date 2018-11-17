# Arduino OneWire Device Library

Library to access OneWire devices from Arduino-compatible systems.

This code is specifically for the OneWire Weather Station devices but should be usable for any other Arduino-compatible system needing generic OneWire device access.

Why have I written this library when so many more are already out there? Well, because most of them seem to include only temperature sensors and I've got a whole weather station with counters, analong to digital converters, and a bunch of other OneWire-y type things connected. Nothing I found has all these devices collected together in a usable "generic" manner.

Much of the underlying individual device code comes from [Joe Bechter](https://github.com/jbechter). I've patched it into sub and super classes so that inheritance works and include some devices that he did not. Thanks Joe.