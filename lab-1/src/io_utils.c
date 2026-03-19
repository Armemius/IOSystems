#include "io_utils.h"

#include "io.h"
#include "str_utils.h"

void print(const char *buffer) {
  while (*buffer) {
    put_char(*buffer++);
  }
}

void println(const char *buffer) {
  print(buffer);
  put_char('\n');
}

void print_buffer(const char *buffer, int length) {
  for (int it = 0; it < length; ++it) {
    put_char(buffer[it]);
  }
}

int get_line(char *buffer, int max_length) {
  if (max_length < 1) {
  }
  char current_char = 0;
  int current_length = 0;
  while (current_length < max_length) {
    current_char = get_char();
    switch (current_char) {
    case '\b':
      print("\b \b");
      --current_length;
      break;
    case '\0':
    case '\n':
    case '\r':
      put_char('\n');
      return current_length;
    default:
      put_char(current_char);
      buffer[current_length++] = current_char;
    }
  }
  put_char('\n');
  return current_length;
}

void printf(const char *fmt, ...) {
  va_list vargs;
  va_start(vargs, fmt);

  while (*fmt) {
    if (*fmt == '%') {
      fmt++;

      int width = 0;
      int zero_pad = 0;
      int left_align = 0;

      while (1) {
        if (*fmt == '0')
          zero_pad = 1;
        else if (*fmt == '-')
          left_align = 1;
        else
          break;
        fmt++;
      }

      while (*fmt >= '0' && *fmt <= '9') {
        width = width * 10 + (*fmt++ - '0');
      }

      switch (*fmt) {
      case 'c': {
        char c = (char)va_arg(vargs, int);
        put_char(c);
        break;
      }

      case 's': {
        const char *s = va_arg(vargs, const char *);
        if (!s)
          s = "(null)";
        int len = strlen(s);

        if (width > len && !left_align) {
          for (int i = len; i < width; i++)
            put_char(zero_pad ? '0' : ' ');
        }

        print(s);

        if (width > len && left_align) {
          for (int i = len; i < width; i++)
            put_char(' ');
        }

        break;
      }

      case 'd':
      case 'i': {
        int num = va_arg(vargs, int);
        char buffer[32];
        itoa(num, buffer);

        int len = strlen(buffer);

        if (width > len && !left_align) {
          for (int i = len; i < width; i++)
            put_char(zero_pad ? '0' : ' ');
        }

        print(buffer);

        if (width > len && left_align) {
          for (int i = len; i < width; i++)
            put_char(' ');
        }
        break;
      }

      case 'u': {
        unsigned num = va_arg(vargs, unsigned);
        char buffer[32];
        itoa((int)num, buffer);

        int len = strlen(buffer);

        if (width > len && !left_align) {
          for (int i = len; i < width; i++)
            put_char(zero_pad ? '0' : ' ');
        }

        print(buffer);

        if (width > len && left_align) {
          for (int i = len; i < width; i++)
            put_char(' ');
        }
        break;
      }

      case 'x':
      case 'X': {
        unsigned num = va_arg(vargs, unsigned);
        char buffer[32];
        char *p = buffer;
        const char *digits =
            (*fmt == 'X') ? "0123456789ABCDEF" : "0123456789abcdef";

        if (num == 0) {
          *p++ = '0';
        } else {
          while (num > 0) {
            *p++ = digits[num & 0xF];
            num >>= 4;
          }
        }

        *p = '\0';
        reverse(buffer);

        int len = strlen(buffer);
        if (width > len && !left_align) {
          for (int i = len; i < width; i++)
            put_char(zero_pad ? '0' : ' ');
        }

        print(buffer);

        if (width > len && left_align) {
          for (int i = len; i < width; i++)
            put_char(' ');
        }
        break;
      }

      case '\0': {
        put_char('%');
        goto end;
      }

      case '%': {
        put_char('%');
        break;
      }

      default: {
        put_char('%');
        put_char(*fmt);
        break;
      }
      }
    }

    else {
      put_char(*fmt);
    }

    fmt++;
  }

end:
  va_end(vargs);
}
