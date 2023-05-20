// using namespace std;         Does not build if this is here
// #include <iostream>
#include <DueTimer.h>
#include <utilities/logging.h>
#include <function_timings.h>
#include <display/screens/screen.h>
#include <utilities/util.h>
#include <display/layouts/layouts.h>
#include "../config/uvent_conf.h"
#include "controls/control.h"
#include "display/TftDisplay.h"
#include "sensors/pressure_sensor.h"
#include "sensors/test_pressure_sensors.h"
#include "display/main_display.h"
#include "utilities/parser.h"
#include "eeprom/test_eeprom.h"

#include <SPI.h>

//IOV CONTROL 
#include <Ethernet2.h>


//sensor 
int photosensor = A1;

// Enter a MAC address and IP address for your controller below.
// The IP address will be dependent on your local network:
byte mac[] = {
  0xDE, 0xAD, 0xBE, 0xEF, 0xFE, 0xED
  //0xC4, 0xE9, 0x84, 0xD5, 0xDA, 0xA2
};
IPAddress ip(192, 168, 0, 177);
IPAddress gateway(192,168,0,1);
IPAddress subnet(255,255,255,0);
//IPAddress ip(192, 168, 0, 1);
//IPAddress pingAddr(192, 168, 0, 177);

// Initialize the Ethernet server library
// with the IP address and port you want to use
// (port 80 is default for HTTP):
EthernetServer server(80);





//Pressure Relief Valve Library 


// #include <Servo.h>
// #include "actuators/prv.h"           Issues with building when uncommented

// Servo prvservo; 

// int pos = 0;  
//double current_pressure; 
//PressureSensor from control.cpp;
//double current_pressure = control_get_gauge_pressure();



TftDisplay tft_display = {TFT_CS, TFT_RST, TOUCH_INT, TOUCH_RST};
MainScreen main_screen;
StartupScreen startup_screen;

// Parser instance to parse commands on the serial line
Parser parser;
test_eeprom eeprom_test;

// Timers
lv_timer_t* update_readout_timer = nullptr;

static void on_startup_confirm_button(lv_event_t* evt)
{
    LV_UNUSED(evt);
    // Destroy the startup screen
    startup_screen.cleanup();
    // Need to be able to create new windows later
    active_floating_window = nullptr;
    // Tell LVGL this is the currently loaded screen
    main_screen.select_screen();
    // Init containers, styles, defaults...
    init_main_display();
    // Creates all the components that go on the main screen in order for it to function.
    main_screen.setup();
    // Arm the speaker so it talks to LVGL on mute/unmute
    control_setup_alarm_cb();

    // LVGL Timer to poll sensors and update screen data
#if ENABLE_CONTROL
    update_readout_timer = lv_timer_create(loop_update_readouts, SENSOR_POLL_INTERVAL, &main_screen);
#else
    // Setup an LVGL timer to loop/update display. Polls sensors, updates graphs, etc.
    update_readout_timer = lv_timer_create(loop_test_readout, SENSOR_POLL_INTERVAL, &main_screen);
#endif
}

static void setup_screens()
{
    // Create the screen objects
    startup_screen.init();
    main_screen.init();

    startup_screen.select_screen();
    startup_screen.on_complete = on_startup_confirm_button;
    startup_screen.setup();
}

// TODO clean up setup & main loop

void setup()
{
    Serial.begin(SERIAL_BAUD_RATE);

    if (!tft_display.init()) {
        while (1);
    }

    // Enable stepper, actuator, etc
    // Load EEPROM storage
    control_init();

    /*******************************/
    /* Setup & arrange the display */
    /*******************************/

    // Set up classes with controlled values, load defaults.
    init_adjustable_values();
    setup_screens();

    // Initialize the parser, with the command array and command array size
    parser.init(command_get_array(), command_get_array_size());

    // prvservo.attach(9); //for prv        Issues with building when uncommented



    //IOV SETUP CONTROL

    // Open serial communications and wait for port to open:
  Serial.begin(9600);
  while (!Serial) {
    ; // wait for serial port to connect. Needed for Leonardo only
  }


  // start the Ethernet connection and the server:
  Ethernet.begin(mac, ip);
  Serial.println(ip);
  //Ethernet.begin(ip);
  server.begin();
  Serial.print("server is at ");
  Serial.println(Ethernet.localIP());



}

