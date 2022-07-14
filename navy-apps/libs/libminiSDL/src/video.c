#include <NDL.h>
#include <sdl-video.h>
#include <assert.h>
#include <string.h>
#include <stdlib.h>
#include <stdio.h>

void SDL_BlitSurface(SDL_Surface *src, SDL_Rect *srcrect, SDL_Surface *dst, SDL_Rect *dstrect)
{
  // printf("dst->format->BitsPerPixe is %d\n",dst->format->BitsPerPixel);
  // TODO();

  assert(dst && src);
  assert(dst->format->BitsPerPixel == src->format->BitsPerPixel);
  // important:!!!uint32_t not int16_t, otherwise overflow, not learn
  // the definition from SDL_Rect!, width * height is pretty large!!
  uint32_t s_start_pos = 0;
  uint32_t s_sf_row_num = src->h, s_sf_col_num = src->w,
           s_rec_row_num = src->h, s_rec_col_num = src->w;
  if (srcrect != NULL)
  {
    s_start_pos = srcrect->x + srcrect->y * src->w;
    s_rec_row_num = srcrect->h;
    s_rec_col_num = srcrect->w;
  }

  uint32_t d_start_pos = 0;
  uint32_t d_sf_row_num = dst->h, d_sf_col_num = dst->w;
  //  d_rec_row_num = dst->h, d_rec_col_num = dst->w;
  // printf("x,y,w is %d-- %d-- %d",dstrect->x,dstrect->y,dst->w);
  if (dstrect != NULL)
  {
    d_start_pos = dstrect->x + dstrect->y * dst->w;
    // d_rec_row_num = (dstrect->h > 0)? dstrect->h : d_rec_row_num;
    // d_rec_col_num = (dstrect->w > 0)? dstrect->w : d_rec_col_num;
  }

  // printf("dst x,y,w,h %d,%d,%d,%d\n", dstrect->x, dstrect->y, dstrect->w, dstrect->h);
  // printf("src x,y,w,h %d,%d,%d,%d\n", srcrect->x, srcrect->y, srcrect->w, srcrect->h);
  // printf("======================");
  if (dst->format->BitsPerPixel == 32)
  {
    for (int row = 0; row < s_rec_row_num; ++row)
    {
      // printf("dst pos is  %d\n", d_sf_col_num);
      // printf("copy %p, size %d to %p, size %d\n",  src->pixels + s_start_pos + row * s_sf_col_num ,s_rec_col_num, dst->pixels + d_start_pos + row * d_sf_col_num, d_rec_col_num);
      memcpy((uint32_t *)dst->pixels + d_start_pos + row * d_sf_col_num, (uint32_t *)src->pixels + s_start_pos + row * s_sf_col_num, s_rec_col_num * sizeof(uint32_t));
    }
  }
  else if (dst->format->BitsPerPixel == 8)
  {
    for (int row = 0; row < s_rec_row_num; ++row)
    {
      // printf("copy %p, size %d to %p, size %d\n",  src->pixels + s_start_pos + row * s_sf_col_num ,s_rec_col_num, dst->pixels + d_start_pos + row * d_sf_col_num, d_rec_col_num);
      memcpy((uint8_t *)dst->pixels + d_start_pos + row * d_sf_col_num, (uint8_t *)src->pixels + s_start_pos + row * s_sf_col_num, s_rec_col_num * sizeof(uint8_t));
    }
  }
  else
    panic("unsupported pixel bites %d!\n", dst->format->BitsPerPixel);
}

void SDL_FillRect(SDL_Surface *dst, SDL_Rect *dstrect, uint32_t color)
{
  // panic();
  // TODO();

  int32_t start_pos = 0;
  uint32_t sf_row_num = dst->h;
  uint32_t sf_col_num = dst->w;
  uint32_t rec_row_num = dst->h;
  uint32_t rec_col_num = dst->w;
  if (dstrect != NULL)
  {
    start_pos = dstrect->x + dstrect->y * dst->w;
    rec_row_num = dstrect->h;
    rec_col_num = dstrect->w;
  }

  if (dst->format->BitsPerPixel == 32)
  {
    for (int row = 0; row < rec_row_num; ++row)
      memset((uint32_t *)dst->pixels + start_pos + row * sf_col_num, color, rec_col_num * sizeof(uint32_t));
  }
  else if (dst->format->BitsPerPixel == 8)
  {
    for (int row = 0; row < rec_row_num; ++row)
      memset((uint8_t *)dst->pixels + start_pos + row * sf_col_num, color, rec_col_num * sizeof(uint8_t));
  }
  else
    panic("unsupported pixel bites %d!\n", dst->format->BitsPerPixel);
  return;
}

