#pragma once

#define va_list __builtin_va_list
#define va_start __builtin_va_start
#define va_end __builtin_va_end
#define va_arg __builtin_va_arg

// Receives pointer to null-terminated string
// and prints it
void print(const char *buffer);

// Receives pointer to null-terminated string
// and prints it with newline
void println(const char *buffer);

// Receives pointer to null-terminated string
// and prints it with formatting
void printf(const char *fmt, ...);

// Receives pointer to buffer and its size
// and prints it
void print_buffer(const char *buffer, int length);

// Scans input line till buffer full or newline character
int get_line(char *buffer, int max_length);
