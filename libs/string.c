#include "string.h"

/* strlen: returns the length of the string */
size_t strlen(const char *str) {
    size_t len = 0;
    while (str[len] != '\0') {
        len++;
    }
    return len;
}

/* strcpy: copies the source string to the destination */
char *strcpy(char *dest, const char *src) {
    char *ret = dest;
    while ((*dest++ = *src++));
    return ret;
}

/* strncpy: copies up to n characters from source to destination */
char *strncpy(char *dest, const char *src, size_t n) {
    char *ret = dest;
    while (n-- && (*dest++ = *src++));
    while (n-- > 0) *dest++ = '\0';
    return ret;
}

/* strcat: appends the source string to the end of the destination string */
char *strcat(char *dest, const char *src) {
    char *ret = dest;
    while (*dest) dest++;
    while ((*dest++ = *src++));
    return ret;
}

/* strcmp: compares two strings lexicographically */
int strcmp(const char *s1, const char *s2) {
    while (*s1 && (*s1 == *s2)) {
        s1++;
        s2++;
    }
    return *(const unsigned char*)s1 - *(const unsigned char*)s2;
}

/* strncmp: compares the first n characters of two strings */
int strncmp(const char *s1, const char *s2, size_t n) {
    while (n-- && *s1 && (*s1 == *s2)) {
        s1++;
        s2++;
    }
    return n == 0 ? 0 : (*(const unsigned char*)s1 - *(const unsigned char*)s2);
}

/* strncat: appends up to n characters from source to the end of destination */
char *strncat(char *dest, const char *src, size_t n) {
    char *ret = dest;
    while (*dest) dest++;
    while (n-- && *src) {
        *dest++ = *src++;
    }
    *dest = '\0';
    return ret;
}

/* memmove: moves a block of memory (safe for overlapping regions) */
void *memmove(void *dest, const void *src, size_t n) {
    char *d = dest;
    const char *s = src;
    
    if (d < s) {
        while (n--) {
            *d++ = *s++;
        }
    } else {
        char *lasts = (char *)s + (n-1);
        char *lastd = d + (n-1);
        while (n--) {
            *lastd-- = *lasts--;
        }
    }
    
    return dest;
}

/* memset: fills a block of memory with a specified value */
void *memset(void *s, int c, size_t n) {
    unsigned char *p = s;
    while (n--) {
        *p++ = (unsigned char)c;
    }
    return s;
}

/* memcpy: copies a block of memory */
void *memcpy(void *dest, const void *src, size_t n) {
    char *d = dest;
    const char *s = src;
    while (n--) {
        *d++ = *s++;
    }
    return dest;
}