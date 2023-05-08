# Smart Streetlighting using LoRaWAN

### Proof of concept project

Streetlight that adjusts its brightness according to a light sensor.
This reduces power consumption, compared to a traditional timer.
It periodically sends its brightness to an application server through
the LoRaWAN infrastructure. This allows the monitoring of Streetlights in a city.
Can be used to detect outages and monitor energy consumption - all whilst being
low powered.


Parts:
- Arduino MKRWAN 1300 
- LED (Streetlight concept)
- BH1750 light sensor

To run:
- Power up circuit and upload *light-sensor.ino* to the arduino
- `$ node-red`
- Import *flow.json* into the canvas
- Go to dashboard and view test deployment
