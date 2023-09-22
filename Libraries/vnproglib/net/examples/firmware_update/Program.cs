using System;
using System.Threading;

// Allows access to data types within the VectorNav .NET Library.
using VectorNav.Sensor;
using VectorNav.Math;
using VectorNav.Protocol.Uart;

class Program
{
	static void one_call_method_for_the_VN300()
	{
        // This example walks through using the VectorNav NET Library to do a firmware update on the different processors of the VN300 using the firmwareUpdate method

        // First determine which COM port your sensor is attached to and update the constant below. Also, if you have changed your sensor from the
        // factory default baudrate of 115200, you will need to update the baudrate constant below as well.
        const string SensorPort = "COM4";               // Windows format for physical and virtual (USB) serial port.
		// const string SensorPort = "/dev/ttyS1";      // Linux format for physical serial port.
		// const string SensorPort = "/dev/ttyUSB0";    // Linux format for virtual (USB) serial port.
		const UInt32 SensorBaudrate = 115200;
		const UInt32 FirmwareUpdateBaudrate = 115200;   // This can be different than the sensor baudrate, recommend 115200 or slower for reliability
        string VN300_NAVFirmwareUpdate = "../../../../FirmwareUpdates/VN300_NAV_v0.5.0.0.vnx";
        string VN300_GPSFirmwareUpdate = "../../../../FirmwareUpdates/VN300_GPS_v0.5.0.0.vnx";

        // Now let's create a VnSensor object and use it to connect to our sensor.
        Console.WriteLine("Connecting to sensor");
		VnSensor vs = new VnSensor();
		vs.Connect(SensorPort, SensorBaudrate);

		Console.WriteLine("Querry Sensor Information");
		// Let's query the sensor's model number.
		string mn = vs.ReadModelNumber();
		Console.WriteLine("Model Number: " + mn);

		// Let's query the sensor's firmware version.
		string fwv = vs.ReadFirmwareVersion();
		Console.WriteLine("Firmware Version: " + fwv);

		// Let's update the firmware on the target processor.
		Console.WriteLine("Updating NAV processor");
		vs.FirmwareUpdate(FirmwareUpdateBaudrate, VN300_NAVFirmwareUpdate);

        // Let's switch to the target processor to update.
        Console.WriteLine("Switching to the GPS processor to update");
        vs.SwitchProcessors(VnSensor.VnProcessorType.GPS, mn, fwv);

        // Let's update the firmware on the target processor.
        Console.WriteLine("Updating GPS processor");
        vs.FirmwareUpdate(FirmwareUpdateBaudrate, VN300_GPSFirmwareUpdate);

        // Now disconnect from the sensor since we are finished.
        Console.WriteLine("Disconnecting from sensor");
		vs.Disconnect();

        // Depending on the sensor, a power cycle may be needed instead of a reset.
        Console.WriteLine("Power Cycle the sensor");
    }

    static void manage_each_step_method_for_the_VN200()
    {
        // This example walks through using the VectorNav NET Library to do a firmware update of the VN200 using the firmwareUpdate method

        // First determine which COM port your sensor is attached to and update the constant below. Also, if you have changed your sensor from the
        // factory default baudrate of 115200, you will need to update the baudrate constant below as well.
        const string SensorPort = "COM3";               // Windows format for physical and virtual (USB) serial port.
        // const string SensorPort = "/dev/ttyS1";      // Linux format for physical serial port.
        // const string SensorPort = "/dev/ttyUSB0";    // Linux format for virtual (USB) serial port.
        const UInt32 SensorBaudrate = 115200;
        const UInt32 FirmwareUpdateBaudrate = 115200;
        string VN200FirmwareUpdate = "../../../../FirmwareUpdates/VN200_NAV_v2.0.0.1.vnx";

        // Now let's create a VnSensor object and use it to connect to our sensor.
        Console.WriteLine("Connecting to sensor");
        VnSensor vs = new VnSensor();
        vs.Connect(SensorPort, SensorBaudrate);

        Console.WriteLine("Querry Sensor Information");
        // Let's query the sensor's model number.
        string mn = vs.ReadModelNumber();
        Console.WriteLine("Model Number: " + mn);

        // Let's query the sensor's firmware version.
        string fwv = vs.ReadFirmwareVersion();
        Console.WriteLine("Firmware Version: " + fwv);

        // Let's update the firmware on the target processor.
        string record = "";

        // Open firmware update file
        Console.WriteLine("Load Update File: {0}", VN200FirmwareUpdate);
        vs.OpenFirmwareUpdateFile(VN200FirmwareUpdate);

        // Enter bootloader mode
        Console.WriteLine("Enter Bootloader Mode");
        vs.FirmwareUpdateMode(true);

        // Give the sensor time to reboot into bootloader mode
        Thread.Sleep(1000);

        // Configure baudrate for firmware update
        Console.WriteLine("Change baud rate to bootloader to: {0}", FirmwareUpdateBaudrate);
        vs.SetFirmwareUpdateBaudRate(FirmwareUpdateBaudrate);

        // Calibrate the bootloader by letting it calculate the current baudrate.
        string bootloaderVersion = vs.CalibrateBootloader();
        Console.WriteLine(bootloaderVersion);

        // Send each record from the firmware update file one at a time
        Console.WriteLine("Updating processor");
        do
        {
            record = vs.GetNextFirmwareUpdateRecord();
            if ((record != null) && (record.Length != 0))
            {
                // Remove the ':' from the beginning of the line.
                vs.WriteFirmwareUpdateRecord(record.TrimStart(':'), true);
                Console.Write(".");
            }
        } while ((record != null) && (record.Length != 0));
        Console.WriteLine("!");

        // Close firmware update file
        Console.WriteLine("Close the Update File");
        vs.CloseFirmwareUpdateFile();

        // Reset baudrate
        Console.WriteLine("Reset baud rate back to the sensor's baud rate of {0}", SensorBaudrate);
        vs.SetFirmwareUpdateBaudRate(SensorBaudrate);

        // Exit bootloader mode. Just sleep for 10 seconds or reset
        Console.WriteLine("Wait 10 seconds to exit bootloader mode");
        Thread.Sleep(10000);

        Console.WriteLine("Issue a reset");
        vs.Reset();

        // Exit bootloader mode. Just sleep for 10 seconds or reset
        Console.WriteLine("Wait 2 seconds after a reset");
        Thread.Sleep(2000);

        // Now disconnect from the sensor since we are finished.
        Console.WriteLine("Disconnecting from sensor");
        vs.Disconnect();

        // Depending on the sensor, a power cycle may be needed instead of a reset.
        Console.WriteLine("Power Cycle the sensor");

    }