void loop()
{
    tft_display.update();
    delay(5);

    parser.service();

#if ENABLE_TEST_PRESSURE_SENSORS
    //test_sensors_read_pressure(250, true, Units_pressure::cmH20, Units_pressure::mbar);
    //test_sensors_read_gauge(250, true, Units_pressure::cmH20);
    //test_sensors_read_differential(250, true, Units_pressure::mbar);
    //test_sensors_read_flow(250, true, Order_type::first, units_flow::lpm);
    //test_sensors_read_flow(250, true, Order_type::second, units_flow::lpm);
    test_sensors_read_flow(100, true, Order_type::third, units_flow::lpm);
#endif

#if ENABLE_TEST_EEPROM
    eeprom_test.select_test();
    delay(1000);
#endif

    control_service();



    //IOV LOOP CONTROL 
    // listen for incoming clients
  EthernetClient client = server.available();
  if (client) {
    Serial.println("new client");
    // an http request ends with a blank line
    boolean currentLineIsBlank = true;
    while (client.connected()) {
      if (client.available()) {
        char c = client.read();
        Serial.write(c);
        // if you've gotten to the end of the line (received a newline
        // character) and the line is blank, the http request has ended,
        // so you can send a reply
        if (c == '\n' && currentLineIsBlank) {
          // send a standard http response header
          client.println("HTTP/1.1 200 OK");
          client.println("Content-Type: text/html");
          client.println("Connection: close");  // the connection will be closed after completion of the response
          client.println("Refresh: 5");  // refresh the page automatically every 5 sec
          client.println();
          client.println("<!DOCTYPE HTML>");
          client.println("<html>");
          // output the value of each analog input pin
            client.print("Current Flow value is ");
            client.print(control_get_guage_pressure());
            
         
            client.println("<br />");
          }
          client.println("</html>");
          break;
        }
        if (c == '\n') {
          // you're starting a new line
          currentLineIsBlank = true;
        }
        else if (c != '\r') {
          // you've gotten a character on the current line
          currentLineIsBlank = false;
        }
      }
    }
    // give the web browser time to receive the data
    delay(1);
    // close the connection:
    client.stop();
    Serial.println("client disconnected");
  }

//PRV control 
/*
if (current_pressure >=40){
for (pos = 0; pos <= 180; pos += 1) { 
    // in steps of 1 degree
    prvservo.write(pos);             
    delay(15);                       
  }
  for (pos = 180; pos >= 0; pos -= 1) { 
    prvservo.write(pos);              
    delay(15);                       
  }
} */
}

#if USE_DMA_INTERRUPT

ISR(DMAC_Handler)
{
    RA_DEBUG_STOP(SPI_TIMING);
    RA_DEBUG_START(INTERRUPT);
    tft_display.onDMAInterrupt();
    RA_DEBUG_STOP(INTERRUPT);
}

#endif

/*
// PRV 
PressureSensor returned_pressure_val;

PrvServoLib prvServo;
double current_pressure;

int ServoPin = 5;
int level = 0;


void setup(){

  pinMode(ServoPin, OUTPUT);
  prvServo.setPin(MyServoPin);  
  prvServo.attach();

  // your additional setup code here
}

void loop(){

  current_pressure = returned_pressure_val.get_pressure(Units_pressure::cmH20,true);
if (current_pressure >=40){
for(level = 0; level < 180; level += 1) 	 // command to move from 0 degrees to 180 degrees 
  {                                  
    prvServo.write(level);              	 //command to rotate the servo to the specified angle
    delay(15);                       
  } 
 
  delay(1000);
}

}

*/