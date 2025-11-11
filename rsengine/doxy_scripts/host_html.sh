#!/bin/bash
# Script for doxygen html host. Any passed param will be passed to python http.server.
# Refer following for available param: https://docs.python.org/ko/3/library/http.server.html
# 
# :Author: jupiterbjy@gmail.com

echo "[HostHTML] Starting Doxygen html server"

echo Hosting at: http://$(ip addr show eth0 | grep "inet\b" | awk '{print $2}' | cut -d/ -f1):8000

python3 -m http.server 8000 -d ../doxy_output/html/ "$@"
