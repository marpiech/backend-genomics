import psutil
import statistics
import time
import json
import serial

# Initialize an empty list to store the data
data = []
interval = 10

# Open serial port
ser = serial.Serial('/dev/ttyACM0', 115200)

while True:

  # Get system usage information
  cpu_percent = psutil.cpu_percent()
  memory_percent = psutil.virtual_memory().percent
  disk_read_bytes = psutil.disk_io_counters().read_bytes
  disk_write_bytes = psutil.disk_io_counters().write_bytes
  network_sent_bytes = psutil.net_io_counters().bytes_sent
  network_recv_bytes = psutil.net_io_counters().bytes_recv
  temp = statistics.mean([a[1] for a in psutil.sensors_temperatures()["coretemp"]])
  
  idx = len(data)
  if (idx > 0):
    disk_read_mbs = round((disk_read_bytes - data[idx-1]["disk_read_bytes"]) / interval / 1024 / 1024, 2)
    disk_write_mbs = round((disk_write_bytes - data[idx-1]["disk_write_bytes"]) / interval / 1024 / 1024, 2)
    network_sent_mbs = round((network_sent_bytes - data[idx-1]["network_sent_bytes"]) / interval / 1024 / 1024, 2)
    network_recv_mbs = round((network_recv_bytes - data[idx-1]["network_recv_bytes"]) / interval / 1024 / 1024, 2)
  else:
    disk_read_mbs = 0
    disk_write_mbs = 0
    network_sent_mbs = 0
    network_recv_mbs = 0

  # Get system information
  max_ram_gb = round(psutil.virtual_memory().total / 1024 / 1024 / 1024, 2)
  max_threads = psutil.cpu_count()
  hdd_space_gb = round(psutil.disk_usage('/').total / 1024 / 1024 / 1024, 2) 
  hdd_percent = round(100 * (psutil.disk_usage('/').used / 1024 / 1024 / 1024 ) / hdd_space_gb, 2)

  # Store the data point as a tuple
  data_point = {"cpu_percent": cpu_percent,
                "memory_percent": memory_percent,
                "disk_read_bytes": disk_read_bytes,
                "disk_write_bytes": disk_write_bytes,
                "network_sent_bytes": network_sent_bytes,
                "network_recv_bytes": network_recv_bytes,
                "disk_read_mbs": disk_read_mbs,
                "disk_write_mbs": disk_write_mbs,
                "network_sent_mbs": network_sent_mbs,
                "network_recv_mbs": network_recv_mbs,
                "max_ram_gb": max_ram_gb,
                "max_threads": max_threads,
                "hdd_space_gb": hdd_space_gb,
                "hdd_percent": hdd_percent,
                "temp": temp}
  
  data.append(data_point)

  # If the list has more than 1000 elements, remove the oldest data point
  if len(data) > 1000:
      data.pop(0)

  # Serialize the data to JSON and sent to uart using serial.write
  ser.write((json.dumps(data_point) + "\n").encode('utf-8')) 

  # Sleep for 10 seconds
  time.sleep(interval)

