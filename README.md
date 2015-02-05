## Wysiwidi - Miracast & WiDi library

Wysiwidi is a set of libraries for developers who want to build Miracast/WiDi-enabled applications on linux. It is still an unreleased work-in-progress but can be tested already.

Wysiwidi consists of:
 * _wysiwidi-wfd:_ Main library that implements the Miracast-dialect of RTSP including the parser, actual negotiation logic for sink and source, and the related data structures. wfd is not tied to a specific connection manager, media framework or main loop.
 * _wysiwidi-network:_ Integration with GLib main loop and GStreamer
 * _wysiwidi-p2p:_ Integration with Connman Wifi P2P features.

The source code includes example implementations:
 * _sink:_ Miracast sink that depends on Gstreamer, Connman and GLib mainloop
 * _desktop_source:_ Miracast source that depends on Gstreamer, Connman and GLib mainloop

More information can be found on the [mailing list](https://lists.01.org/mailman/listinfo/wysiwidi-dev) and the [wiki](https://github.com/01org/wysiwidi/wiki).

### Requirements:

wysiwidi test executables have runtime dependencies on just a few things (mostly GStreamer and GLib), but for successful Miracast sessions the following are adviced:
 * Wifi adapter from Intel 7260-family
 * [wpa_supplicant](http://w1.fi/wpa_supplicant/): master branch checked out after Dec 2014, built with	`CONFIG_P2P=y`, `CONFIG_WIFI_DISPLAY=y` and `CONFIG_CTRL_IFACE_DBUS_NEW=y`
 * [connman](https://01.org/connman): master branch checked out after Jan 26th 2015 (499a424d)
 * gstreamer: either master branch more recent than Feb 3rd 2015 (commit d0a50be2), or 1.4 branch more recent than 
Feb 3rd 2005 (commit 1ce3260a638d or release 1.4.6 or later).

Test results with other Wifi adapters are very welcome but be warned that in many cases Wifi-P2P has not had the testing it needs on linux: you may run into problems in surprising places.

### Building Wysiwidi from git:

```
cmake .
make
```

### Testing Wysiwidi

#### Pre-requisites

Make sure wpa_supplicant & connmand are running. Running both of them uninstalled is possible (but in that case make sure the system wpa_supplicant and system connection manager are _not_ running):

```
$ sudo hostap/wpa_supplicant/wpa_supplicant -ddt -u

$ sudo connman/src/connmand -n -d
```

Use connmanctl to enable Wi-Fi and Wi-Fi P2P:

```
$ connman/client/connmanctl
connmanctl> enable wifi
Enabled wifi
connmanctl> enable p2p
Enabled p2p
```

Check that Peer-to-peer functionality is working:

```
connmanctl> scan p2p
Scan completed for p2p
connmanctl> peers
Nexus 5 (jku) idle peer_0c8bfd5f12fc_8ac9d0c0da67
Push2TV 75AEB2-PTV3000 idle peer_0c8bfd5f12fc_22e52a75aeb2

```

Enable agent:

```
connmanctl> agent on
Agent registered
```

#### Testing Wysiwidi sink with an Android device (e.g. Nexus 5):

* start sink: `sink/sink-test`
* Android: select the sink from the list in "Settings > Display > Cast Screen"
* connmanctl: when agent asks, accept the connection

In a few seconds, a window should open and a audio/video stream should start playing.

#### Testing Wysiwidi source with another sink:

* start source: `desktop_source/desktop_source`
* Initial scan will be automatic, but `scan` command will re-scan.
* After getting scan results, connect with `connect N` (where N is number from scan result)
* connmanctl: when agent asks, accept the connection

In a few seconds, the sink should start showing a stream mirroring your desktop.

