#ifndef DIRECTGL_INCLUDED
	#define DIRECTGL_INCLUDED

	#ifndef UNICODE
		#define UNICODE
		#define UNICODE_WAS_UNDEFINED
	#endif

	#include <map>
	#include <string>
	#include <math.h>
	#include <vector>

	#include <wincodec.h>
	#include <windows.h>
	#include <dwrite.h>
	#include <d2d1.h>

	#ifdef UNICODE_WAS_UNDEFINED
		#undef UNICODE
	#endif

	INT WINAPI WinMain(HINSTANCE, HINSTANCE, PSTR, INT);
	LRESULT CALLBACK WindowProc(HWND, UINT, WPARAM, LPARAM);

	namespace std
	{
		namespace GL
		{
			class Vertex
			{
				public:
					float x, y;

					Vertex(float x = 0, float y = 0)
					{
						this->x = x;
						this->y = y;
					}

					operator D2D1_POINT_2F()
					{return {x, y};}

					bool operator==(Vertex &rhs)
					{return (x == rhs.x && y == rhs.y);}

					bool operator!=(Vertex &rhs)
					{return !(*this == rhs);}

					Vertex offsetted(Vertex offset)
					{return {x + offset.x, y + offset.y};}

					float distance(Vertex vertex)
					{return sqrt(pow(x - vertex.x, 2) + pow(y - vertex.y, 2));}

					bool inside(Vertex start, Vertex end)
					{return x >= start.x && x <= end.x && y >= start.y && y <= end.y;}
			};

			class Color
			{
				public:
					float r, g, b, a;

					static Color fromRGBA(unsigned char r = 0, unsigned char g = 0, unsigned char b = 0, unsigned char a = 255)
					{return {(float)r / 255, (float)g / 255, (float)b / 255, (float)a / 255};}

					Color(float r = 0, float g = 0, float b = 0, float a = 1)
					{this->r = r; this->g = g; this->b = b; this->a = a;}

					Color(wstring hex)
					{
						float values[4]
						{0, 0, 0, 1};

						for(size_t i = 0; i < hex.size() && i < 8; i += 2)
						{values[i / 2] = ((float)stoi(hex.substr(i, 2), nullptr, 16)) / 255;}

						r = values[0];
						g = values[1];
						b = values[2];
						a = values[3];
					}

					Color(const wchar_t *hex)
					{*this = Color(wstring(hex));}

					operator D2D1_COLOR_F()
					{return {r, g, b, a};}

					bool operator==(Color &rhs)
					{return (r == rhs.r && g == rhs.g && b == rhs.b);}

					bool operator!=(Color rhs)
					{return !(*this == rhs);}
			};

			class Request
			{
				friend class Window;

				static inline vector<Request*> requests;
				static inline bool available = true;

				protected:
					bool request()
					{
						if(available)
						{requests.push_back(this); return true;}

						return false;
					}

					virtual void onRequest()
					{}
			};

			class Window *window;
			class Window
			{
				friend class Request;
				friend class Drawable;
				friend class Brush;

				friend LRESULT CALLBACK ::WindowProc(HWND, UINT, WPARAM, LPARAM);
				friend INT WINAPI ::WinMain(HINSTANCE, HINSTANCE, PSTR, INT);

				class Resources
				{
					public:
						static inline map<wstring, ID2D1Bitmap*> bitmaps = map<wstring, ID2D1Bitmap*>();

						~Resources()
						{
							for(map<wstring, ID2D1Bitmap*>::iterator pair = bitmaps.begin(); pair != bitmaps.end(); ++pair)
							{pair->second->Release();}
						}
				} resources;

				wstring title = L"DirectGL";
				size_t width = 640;
				size_t height = 480;

				HWND handle;
				ID2D1Factory *factory;
				ID2D1BitmapRenderTarget *target;
				ID2D1HwndRenderTarget *windowTarget;

				bool key[256];

				void create()
				{
					RECT rect; GetClientRect(handle, &rect);
					
					CoInitializeEx(nullptr, COINITBASE_MULTITHREADED);
					D2D1CreateFactory(D2D1_FACTORY_TYPE_MULTI_THREADED, &factory);

					factory->CreateHwndRenderTarget(D2D1::RenderTargetProperties(), D2D1::HwndRenderTargetProperties(handle, D2D1::SizeU(rect.right, rect.bottom)), &windowTarget);
					windowTarget->CreateCompatibleRenderTarget(&target);

					for(size_t i = 0; i < Request::requests.size(); i++)
					{Request::requests[i]->onRequest();} Request::requests.clear(); Request::available = false;
					
					target->BeginDraw();
					onCreate();
				}

				void update()
				{
					ID2D1Bitmap *bitmap;

					onUpdate();

					target->EndDraw();
					windowTarget->BeginDraw();

					target->GetBitmap(&bitmap);
					windowTarget->DrawBitmap(bitmap, {0, 0, (float)width, (float)height}, 1, D2D1_BITMAP_INTERPOLATION_MODE_NEAREST_NEIGHBOR, {0, 0, (float)width, (float)height});

					windowTarget->EndDraw();
					target->BeginDraw();
				}

				void setHeld(unsigned char key, bool held)
				{
					if(key >= 0 && key < 256)
					{this->key[key] = held;}
				}

				public:
					enum Key
					{
						leftButton = 0x01, rightButton, middleButton = 0x04, xButton1, xButton2, backspace = 0x08, tabulator,
						enter = 0x0d, shift = 0x10, control, alternative, capsLock = 0x14, escape = 0x1b, f1 = 0x70, f2, f3, f4,
						f5, f6, f7, f8, f9, f10, f11, f12, f13, f14, f15, f16, f17, f18, f19, f20, f21, f22, f23, f25, numLock = 0x90,
						scrollLock, spacebar = 0x20, pageUp, pageDown, end, home, plus = 0xbb, comma, dash, dot, slash, tilde,
						leftBracket = 0xdb, backslash, rightBracket, quote, leftArrow = 0x25, upArrow, rightArrow, downArrow, _0 = 0x30,
						_1, _2, _3, _4, _5, _6, _7, _8, _9, numpad0 = 0x60, numpad1, numpad2, numpad3, numpad4, numpad5, numpad6, numpad7,
						numpad8, numpad9, a = 0x41, b, c, d, e, f, g, h, i, j, k, l, m, n, o, p, q, r, s, t, u, v, w, x, y, z
					};

					enum Direction
					{
						forwards = 7864320,
						backwards = 4287102976
					};

					Window()
					{window = this;}
					
					Window(wstring title, size_t width, size_t height)
					{
						this->title = title;
						this->width = width;
						this->height = height;

						window = this;
					}

					~Window()
					{
						target->EndDraw();
						factory->Release();
						target->Release();
					}

					bool isHeld(Key key)
					{return this->key[key];}

					wstring getTitle()
					{return title;}

					size_t getWidth()
					{return width;}

					size_t getHeight()
					{return height;}

					Vertex getMousePos()
					{POINT mouse; GetCursorPos(&mouse); ScreenToClient(handle, &mouse); return {(float)mouse.x, (float)mouse.y};}

					void setMousePos(Vertex vertex)
					{POINT mouse {(long int)vertex.x, (long int)vertex.y}; ScreenToClient(handle, &mouse); SetCursorPos(mouse.x, mouse.y);}

					void clear(Color color = L"")
					{target->Clear(color);}

					void destroy()
					{DestroyWindow(handle); exit(0);}

					virtual void onCreate()
					{}

					virtual void onUpdate()
					{}

					virtual void onDestroy()
					{}

					virtual void onKeystroke(Key)
					{}

					virtual void onScroll(Direction)
					{}
			};

			class Drawable
			{
				protected:
					class Brush
					{
						ID2D1SolidColorBrush *brush;

						public:
							Brush(Color color = L"")
							{window->target->CreateSolidColorBrush(color, &brush);}

							~Brush()
							{brush->Release();}

							operator ID2D1SolidColorBrush*()
							{return brush;}
					};

					class Font
					{
						IDWriteFactory *factory;
						IDWriteTextFormat *format;

						public:
							Font(float fontSize, wstring fontFamily)
							{
								DWriteCreateFactory(DWRITE_FACTORY_TYPE_SHARED, __uuidof(factory), (IUnknown**)&factory);
								factory->CreateTextFormat(fontFamily.c_str(), nullptr, DWRITE_FONT_WEIGHT_NORMAL, DWRITE_FONT_STYLE_NORMAL, DWRITE_FONT_STRETCH_NORMAL, fontSize, L"", &format);
							}

							~Font()
							{
								format->Release();
								factory->Release();
							}

							operator IDWriteTextFormat*()
							{return format;}
					};

					static ID2D1BitmapRenderTarget *getTarget()
					{return window->target;}

					static ID2D1Factory *getFactory()
					{return window->factory;}

					static Window::Resources *getResources()
					{return &window->resources;}

					static float getWindowWidth()
					{return window->getWidth();}

					static float getWindowHeight()
					{return window->getHeight();}

				public:
					virtual void draw(ID2D1RenderTarget *target)
					{}
			};

			class Shape : public Drawable
			{
				public:
					Color color;

					virtual void fill(ID2D1RenderTarget *target)
					{}
			};

			class Text : public Drawable
			{
				public:
					Vertex vertex;
					wstring text, fontFamily;
					float fontSize;
					Color color;

					Text(Vertex vertex = {}, wstring text = L"", Color color = L"", float fontSize = 11, wstring fontFamily = L"Arial")
					{
						this->vertex = vertex;
						this->text = text;
						this->fontSize = fontSize;
						this->fontFamily = fontFamily;
						this->color = color;
					}

					~Text()
					{}

					void draw(ID2D1RenderTarget *target = getTarget())
					{target->DrawTextW(text.c_str(), text.size(), Font(fontSize, fontFamily), {vertex.x, vertex.y, getWindowWidth(), getWindowHeight()}, Brush(color));}
			};

			class Line : public Drawable
			{
				public:
					Vertex start, end;
					Color color;
					float thickness;

					Line(Vertex start = {}, Vertex end = {}, Color color = L"", float thickness = 1)
					{
						this->start = start;
						this->end = end;
						this->color = color;
						this->thickness = thickness;
					}

					void draw(ID2D1RenderTarget *target = getTarget())
					{target->DrawLine(start, end, Brush(color), thickness);}
			};

			class Rectangle : public Shape
			{
				public:
					Vertex vertex, size;

					Rectangle(Vertex start = {}, Vertex size = {}, Color color = L"")
					{
						this->vertex = start;
						this->size = size;
						this->color = color;
					}

					void draw(ID2D1RenderTarget *target = getTarget())
					{target->DrawRectangle({vertex.x, vertex.y, vertex.x + size.x, vertex.y + size.y}, Brush(color));}

					void fill(ID2D1RenderTarget *target = getTarget())
					{target->FillRectangle({vertex.x, vertex.y, vertex.x + size.x, vertex.y + size.y}, Brush(color));}
			};

			class Ellipse : public Shape
			{
				public:
					Vertex vertex, size;

					Ellipse(Vertex vertex = {}, Vertex size = {}, Color color = L"")
					{
						this->vertex = vertex;
						this->size = size;
						this->color = color;
					}

					void draw(ID2D1RenderTarget *target = getTarget())
					{target->DrawEllipse({vertex.x - size.x / 2, vertex.y - size.y / 2, size.x, size.y}, Brush(color));}

					void fill(ID2D1RenderTarget *target = getTarget())
					{target->FillEllipse({vertex.x - size.x / 2, vertex.y - size.y / 2, size.x, size.y}, Brush(color));}
			};

			class Polygon : public Shape
			{
				public:
					vector<Vertex> vertices;

					Polygon(vector<Vertex> vertices, Color color)
					{
						this->vertices = vertices;
						this->color = color;
					}

					void draw(ID2D1RenderTarget *target = getTarget())
					{
						ID2D1PathGeometry *geometry; getFactory()->CreatePathGeometry(&geometry);
						ID2D1GeometrySink *sink; geometry->Open(&sink);
						
						sink->BeginFigure(vertices[0], D2D1_FIGURE_BEGIN_HOLLOW);
						sink->AddLines((D2D1_POINT_2F*)vertices.begin().base(), vertices.size());
						sink->EndFigure(D2D1_FIGURE_END_CLOSED);

						sink->Close();
						target->DrawGeometry(geometry, Brush(color));

						sink->Release();
						geometry->Release();
					}

					void fill(ID2D1RenderTarget *target = getTarget())
					{
						ID2D1PathGeometry *geometry; getFactory()->CreatePathGeometry(&geometry);
						ID2D1GeometrySink *sink; geometry->Open(&sink);
						
						sink->BeginFigure(vertices[0], D2D1_FIGURE_BEGIN_FILLED);
						sink->AddLines((D2D1_POINT_2F*)vertices.begin().base(), vertices.size());
						sink->EndFigure(D2D1_FIGURE_END_CLOSED);

						sink->Close();
						target->FillGeometry(geometry, Brush(color));

						sink->Release();
						geometry->Release();
					}
			};

			class Bitmap : public Drawable, public Request
			{
				wstring filePath;
				size_t width = 0, height = 0;
				vector<Color> colors = vector<Color>();
	
				void preLoad(wstring filePath)
				{
					if(getResources()->bitmaps.find(filePath) == getResources()->bitmaps.end())
					{
						IWICImagingFactory *factory; CoCreateInstance(CLSID_WICImagingFactory, nullptr, CLSCTX_INPROC_SERVER, IID_IWICImagingFactory, (void**)&factory);
						IWICBitmapDecoder *decoder; factory->CreateDecoderFromFilename(filePath.c_str(), nullptr, GENERIC_READ, WICDecodeMetadataCacheOnLoad, &decoder);
						IWICBitmapFrameDecode *frame; decoder->GetFrame(0, &frame); frame->GetSize((UINT*)&width, (UINT*)&height);
						IWICFormatConverter *converter; factory->CreateFormatConverter(&converter);
						ID2D1Bitmap *bitmap;
						
						converter->Initialize(frame, GUID_WICPixelFormat32bppPBGRA, WICBitmapDitherTypeNone, nullptr, 0, WICBitmapPaletteTypeMedianCut);
						getTarget()->CreateBitmapFromWicBitmap(converter, &bitmap); getResources()->bitmaps[filePath] = bitmap;

						BYTE *bytes = new BYTE[width * height * 4];
						frame->CopyPixels(nullptr, width * 4, width * height * 4, bytes);

						for(size_t i = 0; i < width * height * 4; i += 4)
						{colors.push_back(Color::fromRGBA(*(bytes + i + 2), *(bytes + i + 1), *(bytes + i), *(bytes + i + 3)));}

						delete []bytes;
						converter->Release();
						frame->Release();
						decoder->Release();
						factory->Release();
					}
				}

				public:
					Vertex vertex;

					Bitmap(Vertex vertex = {})
					{this->vertex = vertex;}

					Bitmap(Vertex vertex, wstring filePath)
					{
						this->vertex = vertex;
						this->filePath = filePath;

						if(!request())
						{preLoad(filePath);}
					}
					
					void onRequest()
					{preLoad(filePath);}

					size_t getWidth()
					{return width;}

					size_t getHeight()
					{return height;}

					Color getColor(Vertex vertex)
					{return colors[vertex.y * width + vertex.x];}

					void draw(ID2D1RenderTarget *target = getTarget())
					{target->DrawBitmap(getResources()->bitmaps[filePath], {vertex.x, vertex.y, vertex.x + width, vertex.y + height}, 1, D2D1_BITMAP_INTERPOLATION_MODE::D2D1_BITMAP_INTERPOLATION_MODE_NEAREST_NEIGHBOR, {0, 0, (float)width, (float)height});}
			};

			class Layer : public Drawable, public Request
			{
				ID2D1BitmapRenderTarget *target = nullptr;

				public:
					Layer()
					{
						if(!request())
						{onRequest();}
					}

					~Layer()
					{
						if(target)
						{target->EndDraw(); target->Release(); target = nullptr;}
					}

					operator ID2D1RenderTarget*()
					{return target;}

					void onRequest()
					{getTarget()->CreateCompatibleRenderTarget(&target); target->BeginDraw();}

					void clear(Color color = L"")
					{target->Clear(color);}

					void draw()
					{
						ID2D1Bitmap *bitmap;

						target->EndDraw();
						target->GetBitmap(&bitmap);

						getTarget()->DrawBitmap(bitmap, {0, 0, getWindowWidth(), getWindowHeight()}, 1, D2D1_BITMAP_INTERPOLATION_MODE_NEAREST_NEIGHBOR, {0, 0, getWindowWidth(), getWindowHeight()});

						bitmap->Release();
						target->BeginDraw();
					}
			};
		}
	}

	INT WINAPI WinMain(HINSTANCE instance_handle, HINSTANCE previous_instance_handle, PSTR cmd_line, INT show_cmd)
	{
		MSG message = {};
		WNDCLASS window_class = {};

		window_class.lpfnWndProc = WindowProc;
		window_class.hInstance = instance_handle;
		window_class.lpszClassName = L"DIRECTGL_WINDOW";

		RegisterClassW(&window_class);

		std::GL::window->handle = CreateWindowExW
		(
			0,
			window_class.lpszClassName,
			std::GL::window->title.c_str(),
			WS_OVERLAPPED | WS_CAPTION | WS_SYSMENU | WS_MINIMIZEBOX,
			CW_USEDEFAULT,
			CW_USEDEFAULT,
			std::GL::window->width + 5,
			std::GL::window->height + 29,
			nullptr,
			nullptr,
			instance_handle,
			nullptr
		);
		
		std::GL::window->create();

		ShowWindow(std::GL::window->handle, show_cmd);
		ShowWindow(GetConsoleWindow(), SW_HIDE);
		UpdateWindow(std::GL::window->handle);

		while(GetMessageW(&message, nullptr, 0, 0) > 0)
		{TranslateMessage(&message); DispatchMessageW(&message);}

		return 0;
	}

	LRESULT CALLBACK WindowProc(HWND window_handle, UINT message, WPARAM message_parameter0, LPARAM message_parameter1)
	{
		switch(message)
		{
			case WM_DESTROY: std::GL::window->onDestroy(); PostQuitMessage(0); return 0;
			case WM_PAINT:  std::GL::window->update(); return 0;
			
			case WM_KEYDOWN: std::GL::window->setHeld(message_parameter0, true); std::GL::window->onKeystroke((std::GL::Window::Key)message_parameter0); return 0;
			case WM_LBUTTONDOWN: std::GL::window->key[VK_LBUTTON] = true; std::GL::window->onKeystroke((std::GL::Window::Key)VK_LBUTTON); return 0;
			case WM_MBUTTONDOWN: std::GL::window->key[VK_MBUTTON] = true; std::GL::window->onKeystroke((std::GL::Window::Key)VK_MBUTTON); return 0;
			case WM_RBUTTONDOWN: std::GL::window->key[VK_RBUTTON] = true; std::GL::window->onKeystroke((std::GL::Window::Key)VK_RBUTTON); return 0;
			case WM_XBUTTONDOWN:
				switch(message_parameter0)
				{
					case XBUTTON1: std::GL::window->key[VK_XBUTTON1] = true; std::GL::window->onKeystroke((std::GL::Window::Key)VK_XBUTTON1); return 0;
					case XBUTTON2: std::GL::window->key[VK_XBUTTON2] = true; std::GL::window->onKeystroke((std::GL::Window::Key)VK_XBUTTON2); return 0;
				}

			case WM_KEYUP: std::GL::window->setHeld(message_parameter0, false); return 0;
			case WM_LBUTTONUP: std::GL::window->key[VK_LBUTTON] = false; return 0;
			case WM_MBUTTONUP: std::GL::window->key[VK_MBUTTON] = false; return 0;
			case WM_RBUTTONUP: std::GL::window->key[VK_RBUTTON] = false; return 0;
			case WM_XBUTTONUP:
				switch(message_parameter0)
				{
					case XBUTTON1: std::GL::window->key[VK_XBUTTON1] = false; return 0;
					case XBUTTON2: std::GL::window->key[VK_XBUTTON2] = false; return 0;
				}

			case WM_MOUSEWHEEL: std::GL::window->onScroll((std::GL::Window::Direction)message_parameter0); return 0;
		}

		return DefWindowProcW(window_handle, message, message_parameter0, message_parameter1);
	}
#endif