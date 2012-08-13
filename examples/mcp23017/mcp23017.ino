#define I2C_DEVICE_DEBUG
// Get this from https://github.com/rambo/I2C
#include <I2C.h> // For some weird reason including this in the relevant .h file does not work
#include <i2c_device.h> // For some weird reason including this in the relevant .h file does not work
#include <mcp23017.h>
// Container for the device
mcp23017 expander;

void setup()
{
    Serial.begin(115200);
    /*
    Serial.print(F("INPUT=B"));
    Serial.println(INPUT, BIN);
    Serial.print(F("OUTPUT=B"));
    Serial.println(OUTPUT, BIN);
    Serial.print(F("(byte)_BV(3)=B"));
    Serial.println((byte)_BV(3), BIN);
    Serial.print(F("(byte)~_BV(3)=B"));
    Serial.println((byte)~_BV(3), BIN);
    */

    // Initialize I2C library manually
    I2c.begin();
    I2c.timeOut(500);
    I2c.pullup(true);

    // Scan the bus
    I2c.scan();

    // While strictly not neccessary for the 0th board it's good reminder    
    expander.begin(0x0, false);
    
    expander.set_port_mode(0, B00000000);
    expander.set_port_mode(1, B00000000);
    expander.data[0] = B10101010;
    expander.data[1] = B01010101;
    expander.write_data();

    Serial.println(F("Booted"));
}

byte ii;
void loop()
{
    // Dump device registers and wait 15sek
    Serial.println(F("=== Dump ==="));
    expander.dump_registers(0x0, 0x15);
    Serial.println(F("=== Done ==="));
    for (byte i = 0; i < 16; i++)
    {
        if ((ii % 2) == 1)
        {
            expander.digitalWrite(i, HIGH);
        }
        else
        {
            expander.digitalWrite(i, LOW);
        }
    }
    ii++;

    delay(5000);
}
