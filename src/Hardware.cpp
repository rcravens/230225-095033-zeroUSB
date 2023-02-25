#include "Hardware.h"

#define LTE_RESET_PIN 6
#define LTE_PWRKEY_PIN 5
#define LTE_FLIGHT_PIN 7

#define PIN_SD_SELECT 4

#define MODEM_RXD 0
#define MODEM_TXD 1

namespace rlc
{
    Hardware::Hardware(rlc::AtCommand command_helper) : _command_helper(command_helper)
    {
    }

    void Hardware::init()
    {
        SerialUSB.begin(115200);

        Serial1.begin(115200);

        pinMode(LTE_RESET_PIN, OUTPUT);
        pinMode(LTE_PWRKEY_PIN, OUTPUT);
        pinMode(LTE_FLIGHT_PIN, OUTPUT);

        digitalWrite(LTE_RESET_PIN, LOW);
        delay(100);

        digitalWrite(LTE_FLIGHT_PIN, LOW); // LOW = Normal Mode, HIGH = Flight Mode
        delay(1000);
    }

    bool Hardware::init_sd()
    {
        if (!SD.begin(PIN_SD_SELECT))
        {
            return false;
        }
        return true;
    }

    bool Hardware::is_module_on()
    {
        for (int i = 0; i < 5; i++)
        {
            if (_command_helper.send_command_and_wait("AT"))
            {
                return true;
            }
            delay(1000);
        }
        return false;
    }

    bool Hardware::turn_off_module()
    {
        if (is_module_on()) // if it's off, turn on it.
        {
            digitalWrite(LTE_PWRKEY_PIN, HIGH);
            delay(3000);
            digitalWrite(LTE_PWRKEY_PIN, LOW);
            delay(10000);

            return is_module_on() == false;
        }

        return true;
    }

    bool Hardware::turn_on_module()
    {
        if (!is_module_on()) // if it's off, turn on it.
        {
            digitalWrite(LTE_PWRKEY_PIN, LOW);
            delay(3000);
            digitalWrite(LTE_PWRKEY_PIN, HIGH);
            delay(10000);

            return is_module_on();
        }

        return true;
    }

    bool Hardware::configure_module()
    {
        if (is_module_on())
        {
            _command_helper.send_command_and_wait("AT");
            _command_helper.send_command_and_wait("AT+CMEE=2");
            if (!_command_helper.send_command_and_wait("AT+CGMI")) // request manufacturer indentification
            {
                return false;
            }
            manufacturer = _command_helper.last_command_response;
            if (!_command_helper.send_command_and_wait("AT+CGMM")) // request model identification
            {
                return false;
            }
            model = _command_helper.last_command_response;
            if (!_command_helper.send_command_and_wait("AT+CGSN")) // request product serial number identification
            {
                return false;
            }
            imei = _command_helper.last_command_response;
            //_command_helper.send_command_and_wait("AT+CSUB");   // request the module version and chip
            //_command_helper.send_command_and_wait("AT+CPIN?");  // request the state of the SIM card
            //_command_helper.send_command_and_wait("AT+CICCID"); // read ICCID from SIM card
            //_command_helper.send_command_and_wait("AT+CNUM");   // request the subscriber number
            //_command_helper.send_command_and_wait("AT+COPS");   // check the current network operator
            //_command_helper.send_command_and_wait("AT+IPREX?"); // check local baud rate

            //_command_helper.send_command_and_wait("AT+CREG?");
            _command_helper.send_command_and_wait("AT+CGATT=1");
            _command_helper.send_command_and_wait("AT+CGACT=1,1");
            // send_command_and_wait("AT+CMFG=1"); // Sets SMS message format to TEXT mode
            _command_helper.send_command_and_wait("AT+CGDCONT=1,\"IP\",\"super\"");

            return true;
        }

        return false;
    }

    void Hardware::send_module_output_to_console_out()
    {
        while (Serial1.available() > 0)
        {
            SerialUSB.write(Serial1.read());
            yield();
        }
    }

    void Hardware::send_console_input_to_module()
    {

        while (SerialUSB.available() > 0)
        {
            int c = -1;
            c = SerialUSB.read();
            if (c != '\n' && c != '\r')
            {
                _from_usb += (char)c;
            }
            else
            {
                if (!_from_usb.equals(""))
                {
                    SerialUSB.println(_from_usb);
                    _command_helper.send_data(_from_usb, 1000);
                    _from_usb = "";
                }
            }
            yield();
        }
    }

}