@echo off
py -m http.server 3377 -d ../doxy_output/html/ --bind 127.0.0.1
