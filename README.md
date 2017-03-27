# User friendly webserver

## qweborf
Provides a GUI to share local files.

It can do NAT traversal to share files outside of the local network.

Can enable authentication and sending directories as `.tar.gz` files.

## weborf 0.13.5d
The web server used by qweborf.

Can be used from inetd, supports WebDAV, caching, CGI, virtual hosts.

### Compile

**To build weborf:**
See INSTALL

INSTALL can be sourced as a script as well:
```
 USAGE: . INSTALL [-i|-u|-h]
   (no option):  make and optionally install
            -i:  install
            -u:  uninstall
            -h:  this help text
```

**To build qweborf:**
```
pyuic5 qweborf/main.ui >qweborf/main.py
```

### Run
```
./weborf  # or when installed: weborf
```

*qweborf: will look for weborf in the PATH*
```
python3 -m qweborf
```

### Uninstall
```
sudo make uninstall
```

