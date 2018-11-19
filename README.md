# weborf 
**A userfriendly file/directory/webserver**

https://gitlab.com/pepa65/weborf

## qweborf
Provides a GUI to share local files.

It can do NAT traversal to share files outside of the local network.

Can enable authentication and sending directories as `.tar.gz` files.

## weborf 0.13.5g
The web server used by qweborf.

Can be used from inetd, supports WebDAV, caching, CGI, virtual hosts.

### Compile

**Build weborf**

See INSTALL, which can be sourced as a script as well. Usage:
```
source INSTALL [-i|-u|-h]
    (no option):  make and optionally install
             -i:  install
             -u:  uninstall
             -h:  this help text
```

**Build qweborf**

`pyuic5 qweborf/main.ui >qweborf/main.py`  # requires package `pyqt5-dev-tools`

### Run
`./weborf`  # or when installed (in PATH): `weborf`

`qweborf`  # will look for weborf in PATH

`python3 -m qweborf`  # or provide the path to `qweborf`

### Uninstall
`sudo make uninstall`
