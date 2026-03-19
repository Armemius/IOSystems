#include "io_utils.h"

#include "io.h"
#include "str_utils.h"

static int is_space(char c) {
  return c == ' ' || c == '\t' || c == '\n' || c == '\r' || c == '\v' ||
         c == '\f';
}

static int parse_decimal(const char **input, int *out) {
  const char *cursor = *input;
  int sign = 1;
  int value = 0;
  int has_digits = 0;

  if (*cursor == '-') {
    sign = -1;
    cursor++;
  } else if (*cursor == '+') {
    cursor++;
  }

  while (*cursor >= '0' && *cursor <= '9') {
    value = value * 10 + (*cursor - '0');
    cursor++;
    has_digits = 1;
  }

  if (!has_digits) {
    return 0;
  }

  *out = value * sign;
  *input = cursor;
  return 1;
}

static int parse_unsigned_decimal(const char **input, unsigned *out) {
  const char *cursor = *input;
  unsigned value = 0;
  int has_digits = 0;

  while (*cursor >= '0' && *cursor <= '9') {
    value = value * 10u + (unsigned)(*cursor - '0');
    cursor++;
    has_digits = 1;
  }

  if (!has_digits) {
    return 0;
  }

  *out = value;
  *input = cursor;
  return 1;
}

static int hex_digit_value(char c) {
  if (c >= '0' && c <= '9') {
    return c - '0';
  }
  if (c >= 'a' && c <= 'f') {
    return 10 + (c - 'a');
  }
  if (c >= 'A' && c <= 'F') {
    return 10 + (c - 'A');
  }
  return -1;
}

static int parse_hex(const char **input, unsigned *out) {
  const char *cursor = *input;
  unsigned value = 0;
  int has_digits = 0;
  int digit = 0;

  if (cursor[0] == '0' && (cursor[1] == 'x' || cursor[1] == 'X')) {
    cursor += 2;
  }

  digit = hex_digit_value(*cursor);
  while (digit >= 0) {
    value = (value << 4) | (unsigned)digit;
    cursor++;
    has_digits = 1;
    digit = hex_digit_value(*cursor);
  }

  if (!has_digits) {
    return 0;
  }

  *out = value;
  *input = cursor;
  return 1;
}

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
    return 0;
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

int scanf(const char *fmt, ...) {
  char input_line[256];
  int input_length = get_line(input_line, (int)sizeof(input_line) - 1);
  const char *input_cursor = input_line;
  int assigned = 0;

  va_list vargs;
  va_start(vargs, fmt);

  input_line[input_length] = '\0';

  while (*fmt) {
    if (is_space(*fmt)) {
      while (is_space(*fmt)) {
        fmt++;
      }
      while (is_space(*input_cursor)) {
        input_cursor++;
      }
      continue;
    }

    if (*fmt != '%') {
      if (*input_cursor != *fmt) {
        break;
      }
      if (*input_cursor != '\0') {
        input_cursor++;
      }
      fmt++;
      continue;
    }

    fmt++;
    if (*fmt == '\0') {
      break;
    }

    if (*fmt == '%') {
      if (*input_cursor != '%') {
        break;
      }
      input_cursor++;
      fmt++;
      continue;
    }

    if (*fmt != 'c') {
      while (is_space(*input_cursor)) {
        input_cursor++;
      }
    }

    switch (*fmt) {
    case 'd':
    case 'i': {
      int *out = va_arg(vargs, int *);
      if (!parse_decimal(&input_cursor, out)) {
        goto done;
      }
      assigned++;
      break;
    }
    case 'u': {
      unsigned *out = va_arg(vargs, unsigned *);
      if (!parse_unsigned_decimal(&input_cursor, out)) {
        goto done;
      }
      assigned++;
      break;
    }
    case 'x':
    case 'X': {
      unsigned *out = va_arg(vargs, unsigned *);
      if (!parse_hex(&input_cursor, out)) {
        goto done;
      }
      assigned++;
      break;
    }
    case 'c': {
      char *out = va_arg(vargs, char *);
      if (*input_cursor == '\0') {
        goto done;
      }
      *out = *input_cursor++;
      assigned++;
      break;
    }
    case 's': {
      char *out = va_arg(vargs, char *);
      int written = 0;

      while (*input_cursor && !is_space(*input_cursor)) {
        *out++ = *input_cursor++;
        written++;
      }

      if (written == 0) {
        goto done;
      }

      *out = '\0';
      assigned++;
      break;
    }
    default:
      goto done;
    }

    fmt++;
  }

done:
  va_end(vargs);
  return assigned;
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
