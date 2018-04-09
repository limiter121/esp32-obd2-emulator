# Third Party Components

- ESPCan Driver 
  - Base CAN Driver [forked from Thomas Barth](https://github.com/ThomasBarth/ESP32-CAN-Driver)
  - General [Component CAN Driver Pack](https://github.com/ESP32DE/ESP32-CAN-Driver/tree/Component_CAN_Driver_Pack) Work for ESP-IDF with menuconfig from [rudi ;-)](http://esp32.de)

Changelog:
  - Better throughput waking up the HigherPriorityTask in the ISR
  - Wait for the physical TX to be completed before writing another frame to avoid internal buffer corruptions in some high-load scenarios
  - Added 200KBPS CAN speed
