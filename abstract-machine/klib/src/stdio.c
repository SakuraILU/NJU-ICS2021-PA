#include <am.h>
#include <klib.h>
#include <klib-macros.h>
#include <stdarg.h>

#if !defined(__ISA_NATIVE__) || defined(__NATIVE_USE_KLIB__)

#if defined(CONFIG_ISA64)
typedef uint64_t word_t;
#else
typedef uint32_t word_t;
#endif

#define NUM_LEN 32
#define OSTRING_LEN 5120

int printf(const char *fmt, ...)
{
  char out[OSTRING_LEN];

  va_list ap;
  va_start(ap, fmt);
  int num = vsprintf(out, fmt, ap);
  va_end(ap);

  for (int i = 0; i < num; ++i)
  {
    putch(out[i]);
  }

  return num;
}

static char *__itoa(int num, char *buff, uint16_t base)
{
  static const char sym[] = "0123456789abcdef";

  char tmp[NUM_LEN];
  bool is_neg = false;
  if (num == 0)
  {
    strcpy(buff, "0");
    return buff;
  }
  else if (num < 0)
  {
    is_neg = true;
    strcpy(buff, "-");
    buff++;
    num = -num;
  }

  uint8_t i = 0;
  while (num != 0)
  {
    tmp[i] = sym[num % base];
    num /= base;
    i++;
  }

  for (int j = i - 1; j >= 0; --j)
    buff[i - 1 - j] = tmp[j];
  buff[i] = '\0';

  return is_neg ? (buff - 1) : buff;
}

static char *__ptoa(void *p, char *buff)
{
  static const char sym[] = "0123456789abcdef";

  word_t num = (word_t)p;
  char tmp[NUM_LEN];

  if (num == 0)
  {
    buff[0] = '0';
    buff[1] = '\0';
    return (buff - 1);
  }

  uint8_t i = 0;
  while (num != 0)
  {
    tmp[i] = sym[num % 16];
    num /= 16;
    i++;
  }

  for (int j = i - 1; j >= 0; --j)
    buff[i - 1 - j] = tmp[j];
  buff[i] = '\0';

  return (buff - 1);
}

int vsprintf(char *out, const char *fmt, va_list ap)
{
  char *pout = out;
  size_t i = 0;
  while (fmt[i] != '\0')
  {
    if (fmt[i] != '%')
    {
      *(pout++) = fmt[i];
    }
    else
    {
      bool is_end = false;
      char num_leftpad_sym = ' ';
      bool left_align = false;
      int width = 0;
      while (!is_end)
      {
        i++;
        switch (fmt[i])
        {
        case '0':
        {
          if (fmt[i - 1] == '%')
          {
            num_leftpad_sym = '0';
            break;
          }
        }
        case '1':
        case '2':
        case '3':
        case '4':
        case '5':
        case '6':
        case '7':
        case '8':
        case '9':
        {
          width = width * 10 + (fmt[i] - '0');
          break;
        }
        case '-':
        {
          if (fmt[i - 1] == '%')
          {
            left_align = true;
          }
          break;
        }
        case 's':
        {
          const char *str = va_arg(ap, char *);
          if (!left_align)
          {
            for (int j = 0; j < width - (int)strlen(str); ++j)
            {
              *(pout++) = ' ';
            }
            strcpy(pout, str);
            pout += strlen(str);
          }
          else
          {
            strcpy(pout, str);
            pout += strlen(str);
            for (int j = 0; j < width - (int)strlen(str); ++j)
            {
              *(pout++) = ' ';
            }
          }
          is_end = true;
          break;
        }
        case 'd':
        case 'i':
        case 'x':
        {
          int val = va_arg(ap, int);

          char buff[NUM_LEN];
          __itoa(val, buff, (fmt[i] == 'x') ? 16 : 10);
          if (!left_align)
          {
            for (int j = 0; j < width - (int)strlen(buff); ++j)
              *(pout++) = num_leftpad_sym;
            strcpy(pout, buff);
            pout += strlen(buff);
          }
          else
          {
            strcpy(pout, buff);
            pout += strlen(buff);
            for (int j = 0; j < width - (int)strlen(buff); ++j)
              *(pout++) = num_leftpad_sym;
          }
          is_end = true;
          break;
        }
        case 'p':
        {
          void *val = va_arg(ap, void *);

          char buff[NUM_LEN];
          __ptoa(val, buff);
          if (!left_align)
          {
            for (int j = 0; j < width - (int)strlen(buff); ++j)
              *(pout++) = num_leftpad_sym;
            strcpy(pout, buff);
            pout += strlen(buff);
          }
          else
          {
            strcpy(pout, buff);
            pout += strlen(buff);
            for (int j = 0; j < width - (int)strlen(buff); ++j)
              *(pout++) = num_leftpad_sym;
          }
          is_end = true;
          break;
        }
        default:
        {
          printf("print format is wrong!\n");
          // Wrong format!!!
          assert(0);
        }
        }
      }
    }
    ++i;
  }
  *pout = '\0';

  va_end(ap);

  return strlen(out);
}

int sprintf(char *out, const char *fmt, ...)
{
  va_list ap;
  va_start(ap, fmt);
  int num = vsprintf(out, fmt, ap);
  va_end(ap);

  return num;
}

int snprintf(char *out, size_t n, const char *fmt, ...)
{
  panic("Not implemented");
}

int vsnprintf(char *out, size_t n, const char *fmt, va_list ap)
{
  panic("Not implemented");
}

#endif
