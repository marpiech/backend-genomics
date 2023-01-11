import psutil
import statistics
import time
import serial

# Initialize an empty list to store the data
data = []
interval = 10
cache_items = 200

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
    disk_read_mbs = round(8 * (disk_read_bytes - data[idx-1]["disk_read_bytes"]) / interval / 1024 / 1024, 2)
    # calculate disk read speed as percent of 6 Gb/s (6000 MB/s), round and set to 100 if > 100 and to 0 if < 0
    disk_read_percent = max(0, min(100, int(100 * disk_read_mbs / 12000)))
    disk_write_mbs = round(8 * (disk_write_bytes - data[idx-1]["disk_write_bytes"]) / interval / 1024 / 1024, 2)
    disk_write_percent = max(0, min(100, int(100 * disk_write_mbs / 12000)))
    network_sent_mbs = round(8 * (network_sent_bytes - data[idx-1]["network_sent_bytes"]) / interval / 1024 / 1024, 2)
    # calculate network sent speed as percent of 1 Gb/s (1000 MB/s), round and set to 100 if > 100 and to 0 if < 0
    network_sent_percent = max(0, min(100, int(100 * network_sent_mbs / 1000)))
    network_recv_mbs = round(8 * (network_recv_bytes - data[idx-1]["network_recv_bytes"]) / interval / 1024 / 1024, 2)
    network_recv_percent = max(0, min(100, int(100 * network_recv_mbs / 1000)))
  else:
    disk_read_mbs = 0
    disk_read_percent = 0
    disk_write_mbs = 0
    disk_write_percent = 0
    network_sent_mbs = 0
    network_sent_percent = 0
    network_recv_mbs = 0
    network_recv_percent = 0

  # Get system information
  max_ram_gb = round(psutil.virtual_memory().total / 1024 / 1024 / 1024, 2)
  max_threads = psutil.cpu_count()
  hdd_space_gb = round(psutil.disk_usage('/').total / 1024 / 1024 / 1024, 2)
  hdd_percent = round(100 * (psutil.disk_usage('/').used / 1024 / 1024 / 1024 ) / hdd_space_gb, 2)

  # Store the data point as a tuple
  init_point = {"cpu_percent": 0,
                "memory_percent": 0,
                "disk_read_bytes": 0,
                "disk_write_bytes": 0,
                "network_sent_bytes": 0,
                "network_recv_bytes": 0,
                "disk_read_mbs": 0,
                "disk_read_percent": 0,
                "disk_write_mbs": 0,
                "disk_write_percent": 0,
                "network_sent_mbs": 0,
                "network_sent_percent": 0,
                "network_recv_mbs": 0,
                "network_recv_percent": 0,
                "max_ram_gb": 0,
                "max_threads": 0,
                "hdd_space_gb": 0,
                "hdd_percent": 0,
                "temp": 0}

  data_point = {"cpu_percent": cpu_percent,
                "memory_percent": memory_percent,
                "disk_read_bytes": disk_read_bytes,
                "disk_write_bytes": disk_write_bytes,
                "network_sent_bytes": network_sent_bytes,
                "network_recv_bytes": network_recv_bytes,
                "disk_read_mbs": disk_read_mbs,
                "disk_read_percent": disk_read_percent,
                "disk_write_mbs": disk_write_mbs,
                "disk_write_percent": disk_write_percent,
                "network_sent_mbs": network_sent_mbs,
                "network_sent_percent": network_sent_percent,
                "network_recv_mbs": network_recv_mbs,
                "network_recv_percent": network_recv_percent,
                "max_ram_gb": max_ram_gb,
                "max_threads": max_threads,
                "hdd_space_gb": hdd_space_gb,
                "hdd_percent": hdd_percent,
                "temp": temp}

  
  if (idx == 0):
    for i in range(cache_items - 1):
      data.append(init_point)
  data.append(data_point)

  # If the list has more than cache_items elements, remove the oldest data point
  if len(data) > cache_items:
      data.pop(0)

  # serialize cpu percent into ascii string
  try:
    # Open serial port
    ser = serial.Serial('/dev/ttyACM0', 115200)
    ascii_cpu = "".join([chr(int(obj["cpu_percent"]) + 33) for obj in data])
    cpu_info = "cpu:CPU_" + str(max_threads) + "_threads "
    ser.write((cpu_info + ascii_cpu + chr(0)).encode('utf-8'))
    print("cpu " + str(len(ascii_cpu.encode('utf-8'))) + " " + ascii_cpu[-10:])
    time.sleep(1)

    ascii_mem = "".join([chr(int(obj["memory_percent"]) + 33) for obj in data])
    mem_info = "mem:RAM_" + str(int(max_ram_gb)) + "GB "
    ser.write((mem_info + ascii_mem + chr(0)).encode('utf-8'))
    print("mem " + str(len(ascii_mem.encode('utf-8'))) + " "  + ascii_mem[-10:])
    time.sleep(1)

    ascii_disk_read = "".join([chr(int(obj["disk_read_percent"]) + 33) for obj in data])
    disk_read_info = "disk_read:HDD_read_12gbps "
    ser.write((disk_read_info + ascii_disk_read + chr(0)).encode('utf-8'))
    print("disk_read " + str(len(ascii_disk_read.encode('utf-8'))) + " "  + ascii_disk_read[-10:])
    time.sleep(1)

    ascii_disk_write = "".join([chr(int(obj["disk_write_percent"]) + 33) for obj in data])
    disk_write_info = "disk_write:write "
    ser.write((disk_write_info + ascii_disk_write + chr(0)).encode('utf-8'))
    print("disk_write " + str(len(ascii_disk_write.encode('utf-8'))) + " "  + ascii_disk_write[-10:])
    time.sleep(1)

    ascii_network_sent = "".join([chr(int(obj["network_sent_percent"]) + 33) for obj in data])
    network_sent_info = "network_sent:sent "
    ser.write((network_sent_info + ascii_network_sent + chr(0)).encode('utf-8'))
    print("network_sent " + str(len(ascii_network_sent.encode('utf-8'))) + " "  + ascii_network_sent[-10:])
    time.sleep(1)

    ascii_network_recv = "".join([chr(int(obj["network_recv_percent"]) + 33) for obj in data])
    network_recv_info = "network_recv:Network_recvd_1Gbps "
    ser.write((network_recv_info + ascii_network_recv + chr(0)).encode('utf-8'))
    print("network_recv " + str(len(ascii_network_recv.encode('utf-8'))) + " "  + ascii_network_recv[-10:])
    time.sleep(1)

    ascii_temp = "".join([chr(int(obj["temp"]) + 33) for obj in data])
    temp_info = "temp:CPU_temp_C "
    ser.write((temp_info + ascii_temp + chr(0)).encode('utf-8'))
    print("temp " + str(len(ascii_temp.encode('utf-8'))) + " "  + ascii_temp[-10:])

    time.sleep(4)
    ser.close()
  except:
    print("Serial port error")
    time.sleep(10)

