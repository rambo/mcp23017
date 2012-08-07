#include "mcp23017.h"

// Constructor
mcp23017::mcp23017()
{
    device_address = 0x20; // Default to the address on the breakout board with no address pins pulled down (the board has pull-ups for them)
}


void mcp23017::begin(byte board_num, boolean wire_begin)
{
    if (wire_begin)
    {
        I2c.begin();
    }

    // 0x40 is default address on the breakout board with no address pins pulled down (board has pull-ups)
    device_address = 0x20 | board_num; 

}
// Funky way to handle default arguments
void mcp23017::begin(byte board_addr)
{
    mcp23017::begin(board_addr, true);
}

void mcp23017::begin()
{
    mcp23017::begin(0x0, true);
}



/**
 * Sets the mode bits for given port (0/1), a low bit means output, default is all inputs (0xff)
 *
 * See also mcp23017::pinMode()
 */
boolean mcp23017::set_port_mode(byte port, byte mode)
{
    return this->write(0x0 + port, mode);
}

/**
 * Sets the invert bits for given port (0/1), a high bit means inverted input, default is all normal (0x0)
 */
boolean mcp23017::set_port_invert(byte port, byte mode)
{
    return this->write(0x2 + port, mode);
}

/**
 * Reads the input bits
 */
boolean mcp23017::read_data()
{
    return this->read_many(0x12, 2, this->data);
}

/**
 * Writes the output bits
 *
 * See also mcp23017::digitalWrite()
 */
boolean mcp23017::write_data()
{
    return this->write_many(0x12, 2, this->data);
}

/**
 * calls write_output and read_input, returns true if both succeed false if one fails
 */
boolean mcp23017::sync()
{
    boolean ret;
    ret = this->write_data();
    ret = ret & this->read_data();
    return ret;
}

/**
 * Do a Read,Modify,Write on the given port (we cannot use the i2c_device method since the write reg is different from the read one)
 */
boolean mcp23017::port_read_modify_write(byte port, byte mask, byte value)
{
    byte tmp;
    if (!this->read_many(0x12 + port, 1, &tmp))
    {
        return false;
    }
    tmp = (tmp & mask) | value;
    if (!this->write_many(0x12 + port, 1, &tmp))
    {
        return false;
    }
    // Upodate the local copy of the register too
    this->data[port] = tmp;
    return true;
}

/**
 * Reminders about constants
HIGH=B1
LOW=B0
INPUT=B0
OUTPUT=B1
(byte)_BV(3)=B1000
(byte)~_BV(3)=B11110111
*/

/**
 * Arduino style pinmode setting. 0-7 in portA, 8-15 in portB
 */
boolean mcp23017::pinMode(byte pin, byte mode)
{
    byte port = 0;
    if (pin > 7)
    {
        port = 1;
    }
    byte value = 0x0; // output is bit set to low
    if (mode != OUTPUT)
    {
        value = (byte)_BV(pin % 8);
    }
    return this->read_modify_write(0x0 + port, (byte)~_BV(pin % 8), value);
}

/**
 * Set input inversion on single pin, Arduino style
 */
boolean mcp23017::pinInvert(byte pin, boolean invert)
{
    byte port = 0;
    if (pin > 7)
    {
        port = 1;
    }
    byte value = 0x0; // default is non-inverting input
    if (invert)
    {
        value = (byte)_BV(pin % 8);
    }
    return this->read_modify_write(0x2 + port, (byte)~_BV(pin % 8), value);
}


/**
 * Arduino style pin value setting. 0-7 in portA, 8-15 in portB
 */
boolean mcp23017::digitalWrite(byte pin, byte state)
{
    byte port = 0;
    if (pin > 7)
    {
        port = 1;
    }
    byte value = 0x0;
    if (state == HIGH)
    {
        value = (byte)_BV(pin % 8);
    }
    return this->port_read_modify_write(port, (byte)~_BV(pin % 8), value);
}

/**
 * Arduino style pin value setting. 0-7 in portA, 8-15 in portB
 */
boolean mcp23017::digitalRead(byte pin)
{
    this->read_data();
    byte port = 0;
    if (pin > 7)
    {
        port = 1;
    }
    if (this->data[port] & (byte)_BV(pin % 8))
    {
        return true;
    }
    return false;
}

