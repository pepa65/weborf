# User friendly webserver

## qweborf
Provides a GUI to share local files.

It can do NAT traversal to share files outside of the local network.

Can enable authentication and sending directories as `.tar.gz` files.

## weborf 0.13.5a
The web server used by qweborf.

Can be used from inetd, supports WebDAV, caching, CGI, virtual hosts.

### Compile
**These are the steps to compile weborf:**
```
autoreconf -fi  # Only if you cloned from git

make clean  # Only after you've done ./configure at least once

./configure

make

sudo make install  # Only if wanting to install
```

See INSTALL as well.

**To compile qweborf:**
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

