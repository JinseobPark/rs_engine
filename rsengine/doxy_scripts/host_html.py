"""
Script to host doxygen HTML files.
DO NOT USE THIS IN PRODUCTION

:Author: jupiterbjy@gmail.com
"""

import http.server
import socketserver
import socket
import pathlib
from os import chdir


PORT = 3377
PATH = pathlib.Path.cwd().parent / "doxy_output" / "html"
if not PATH.exists():
    raise FileNotFoundError("No doxygen html output")


def get_local_ip() -> str:
    """Gets local ip.
    
    Returns:
        host's local ip
    """
    
    return socket.gethostbyname(socket.gethostname())


def host_server():
    # cd into html output dir
    chdir(PATH)
    
    # get ip and print
    ip = get_local_ip()
    print(f"Hosting on http://{ip}:{PORT}")
    
    # run server
    handler = http.server.SimpleHTTPRequestHandler
    with socketserver.TCPServer((ip, PORT), handler) as httpd:
        httpd.serve_forever()


if __name__ == "__main__":
    host_server()
