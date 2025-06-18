import serial.tools.list_ports
from SCons.Script import Import

Import("env")

TARGET_LOCATION = env.GetProjectOption("usb_location");

ports = serial.tools.list_ports.comports()
for port in ports:
    if hasattr(port, 'location') and port.location == TARGET_LOCATION:
        print(f"Using port {port.device} for LOCATION {TARGET_LOCATION}")
        env.Replace(UPLOAD_PORT=port.device)
        env.Replace(MONITOR_PORT=port.device)
        break
else:
    print(f"ERROR: No device with LOCATION {TARGET_LOCATION} found.")
