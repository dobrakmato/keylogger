keystroke_coutner (Daemon)
-------------------------

Keylogger that writes all keystrokes to binary files stored on disk.

Binary log file contains:
- timestamp of each keystroke
- [virtual-key of each keystroke](https://msdn.microsoft.com/en-us/library/windows/desktop/dd375731(v=vs.85).aspx)
- name of process executable that was in foreground at the time when keystroke occurred

It is however compatible only with windows as it uses calls from `Windows.h` file.

Other applications can than analyze content of log file and extract interesting data for example:
- which keys do you use the most
- when you press keys
- in which applications do you type most
- and more...