void SDL_UpdateRect(SDL_Surface *s, int x, int y, int w, int h)
{
  // improtant: only w == 0 && h == 0, then set the s size.
  if (w == 0 && h == 0)
  {
    w = s->w;
    h = s->h;
  }

  uint32_t len = w * h;
  uint32_t *buf = malloc(sizeof(uint32_t) * len);
  uint32_t start_pos = x + y * s->w;
  uint32_t i = 0;
  for (size_t row = 0; row < h; ++row)
  {
    for (size_t col = 0; col < w; ++col)
    {
      uint32_t offset = col + row * s->w;
      if (s->format->BitsPerPixel == 32)
      {
        // important: s->pixels is 8 bit!!! r g b a r g b a...
        //            also, NDL_DrawRect buf should be a r g b a r g b....
        s->pixels[start_pos + offset];
        buf[i++] = s->pixels[start_pos + 4 * offset + 3] << 24 | s->pixels[start_pos + 4 * offset + 2] << 16 | s->pixels[start_pos + 4 * offset + 1] << 8 | s->pixels[start_pos + 4 * offset];
      }
      else if (s->format->BitsPerPixel == 8)
      {
        SDL_Color rgba_color = s->format->palette->colors[s->pixels[start_pos + offset]];
        buf[i++] = rgba_color.a << 24 | rgba_color.r << 16 | rgba_color.g << 8 | rgba_color.b;
      }
      else
        panic("unsupported pixel bites %d!\n", s->format->BitsPerPixel);
    }
  }

  NDL_DrawRect(buf, x, y, w, h);

  free(buf);
}

// APIs below are already implemented.

static inline int maskToShift(uint32_t mask)
{
  switch (mask)
  {
  case 0x000000ff:
    return 0;
  case 0x0000ff00:
    return 8;
  case 0x00ff0000:
    return 16;
  case 0xff000000:
    return 24;
  case 0x00000000:
    return 24; // hack
  default:
    assert(0);
  }
}

SDL_Surface *SDL_CreateRGBSurface(uint32_t flags, int width, int height, int depth,
                                  uint32_t Rmask, uint32_t Gmask, uint32_t Bmask, uint32_t Amask)
{
  assert(depth == 8 || depth == 32);
  SDL_Surface *s = malloc(sizeof(SDL_Surface));
  assert(s);
  s->flags = flags;
  s->format = malloc(sizeof(SDL_PixelFormat));
  assert(s->format);
  if (depth == 8)
  {
    s->format->palette = malloc(sizeof(SDL_Palette));
    assert(s->format->palette);
    s->format->palette->colors = malloc(sizeof(SDL_Color) * 256);
    assert(s->format->palette->colors);
    memset(s->format->palette->colors, 0, sizeof(SDL_Color) * 256);
    s->format->palette->ncolors = 256;
  }
  else
  {
    s->format->palette = NULL;
    s->format->Rmask = Rmask;
    s->format->Rshift = maskToShift(Rmask);
    s->format->Rloss = 0;
    s->format->Gmask = Gmask;
    s->format->Gshift = maskToShift(Gmask);
    s->format->Gloss = 0;
    s->format->Bmask = Bmask;
    s->format->Bshift = maskToShift(Bmask);
    s->format->Bloss = 0;
    s->format->Amask = Amask;
    s->format->Ashift = maskToShift(Amask);
    s->format->Aloss = 0;
  }

  s->format->BitsPerPixel = depth;
  s->format->BytesPerPixel = depth / 8;

  s->w = width;
  s->h = height;
  s->pitch = width * depth / 8;
  assert(s->pitch == width * s->format->BytesPerPixel);

  if (!(flags & SDL_PREALLOC))
  {
    s->pixels = malloc(s->pitch * height);
    assert(s->pixels);
  }

  return s;
}

SDL_Surface *SDL_CreateRGBSurfaceFrom(void *pixels, int width, int height, int depth,
                                      int pitch, uint32_t Rmask, uint32_t Gmask, uint32_t Bmask, uint32_t Amask)
{
  SDL_Surface *s = SDL_CreateRGBSurface(SDL_PREALLOC, width, height, depth,
                                        Rmask, Gmask, Bmask, Amask);
  assert(pitch == s->pitch);
  s->pixels = pixels;
  return s;
}

void SDL_FreeSurface(SDL_Surface *s)
{
  if (s != NULL)
  {
    if (s->format != NULL)
    {
      if (s->format->palette != NULL)
      {
        if (s->format->palette->colors != NULL)
          free(s->format->palette->colors);
        free(s->format->palette);
      }
      free(s->format);
    }
    if (s->pixels != NULL && !(s->flags & SDL_PREALLOC))
      free(s->pixels);
    free(s);
  }
}

SDL_Surface *SDL_SetVideoMode(int width, int height, int bpp, uint32_t flags)
{
  if (flags & SDL_HWSURFACE)
    NDL_OpenCanvas(&width, &height);
  // printf("size is %d,%d\n", width, height);
  return SDL_CreateRGBSurface(flags, width, height, bpp,
                              DEFAULT_RMASK, DEFAULT_GMASK, DEFAULT_BMASK, DEFAULT_AMASK);
}

