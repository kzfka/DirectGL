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
Represents a drawable element.
| Definition | Description |
| ---------- | ----------- |
| `static ID2D1HwndRenderTarget *std::GL::Drawable::getTarget()` | Returns the default render target of the window. |
| `static ID2D1Factory *std::GL::Drawable::getFactory()` | Returns the Direct2D factory. |
| `static float std::GL::Drawable::getWindowWidth()` | Returns window's width. |
| `static float std::GL::Drawable::getWindowHeight()` | Returns window's height. |
| `virtual void std::GL::Drwable::draw(ID2D1RenderTarget*)` | Meant to be overrided; should draw the element onto the given render target. |

### std::GL::Shape
Represents a drawable (and fillable) element.
| Definition | Description |
| ---------- | ----------- |
| `std::GL::Color std::GL::Shape::color` | Color of the shape. |
| `void std::GL::Shape::fill(ID2D1RenderTarget*)` | Meant to be overrided; should fill the element on the given render target. |

### std::GL::Request
Rrepresents a request.
| Definition | Description |
| ---------- | ----------- |
| `bool std::GL::Request::request()` | If possible, sends the request; returns `true` if request was actually sent, `false` otherwise. |
| `virtual void std::GL::Request::onRequest()` | Meant to be overrided; can handle the response to the request. |
