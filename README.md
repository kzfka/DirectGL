## Purpose of DirectGL
The purpose of DirectGL was to wrap [Win32](https://learn.microsoft.com/en-us/windows/win32/) and [Direct2D](https://learn.microsoft.com/en-us/windows/win32/direct2d/direct2d-portal) APIs into a single, OOP based, simple to use library.

## Remarks
In order to compile an application that uses DirectGL you should link the following libraries:
  - libd2d1.a
  - libdwrite.a
  - libwindowscodecs.a
  - libole32.a
