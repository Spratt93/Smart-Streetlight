<div align="center"> <h1>Smart Street Lighting  🏙️ </h1> </div>

![GitHub watchers](https://img.shields.io/github/watchers/Spratt93/Smart-Streetlight?style=for-the-badge)

<div align="center">
    <img src="https://img.shields.io/badge/-Arduino-00979D?style=for-the-badge&logo=Arduino&logoColor=white" alt="badge">
    <img src="https://img.shields.io/badge/Node--RED-%238F0000.svg?style=for-the-badge&logo=node-red&logoColor=white" alt="badge">
</div>

### Intro
This project is a proof of concept. It shows how LoRaWAN can be
used to for remote monitoring and control of street lights.
I designed a street light that:
- Adjusts its brightness depending on ambient light
- Sends its current brightness and power draw over LoRa
- This is communicated to my application running on The Things Network
- My node-RED GUI interfaces with the application using MQTT

### Circuit Parts
- Arduino MKRWAN 1300 
- LED
- BH1750 light sensor

### Deployment
- Power up circuit and upload *light-sensor.ino* to the arduino
- `$ node-red`
- Go to *localhost:1880*
- Import *flow.json* into the canvas
- Go to dashboard and view test deployment

### Dashboard
<div align="center">
    <img src="./ui-final.png" alt="dashboard" width="500">
</div>

