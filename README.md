## Purpose of DirectGL
The purpose of DirectGL was to wrap [Win32](https://learn.microsoft.com/en-us/windows/win32/) and [Direct2D](https://learn.microsoft.com/en-us/windows/win32/direct2d/direct2d-portal) APIs into a single, OOP based, simple to use library.

## Remarks
An application that uses DirectGL needs a few Windows specific libraries in order to be compiled and ran:
  - libd2d1.a
  - libdwrite.a
  - libwindowscodecs.a
  - libole32.a
