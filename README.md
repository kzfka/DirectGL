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
You can create your custom classes that represent various elements. DirectGL provides a couple of abstract classes, so you can interact with the library.


### std::GL::Drawable
Represents a drawable element.
| Modifier | Type | Definition | Description |
| -------- | ---- | ---------- | ----------- |
| static | ID2D1HwndRenderTarget*| getTarget() | Returns the default render target of the window. |
| static | ID2D1Factory*| getFactory() | Returns the Direct2D factory. |
| static | float | getWindowWidth() | Returns window's width. |
| static | float | getWindowHeight() | Returns window's height. |
| virtual | void | draw(ID2D1RenderTarget*) | Meant to be overrided; should draw the element onto the given render target. |

### std::GL::Shape
Represents a drawable (and fillable) element.
| Modifier | Type | Definition | Description |
| -------- | ---- | ---------- | ----------- |
| | std::GL::Color | color | Color of the shape. |
| | void | fill(ID2D1RenderTarget*) | Meant to be overrided; should fill the element on the given render target. |

### std::GL::Request
Rrepresents a request.
| Modifier | Type | Definition | Description |
| -------- | ---- | ---------- | ----------- |
| | bool | request() | If possible, sends the request; returns `true` if request was actually sent, `false` otherwise. |
| virtual | void | onRequest() | Meant to be overrided; can handle the response to the request. |
