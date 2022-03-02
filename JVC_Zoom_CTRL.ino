///////////////////////////////////////////////////////////////////////////////////////
// Zoom Lens remote controller for JVC GY-HM70E
// and compatible camcorders
// Uses transmission via RS232 8N1 @ 9600 BAUD
// (requires UART <-> RS232 transceiver)
// Jack 3,5mm TRRS terminal connections:
// Sleeve = GND
// Ring 1 = TX (from camera)
// Ring 2 = RX (from remote controller)
// Tip = 5V from Camcorder
// The potentiometer is a spring-loaded B10K Pitch-Wheel as used for 
// digital keyboard instruments, that returns in the middle position
//
// Code for Arduino ProMini (Uno/Duemilanove - ATMEGA328P)
// by Guido Scognamiglio - www.GenuineSoundware.com
// Last update: Feb 2022
//

// Set to 1 only for debugging in Arduino serial console
#define DEBUG 0

#define PIN_ZOOM_POT    14 // A0
#define PIN_ZOOM_GND     4
#define PIN_ZOOM_VCC     5
#define PIN_ZOOM_DIR     6 // Attach a toggle switch to reverse the response of the wheel

// This code could be expanded with a second wheel for controlling the focus...
//#define PIN_FOCUS_POT   15 // A1
//#define PIN_FOCUS_GND    7 
//#define PIN_FOCUS_VCC    8
//#define PIN_FOCUS_DIR    9

// The potentiometer on the pitch wheel does not complete a 270° arc, it barely covers a 90° arc.
// So, within the ADC 10 bit range, we must find an ideal minimum and maximum value, leaving some margin.
// Plus, we need a "dead zone" around the middle position within which any little pot movement is ignored.
// Adjust these values according to the pitch-wheel in use.
#define ZOOM_RANGE_MIN 360
#define ZOOM_RANGE_MAX 640
#define ZOOM_DEAD_ZONE  40

struct AIN_Struct
{
  //int AIN_PrevValue = 0;
  int Range, MidPoint;
  int Direction = 0;
};
AIN_Struct Zoom;
int LED = 0;

void SetZoomDirection(int d)
{
  Zoom.Direction = d;
  digitalWrite(PIN_ZOOM_GND, d);
  digitalWrite(PIN_ZOOM_VCC, !d);
}

void setup() 
{
  Serial.begin(9600);

  // Swap these for reversing the pot direction
  pinMode(PIN_ZOOM_DIR, INPUT_PULLUP);
  pinMode(PIN_ZOOM_GND, OUTPUT); 
  pinMode(PIN_ZOOM_VCC, OUTPUT); 

  pinMode(LED_BUILTIN, OUTPUT); 
  digitalWrite(LED_BUILTIN, HIGH);

  Zoom.Range = ZOOM_RANGE_MAX - ZOOM_RANGE_MIN;
  Zoom.MidPoint = ZOOM_RANGE_MIN + (Zoom.Range / 2);
}

void loop() 
{
  ////////////////////////////////////////////////////////////////////////////////////////////////////
  // ZOOM
  
  // Change Zoom Direction
  int ZoomDir = digitalRead(PIN_ZOOM_DIR);
  if (ZoomDir != Zoom.Direction) 
    SetZoomDirection(ZoomDir);
  
  // Defaut = Zoom stop
  int ZoomValue8bit = 128;
  
  // Get the Zoom pot 10-bit value
  auto Zoom_AIN_Value = analogRead(PIN_ZOOM_POT);
  
  // Keep ADC reading within the useful range
  if (Zoom_AIN_Value > ZOOM_RANGE_MAX) Zoom_AIN_Value = ZOOM_RANGE_MAX;
  if (Zoom_AIN_Value < ZOOM_RANGE_MIN) Zoom_AIN_Value = ZOOM_RANGE_MIN;
  
  // Scale value to 8-bit value (0 ~ 255)
  ZoomValue8bit = (int)((float)(Zoom_AIN_Value - ZOOM_RANGE_MIN) * (255.f / (float)Zoom.Range));

  // If within the dead zone
  if (Zoom_AIN_Value >= (Zoom.MidPoint - ZOOM_DEAD_ZONE) && 
      Zoom_AIN_Value <= (Zoom.MidPoint + ZOOM_DEAD_ZONE))
  {
    ZoomValue8bit = 128;
  }

  // Send ZOOM command 95 4C 30 V1 V2 CK
  byte V1 = ZoomValue8bit >> 7;
  byte V2 = ZoomValue8bit & 0x7F;
  byte CheckSum = (0x4C + 0x30 + V1 + V2) & 0x7F;
  byte data[] = { 0x95, 0x4C, 0x30, V1, V2, CheckSum };

#if DEBUG == 1
  //Serial.print("ZOOM Value = ");
  //Serial.println(ZoomValue8bit);
  char msg[20]; sprintf(msg, "95 4C 30 %02X %02X %02X", V1, V2, CheckSum);
  Serial.println(msg);
#else
  Serial.write(data, sizeof(data));
#endif

  // Blink activity LED
  digitalWrite(LED_BUILTIN, LED); LED = !LED;

  // Apparently the camcorder turns off the Remote Controller if there's no data for a certain
  // amount of time, so rather than blocking the sending of repeated messages, just keep sending
  // with a short pause.
  delay(100);
}
