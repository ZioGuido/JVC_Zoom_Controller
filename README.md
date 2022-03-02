# JVC_Zoom_Controller
Zoom Controller for JVC Camcorder GY-HM70/150 with Arduino

This kind of camcorder isn't compatible with the common Sony LARC controllers that can be found for cheap, instead, requires a dedicated controller that is also quite rare to find new today. I was lucky to get in touch with someone at JVC who revealed the RS232 protocol used by the camera to receive commands from the controllers. All I needed was a zoom control, I don't need to control focus, iris or recording stop/start, so I made this project using a Adruino Pro Mini board (ATMEGA328P) and a UART<->RS232 transceiver.

The camera uses a TRRS 3,5mm jack as opposed to the Sony LARC TRS 2,5mm jack. Anyway, since I don't need to receive data from the camera, I used a TRS with ground on sleeve, data on ring and Vcc on tip. Once connected the Arduino UART to the RS232 transceiver and then to the jack, the camera provides power as soon as the jack is inserted and keeps the controller on while the data is constantly received. If the controller stops sending data or is disconnected, the power is interrupted.

See it in action here:
https://www.youtube.com/watch?v=i0gEynOMTOM

