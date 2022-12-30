### a module to collect stats from the server and send them to the server-monitor
### this module runs every 2 seconds and sends the data to the server-monitor
### it uses psutils to collect the data and stores it in memory for 5 minutes
### data are send to the server-monitor every 2 seconds
### the server monitor listens on /dev/acm0 device
### the collected data are:
### - cpu usage as a percentage
### - cpu usage as a percentage times the number of cpu cores
### - total number of cpu cores
### - cpu frequency
### - cpu temperature
### - cpu voltage
### - memory usage as a percentage
### - memory usage in bytes
### - total memory in bytes
### - disk usage
### - total disk space
### - network usage in bytes incoming
### - network usage in bytes outgoing
### - temperature
### - uptime

import psutil
import time
import os
import sys
import serial
import json
import datetime

### read from file the number of seconds to collect data
### the data are stored in memory for this number of seconds
with open("stats.conf") as f:

# the number of seconds to collect data
# the data are stored in memory for this number of seconds
