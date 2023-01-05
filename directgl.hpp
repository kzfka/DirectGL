#ifndef DIRECTGL_INCLUDED
	#define DIRECTGL_INCLUDED

	#pragma comment(lib, "libd2d1.a")
	#pragma comment(lib, "libdwrite.a")
	#pragma comment(lib, "libwindowscodecs.a")
	#pragma comment(lib, "libole32.a")

	#ifndef UNICODE
		#define UNICODE
		#define UNICODE_WAS_UNDEFINED
	#endif

	#include <string>
	#include <math.h>
	#include <vector>
	#include <stdlib.h>

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
			typedef unsigned char Key;
			typedef unsigned short Size;

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

					static Color random()
					{return {(float)(rand() % 256) / 255, (float)(rand() % 256) / 255, (float)(rand() % 256) / 255};}

					Color(float r = 0, float g = 0, float b = 0, float a = 1)
					{this->r = r; this->g = g; this->b = b; this->a = a;}

					Color(wstring hex)
					{
						float values[4]
						{0, 0, 0, 1};

						for(size_t i = 0; i < hex.size() && i < 8; i += 2)
						{values[i / 2] = ((float)stoi(hex.substr(i, 2), NULL, 16)) / 255;}

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

			class Window *window;
			class Window
			{
				friend class Drawable;
				friend class Direct2DBrush;

				friend LRESULT CALLBACK ::WindowProc(HWND, UINT, WPARAM, LPARAM);
				friend INT WINAPI ::WinMain(HINSTANCE, HINSTANCE, PSTR, INT);

				wstring title = L"GL";
				short int width = 640;
				short int height = 480;

				HWND handle;
				ID2D1Factory *factory;
				ID2D1HwndRenderTarget *target;

				void create()
				{
					RECT rect; GetClientRect(handle, &rect);

					CoInitializeEx(nullptr, COINITBASE_MULTITHREADED);
					D2D1CreateFactory(D2D1_FACTORY_TYPE_MULTI_THREADED, &factory);
					factory->CreateHwndRenderTarget(D2D1::RenderTargetProperties(), D2D1::HwndRenderTargetProperties(handle, D2D1::SizeU(rect.right, rect.bottom)), &target);

					onCreate();
				}

				void update()
				{
					target->BeginDraw();
					onUpdate();
					target->EndDraw();
				}

				bool key[256];

				public:
					enum
					{
						leftButton = 0x01, rightButton, middleButton = 0x04, xButton1, xButton2, backspace = 0x08, tabulator,
						enter = 0x0d, shift = 0x10, control, alternative, capsLock = 0x14, escape = 0x1b, f1 = 0x70, f2, f3, f4,
						f5, f6, f7, f8, f9, f10, f11, f12, f13, f14, f15, f16, f17, f18, f19, f20, f21, f22, f23, f25, numLock = 0x90,
						scrollLock, spacebar = 0x20, pageUp, pageDown, end, home, plus = 0xbb, comma, dash, dot, slash, tilde,
						leftBracket = 0xdb, backslash, rightBracket, quote, leftArrow = 0x25, upArrow, rightArrow, downArrow, _0 = 0x30,
						_1, _2, _3, _4, _5, _6, _7, _8, _9, numpad0 = 0x60, numpad1, numpad2, numpad3, numpad4, numpad5, numpad6, numpad7,
						numpad8, numpad9, a = 0x41, b, c, d, e, f, g, h, i, j, k, l, m, n, o, p, q, r, s, t, u, v, w, x, y, z
					};
					
					Window()
					{window = this;}

					~Window()
					{
						factory->Release();
						target->Release();
						CoUninitialize();
					}

					wstring getTitle()
					{return title;}

					Size getWidth()
					{return width;}

					Size getHeight()
					{return height;}

					Vertex getMousePos()
					{POINT mouse; GetCursorPos(&mouse); ScreenToClient(handle, &mouse); return {(float)mouse.x, (float)mouse.y};}

					void setTitle(wstring title)
					{this->title = title; SetWindowTextW(handle, title.c_str());}

					void setWidth(Size width)
					{this->width = width; SetWindowPos(handle, handle, 0, 0, width, height, SWP_NOMOVE); target->Resize(D2D1::SizeU(width, height));}

					void setHeight(Size height)
					{this->height = height; SetWindowPos(handle, handle, 0, 0, width, height, SWP_NOMOVE); target->Resize(D2D1::SizeU(width, height));}

					void setMousePos(Vertex vertex)
					{POINT mouse {(long int)vertex.x, (long int)vertex.y}; ScreenToClient(handle, &mouse); SetCursorPos(mouse.x, mouse.y);}

					bool isHeld(Key key)
					{return this->key[key];}

					void clear(Color color = L"")
					{target->Clear(color);}

					void destroy()
					{DestroyWindow(handle); exit(0);}

					virtual void onCreate()
					{}

					virtual void onUpdate()
					{clear();}

					virtual void onDestroy()
					{}

					virtual void onKeystroke(unsigned char)
					{}
			};

			class Direct2DBrush
			{
				ID2D1SolidColorBrush *brush;

				public:
					Direct2DBrush(Color color = L"")
					{window->target->CreateSolidColorBrush(color, &brush);}

					~Direct2DBrush()
					{brush->Release();}

					operator ID2D1SolidColorBrush*()
					{return brush;}
			};

			class Direct2DFont
			{
				IDWriteFactory *factory;
				IDWriteTextFormat *format;

				public:
					Direct2DFont(wstring fontName, float fontSize)
					{
						DWriteCreateFactory(DWRITE_FACTORY_TYPE_SHARED, __uuidof(factory), (IUnknown**)&factory);
						factory->CreateTextFormat(fontName.c_str(), NULL, DWRITE_FONT_WEIGHT_NORMAL, DWRITE_FONT_STYLE_NORMAL, DWRITE_FONT_STRETCH_NORMAL, fontSize, L"", &format);
					}

					~Direct2DFont()
					{
						format->Release();
						factory->Release();
					}

					operator IDWriteTextFormat*()
					{return format;}
			};

			class Drawable
			{
				protected:
					ID2D1HwndRenderTarget *getTarget()
					{return window->target;}

					ID2D1Factory *getFactory()
					{return window->factory;}

					float getWindowWidth()
					{return window->getWidth();}

					float getWindowHeight()
					{return window->getHeight();}

				public:
					virtual void draw()
					{}
			};

			class Shape : public Drawable
			{
				public:
					Color color;

					virtual void fill()
					{}
			};

			class Text : public Drawable
			{
				public:
					Vertex vertex;
					wstring text, fontName;
					float fontSize;
					Color color;

					Text(Vertex vertex = {}, wstring text = L"", Color color = L"", wstring fontName = L"Verdana", float fontSize = 11)
					{
						this->vertex = vertex;
						this->text = text;
						this->fontName = fontName;
						this->fontSize = fontSize;
						this->color = color;
					}

					~Text()
					{}

					void draw()
					{getTarget()->DrawTextW(text.c_str(), text.size(), Direct2DFont(fontName, fontSize), {vertex.x, vertex.y, getWindowWidth(), getWindowHeight()}, Direct2DBrush(color));}
			};

			class Line : public Drawable
			{
				public:
					Vertex start, end;
					Color color;

					Line(Vertex start = {}, Vertex end = {}, Color color = L"")
					{
						this->start = start;
						this->end = end;
						this->color = color;
					}

					void draw()
					{getTarget()->DrawLine(start, end, Direct2DBrush(color));}
			};

			class Rectangle : public Shape
			{
				public:
					Vertex start, size;

					Rectangle(Vertex start = {}, Vertex size = {}, Color color = L"")
					{
						this->start = start;
						this->size = size;
						this->color = color;
					}

					void draw()
					{getTarget()->DrawRectangle({start.x, start.y, start.x + size.x, start.y + size.y}, Direct2DBrush(color));}

					void fill()
					{getTarget()->FillRectangle({start.x, start.y, start.x + size.x, start.y + size.y}, Direct2DBrush(color));}
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

					void draw()
					{getTarget()->DrawEllipse({vertex.x - size.x / 2, vertex.y - size.y / 2, size.x, size.y}, Direct2DBrush(color));}

					void fill()
					{getTarget()->FillEllipse({vertex.x - size.x / 2, vertex.y - size.y / 2, size.x, size.y}, Direct2DBrush(color));}
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

					void draw()
					{
						ID2D1PathGeometry *geometry; getFactory()->CreatePathGeometry(&geometry);
						ID2D1GeometrySink *sink; geometry->Open(&sink);
						
						sink->BeginFigure(vertices[0], D2D1_FIGURE_BEGIN_HOLLOW);
						sink->AddLines((D2D1_POINT_2F*)vertices.begin().base(), vertices.size());
						sink->EndFigure(D2D1_FIGURE_END_CLOSED);

						sink->Close();
						getTarget()->DrawGeometry(geometry, Direct2DBrush(color));

						sink->Release();
						geometry->Release();
					}

					void fill()
					{
						ID2D1PathGeometry *geometry; getFactory()->CreatePathGeometry(&geometry);
						ID2D1GeometrySink *sink; geometry->Open(&sink);
						
						sink->BeginFigure(vertices[0], D2D1_FIGURE_BEGIN_FILLED);
						sink->AddLines((D2D1_POINT_2F*)vertices.begin().base(), vertices.size());
						sink->EndFigure(D2D1_FIGURE_END_CLOSED);

						sink->Close();
						getTarget()->FillGeometry(geometry, Direct2DBrush(color));

						sink->Release();
						geometry->Release();
					}
			};

			class Bitmap : public Drawable
			{
				ID2D1Bitmap *bitmap;
				Size width, height;

				public:
					Vertex vertex;

					Bitmap(Vertex vertex = {})
					{this->vertex = vertex;}

					void loadFromFile(wstring filePath)
					{
						IWICImagingFactory *factory; CoCreateInstance(CLSID_WICImagingFactory, NULL, CLSCTX_INPROC_SERVER, IID_IWICImagingFactory, (void**)&factory);
						IWICBitmapDecoder *decoder; factory->CreateDecoderFromFilename(filePath.c_str(), NULL, GENERIC_READ, WICDecodeMetadataCacheOnLoad, &decoder);
						IWICBitmapFrameDecode *frame; decoder->GetFrame(0, &frame); frame->GetSize((UINT*)&width, (UINT*)&height);
						IWICFormatConverter *converter; factory->CreateFormatConverter(&converter);

						converter->Initialize(frame, GUID_WICPixelFormat32bppPBGRA, WICBitmapDitherTypeNone, NULL, 0, WICBitmapPaletteTypeMedianCut);
						getTarget()->CreateBitmapFromWicBitmap(converter, NULL, &bitmap);

						converter->Release();
						frame->Release();
						decoder->Release();
						factory->Release();
					}

					Size getWidth()
					{return width;}

					Size getHeight()
					{return height;}

					void draw()
					{getTarget()->DrawBitmap(bitmap, {vertex.x, vertex.y, vertex.x + width, vertex.y + height}, 1, D2D1_BITMAP_INTERPOLATION_MODE::D2D1_BITMAP_INTERPOLATION_MODE_NEAREST_NEIGHBOR, {0, 0, (float)width, (float)height});}
			};
		}
	}

	INT WINAPI WinMain(HINSTANCE instance_handle, HINSTANCE previous_instance_handle, PSTR cmd_line, INT show_cmd)
	{
		MSG message = {};
		WNDCLASS window_class = {};

		window_class.lpfnWndProc = WindowProc;
		window_class.hInstance = instance_handle;
		window_class.lpszClassName = L"GL_WINDOW";

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
			NULL,
			NULL,
			instance_handle,
			NULL
		);
		
		std::GL::window->create();

		ShowWindow(std::GL::window->handle, show_cmd);
		ShowWindow(GetConsoleWindow(), SW_HIDE);
		UpdateWindow(std::GL::window->handle);

		while(GetMessageW(&message, NULL, 0, 0) > 0)
		{TranslateMessage(&message); DispatchMessageW(&message);}

		return 0;
	}

	LRESULT CALLBACK WindowProc(HWND window_handle, UINT message, WPARAM message_parameter0, LPARAM message_parameter1)
	{
		switch(message)
		{
			case WM_DESTROY: std::GL::window->onDestroy(); PostQuitMessage(0); return 0;
			case WM_PAINT:  std::GL::window->update(); return 0;
			
			case WM_KEYDOWN: std::GL::window->key[message_parameter0] = true; std::GL::window->onKeystroke(message_parameter0); return 0;
			case WM_LBUTTONDOWN: std::GL::window->key[VK_LBUTTON] = true; std::GL::window->onKeystroke(VK_LBUTTON); return 0;
			case WM_MBUTTONDOWN: std::GL::window->key[VK_MBUTTON] = true; std::GL::window->onKeystroke(VK_MBUTTON); return 0;
			case WM_RBUTTONDOWN: std::GL::window->key[VK_RBUTTON] = true; std::GL::window->onKeystroke(VK_RBUTTON); return 0;
			case WM_XBUTTONDOWN:
				switch(message_parameter0)
				{
					case XBUTTON1: std::GL::window->key[VK_XBUTTON1] = true; std::GL::window->onKeystroke(VK_XBUTTON1); return 0;
					case XBUTTON2: std::GL::window->key[VK_XBUTTON2] = true; std::GL::window->onKeystroke(VK_XBUTTON2); return 0;
				}

			case WM_KEYUP: std::GL::window->key[message_parameter0] = false; return 0;
			case WM_LBUTTONUP: std::GL::window->key[VK_LBUTTON] = false; return 0;
			case WM_MBUTTONUP: std::GL::window->key[VK_MBUTTON] = false; return 0;
			case WM_RBUTTONUP: std::GL::window->key[VK_RBUTTON] = false; return 0;
			case WM_XBUTTONUP:
				switch(message_parameter0)
				{
					case XBUTTON1: std::GL::window->key[VK_XBUTTON1] = false; return 0;
					case XBUTTON2: std::GL::window->key[VK_XBUTTON2] = false; return 0;
				}
		}

		return DefWindowProcW(window_handle, message, message_parameter0, message_parameter1);
	}
#endif