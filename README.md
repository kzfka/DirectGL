## Purpose of DirectGL
The purpose of DirectGL was to wrap [Win32](https://learn.microsoft.com/en-us/windows/win32/) and [Direct2D](https://learn.microsoft.com/en-us/windows/win32/direct2d/direct2d-portal) APIs into a single, object oriented, simple to use library.

## Remarks
An application that uses DirectGL needs a few Windows specific libraries in order to be compiled and ran:
  - libd2d1.a
  - libdwrite.a
  - libwindowscodecs.a
  - libole32.a

In case if your code editor doesn't feature a linker, edit your compile line so it includes the following: `-ld2d1 -ldwrite -lwindowscodecs -lole32`.

## Extending the implementation
You can create your custom classes that represent various other elements. To make them compatible with the interface of DirectGL you have to use one of the following classes:
  - `std::GL::Drawable`,
  - `std::GL::Shape`
And optional:
  - `std::GL::Request`