    static void one_call_method_for_the_VN310()
    {
        // This example walks through using the VectorNav NET Library to do a firmware update on the different processors of the VN300 using the firmwareUpdate method

        // First determine which COM port your sensor is attached to and update the constant below. Also, if you have changed your sensor from the
        // factory default baudrate of 115200, you will need to update the baudrate constant below as well.
        const string SensorPort = "COM6";               // Windows format for physical and virtual (USB) serial port.
                                                        // const string SensorPort = "/dev/ttyS1";      // Linux format for physical serial port.
                                                        // const string SensorPort = "/dev/ttyUSB0";    // Linux format for virtual (USB) serial port.
        const UInt32 SensorBaudrate = 115200;
        const UInt32 FirmwareUpdateBaudrate = 115200;   // This can be different than the sensor baudrate, recommend 115200 or slower for reliability
        string VN310_NAVFirmwareUpdate = "../../../../FirmwareUpdates/VN310_NAV_v0.8.1.0.vnx";
        string VN310_GPSFirmwareUpdate = "../../../../FirmwareUpdates/VN310_GPS_v0.8.1.0.vnx";
        string VN310_IMUFirmwareUpdate = "../../../../FirmwareUpdates/VN310_IMU_v2.5.1.0.vnx";

        bool updateNAV = true;
        bool updateGPS = true;
        bool updateIMU = true;

        // Now let's create a VnSensor object and use it to connect to our sensor.
        Console.WriteLine("Connecting to sensor");
        VnSensor vs = new VnSensor();
        vs.Connect(SensorPort, SensorBaudrate);

        Console.WriteLine("Querry Sensor Information");
        // Let's query the sensor's model number.
        string mn = vs.ReadModelNumber();
        Console.WriteLine("Model Number: " + mn);

        // Let's query the sensor's firmware version.
        string fwv = vs.ReadFirmwareVersion();
        Console.WriteLine("Firmware Version: " + fwv);

        // Ensure we are starting with the NAV processor
        Console.WriteLine("Switching to the NAV processor to update");
        vs.SwitchProcessors(VnSensor.VnProcessorType.NAV, mn, fwv);

        if (updateNAV)
        {
            // Let's update the firmware on the target processor.
            Console.WriteLine("Updating NAV processor");
            vs.FirmwareUpdate(FirmwareUpdateBaudrate, VN310_NAVFirmwareUpdate);

            Console.WriteLine("Switching back to the NAV processor");
            vs.SwitchProcessors(VnSensor.VnProcessorType.NAV, mn, fwv);
        }

        if (updateGPS)
        {
            // Let's switch to the target processor to update.
            Console.WriteLine("Switching to the GPS processor to update");
            vs.SwitchProcessors(VnSensor.VnProcessorType.GPS, mn, fwv);

            // Let's update the firmware on the target processor.
            Console.WriteLine("Updating GPS processor");
            vs.FirmwareUpdate(FirmwareUpdateBaudrate, VN310_GPSFirmwareUpdate);

            // Let's switch to the NAV processor since we cannot switch directly to the IMU from the GPS
            Console.WriteLine("Switching back to the NAV processor");
            vs.SwitchProcessors(VnSensor.VnProcessorType.NAV, mn, fwv);
        }

        if (updateIMU)
        {
            // Let's switch to the target processor to update.
            Console.WriteLine("Switching to the IMU processor to update");
            vs.SwitchProcessors(VnSensor.VnProcessorType.IMU, mn, fwv);

            // Let's update the firmware on the target processor.
            Console.WriteLine("Updating IMU processor");
            vs.FirmwareUpdate(FirmwareUpdateBaudrate, VN310_IMUFirmwareUpdate);

            // Let's switch back to the NAV processor before we leave
            Console.WriteLine("Switching back to the NAV processor");
            vs.SwitchProcessors(VnSensor.VnProcessorType.NAV, mn, fwv);
        }

        // Now disconnect from the sensor since we are finished.
        Console.WriteLine("Disconnecting from sensor");
        vs.Disconnect();
    }

    static void Main(string[] args)
    {
        Console.WriteLine("********************************************************************************");
        Console.WriteLine("Updating the firmware on a VN200");
        Console.WriteLine("********************************************************************************");
        manage_each_step_method_for_the_VN200();

        Console.WriteLine("********************************************************************************");
        Console.WriteLine("Updating the firmware on a VN300");
        Console.WriteLine("********************************************************************************");
        one_call_method_for_the_VN300();

        Console.WriteLine("********************************************************************************");
        Console.WriteLine("Updating the firmware on a VN310");
        Console.WriteLine("********************************************************************************");
        one_call_method_for_the_VN310();
    }

}

