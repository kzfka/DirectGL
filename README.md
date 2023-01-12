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
You can create your custom classes that represent various other elements. To make them compatible with the interface of DirectGL you have to derive from one of the following classes:
  - `std::GL::Drawable`
  - `std::GL::Shape`

And an optional one being `std::GL::Request`.

### std::GL::Drawable
This class represents a drawable element. Does not implement any default members, only a set of methods:
| Definition | Description |
| ---------- | ----------- |
| `static ID2D1HwndRenderTarget *std::GL::Drawable::getTarget()` | Returns the default render target of the window. |
| `static ID2D1Factory *std::GL::Drawable::getFactory()` | Returns the Direct2D factory. |
| `static float std::GL::Drawable::getWindowWidth()` | Returns window's width. |
| `static float std::GL::Drawable::getWindowHeight()` | Returns window's height. |
| `virtual void std::GL::Drwable::draw(ID2D1RenderTarget*)` | Meant to be overrided; should draw the element onto the given render target. |

### std::GL::Shape
This class represents a drawable (and fillable) element. Implements the same contents as `std::GL::Drawable` and following additional:
  - `std::GL::Color std::GL::Shape::color`
  - `void std::GL::Shape::fill(ID2D1RenderTarget*)`

### std::GL::Request
This class is a little bit tricky to explain. Implements the following methods:
  - `bool std::GL::Request::request()`
  - `virtual void std::GL::Request::onRequest()`

The first one, sends the request if possible, returns `true` if request was sent, `false` otherwise.
Second method is meant to be overrided and is executed whenever a sent request is processed. Each request is being processed before calling the `std::GL::Window::onCreate()` method.
