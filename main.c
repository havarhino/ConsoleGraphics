#include <stdio.h>
#include <Windows.h>
#include <string.h>
#include <math.h>

typedef struct Point
{
  float x;
  float y;
} Point;

typedef unsigned int uint32;

static HDC bitmapHandle; /* Handle to the bitmap device context    */
static uint32 *bitmap;   /* Pointer to the beginning of the bitmap */
static int bitmapWidth;  /* The bitmap width in pixels             */
static int bitmapHeight; /* The bitmap height in pixels            */

void createDIB(HWND windowHandle) {
  int DIBrowByteWidth;           /* How many bytes per row of pixels                                      */
  HBITMAP m_hDIBBitmap;          /* A handle to the new bitmap made                                       */
  BITMAPINFO mybmi;              /* A struct containing information about the bitmap                      */
  RECT mainClientRect;           /* A rect structure containing the width and height of the client window */
  int bitCount;                  /* Number of bits per pixel (8 for R, 8 for G, 8 for B, 8 for alpha)     */
  int totalBytes;                /* Total bytes in the bitmap                                             */

  HDC hdc = GetDC(windowHandle); /* The window device context                                             */

  /* Get the width and height of the client window */
  GetClientRect(windowHandle, &mainClientRect);
  bitmapWidth = mainClientRect.right;
  bitmapHeight = mainClientRect.bottom;
  /* Set the bit count per pixel */
  bitCount = 32;
  /* Set the bytes per row */
  DIBrowByteWidth = ((bitmapWidth * (bitCount / 8) + 3) & -4);
  /* Set the total number of bytes in the bitmap */
  totalBytes = DIBrowByteWidth * bitmapHeight;

  /* Fill out information about the bitmap */
  mybmi.bmiHeader.biSize = sizeof(mybmi);
  mybmi.bmiHeader.biWidth = bitmapWidth;
  /* This needs to be negative to make the pixels read top-down */
  mybmi.bmiHeader.biHeight = -bitmapHeight;
  mybmi.bmiHeader.biPlanes = 1;
  mybmi.bmiHeader.biBitCount = (WORD)bitCount;
  mybmi.bmiHeader.biCompression = BI_RGB;
  mybmi.bmiHeader.biSizeImage = totalBytes;
  mybmi.bmiHeader.biXPelsPerMeter = 0;
  mybmi.bmiHeader.biYPelsPerMeter = 0;

  /* Create a compatible device context from our current device context */
  bitmapHandle = CreateCompatibleDC(hdc);
  /* Create the bytes of the pixels for the bitmap to write to directly */
  m_hDIBBitmap = CreateDIBSection(hdc, &mybmi, DIB_RGB_COLORS, (VOID **)&bitmap, NULL, 0);
  /* Select this device context */
  SelectObject(bitmapHandle, m_hDIBBitmap);

  /* Release the window device context */
  ReleaseDC(windowHandle, hdc);
}

void draw(HDC hdc, int a, int b, float theta)
{
/* FAST */
#if 1
  /* Clear the bitmap to black */
  memset(bitmap, 0, bitmapWidth * bitmapHeight * 4);

  if( a >= (bitmapWidth - 200)) {
	  a = bitmapWidth - 201;
  }
  if( b >= (bitmapHeight - 200)) {
	  b = bitmapHeight - 201;
  }
  /* Draw the square */
  for(int y = b; y < b + 200; y++)
  {
    for(int x = a; x < a + 200; x++)
    {
      bitmap[y * bitmapWidth + x] = 255;
    }
  }

  /* Draw the square that moves in a circle */
  Point pos;
  pos.x = 300.0f + (150.0f * cos(theta));
  pos.y = 300.0f + (150.0f * sin(theta));
  for(int y = (int)pos.y; y < (int)pos.y + 200; y++)
  {
    for(int x = (int)pos.x; x < (int)pos.x + 200; x++)
    {
      bitmap[y * bitmapWidth + x] = 255;
    }
  }


/* SLOW */
#else
  /* Draw the square */
  for(int y = b; y < b + 200; y++)
  {
    for(int x = a; x < a + 200; x++)
    {
      SetPixel(hdc, x, y, RGB(255, 0, 0));
    }
  }

  /* Draw the square that moves in a circle */
  Point pos;
  pos.x = 300.0f + (150.0f * cos(theta));
  pos.y = 300.0f + (150.0f * sin(theta));
  for(int y = (int)pos.y; y < (int)pos.y + 200; y++)
  {
    for(int x = (int)pos.x; x < (int)pos.x + 200; x++)
    {
      SetPixel(hdc, x, y, RGB(255, 0, 0));
    }
  }
#endif
}

int main(void)
{
  /* Get a handle to the console window */
  HANDLE window = GetConsoleWindow();
  /* Get the device context to that window */
  HDC dc = GetDC(window);
  int x = 0;
  int y = 0;
  float theta = 0;

  /* Create the memory buffer of pixels that works with windows */
  createDIB(window);

  while(1)
  {
    /* Update a counter */
    x++;
    y++;
    theta += 0.01f;

    /* Draw my shapes */
    draw(dc, x, y, theta);

    /* Blt the bitmap to the window */
    BitBlt(dc, 0, 0, bitmapWidth, bitmapHeight, bitmapHandle, 0, 0, SRCCOPY);
  }

  /* Release the device context */
  ReleaseDC(window, dc);

  getchar();

  return 0;
}
