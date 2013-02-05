simpleimagewrap
===============

Simpleimagewrap is an INI based disc image wrapper, with the very simple purpose of mounting an image, launching an application, and unmounting the image when the application exits.

Configuration
---------------

Configuration is done entirely via an INI file matching the name of the executable itself. If you rename the executable "ssf_wrap.exe", you should have an INI file named "ssf_wrap.ini" in the same directory. The INI itself is very basic, and an example that uses SSF and DTLite looks like:

```
[application]
mounter    = daemontools ; daemontools or virtualclonedrive                 *required
executable = .\SSF.exe   ; the executable to run after the image is mounted *required
sleep      = 2           ; highly recommend a value of at least 2           *optional

[daemontools]
type       = dt                                                  ; dt or scsi, the type of DT virtual drive
drive      = 0                                                   ; virtual drive number, these start at 0 for daemon tools
path       = C:\Program Files (x86)\DAEMON Tools Lite\DTLite.exe ; path to DTLite.exe
```

When using this with an emulator front-end, you would select "ssf_wrap.exe" as the emulator and have the disc image as the argument.