void SDL_SoftStretch(SDL_Surface *src, SDL_Rect *srcrect, SDL_Surface *dst, SDL_Rect *dstrect)
{
  assert(src && dst);
  assert(dst->format->BitsPerPixel == src->format->BitsPerPixel);
  assert(dst->format->BitsPerPixel == 8);

  int x = (srcrect == NULL ? 0 : srcrect->x);
  int y = (srcrect == NULL ? 0 : srcrect->y);
  int w = (srcrect == NULL ? src->w : srcrect->w);
  int h = (srcrect == NULL ? src->h : srcrect->h);

  assert(dstrect);
  // printf("w and h is %d,%d\n",w,h);
  // printf("w and h is %d,%d\n",dstrect->w,distrect->h);
  if (w == dstrect->w && h == dstrect->h)
  {
    /* The source rectangle and the destination rectangle
     * are of the same size. If that is the case, there
     * is no need to stretch, just copy. */
    SDL_Rect rect;
    rect.x = x;
    rect.y = y;
    rect.w = w;
    rect.h = h;
    SDL_BlitSurface(src, &rect, dst, dstrect);
  }
  else
  {
    // printf("src size is %d, %d\n", src->w, src->h);
    // printf("dst size is %d, %d\n", dst->w, dst->h);
    // int w_ratio = dstrect->w/w, h_ratio = dstrect->h/h;
    // printf("ratio is %d, %d\n", w_ratio,h_ratio);
    // for(int row = y; row < y + w; ++row){
    //   for(int col = x; col < x + h; ++col){
    //     dst->pixels[col + row * w] = src->pixels[w_ratio * col + h_ratio * row * w ];
    //     // printf("from [%d, %d] to [%d, %d]\n", row, col, h_ratio * row, w_ratio * col);
    //   }
    // }
    assert(0);
  }
}

void SDL_SetPalette(SDL_Surface *s, int flags, SDL_Color *colors, int firstcolor, int ncolors)
{
  assert(s);
  assert(s->format);
  assert(s->format->palette);
  assert(firstcolor == 0);

  s->format->palette->ncolors = ncolors;
  memcpy(s->format->palette->colors, colors, sizeof(SDL_Color) * ncolors);

  if (s->flags & SDL_HWSURFACE)
  {
    assert(ncolors == 256);
    for (int i = 0; i < ncolors; i++)
    {
      uint8_t r = colors[i].r;
      uint8_t g = colors[i].g;
      uint8_t b = colors[i].b;
    }
    SDL_UpdateRect(s, 0, 0, 0, 0);
  }
}

static void ConvertPixelsARGB_ABGR(void *dst, void *src, int len)
{
  int i;
  uint8_t(*pdst)[4] = dst;
  uint8_t(*psrc)[4] = src;
  union
  {
    uint8_t val8[4];
    uint32_t val32;
  } tmp;
  int first = len & ~0xf;
  for (i = 0; i < first; i += 16)
  {
#define macro(i)                      \
  tmp.val32 = *((uint32_t *)psrc[i]); \
  *((uint32_t *)pdst[i]) = tmp.val32; \
  pdst[i][0] = tmp.val8[2];           \
  pdst[i][2] = tmp.val8[0];

    macro(i + 0);
    macro(i + 1);
    macro(i + 2);
    macro(i + 3);
    macro(i + 4);
    macro(i + 5);
    macro(i + 6);
    macro(i + 7);
    macro(i + 8);
    macro(i + 9);
    macro(i + 10);
    macro(i + 11);
    macro(i + 12);
    macro(i + 13);
    macro(i + 14);
    macro(i + 15);
  }

  for (; i < len; i++)
  {
    macro(i);
  }
}

SDL_Surface *SDL_ConvertSurface(SDL_Surface *src, SDL_PixelFormat *fmt, uint32_t flags)
{
  assert(src->format->BitsPerPixel == 32);
  assert(src->w * src->format->BytesPerPixel == src->pitch);
  assert(src->format->BitsPerPixel == fmt->BitsPerPixel);

  SDL_Surface *ret = SDL_CreateRGBSurface(flags, src->w, src->h, fmt->BitsPerPixel,
                                          fmt->Rmask, fmt->Gmask, fmt->Bmask, fmt->Amask);

  assert(fmt->Gmask == src->format->Gmask);
  assert(fmt->Amask == 0 || src->format->Amask == 0 || (fmt->Amask == src->format->Amask));
  ConvertPixelsARGB_ABGR(ret->pixels, src->pixels, src->w * src->h);

  return ret;
}

uint32_t SDL_MapRGBA(SDL_PixelFormat *fmt, uint8_t r, uint8_t g, uint8_t b, uint8_t a)
{
  assert(fmt->BytesPerPixel == 4);
  uint32_t p = (r << fmt->Rshift) | (g << fmt->Gshift) | (b << fmt->Bshift);
  if (fmt->Amask)
    p |= (a << fmt->Ashift);
  return p;
}

int SDL_LockSurface(SDL_Surface *s)
{
  return 0;
}

void SDL_UnlockSurface(SDL_Surface *s)
{
}
