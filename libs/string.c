#include "string.h"

/* strlen: stringin uzunluğunu döndürür */
size_t strlen(const char *str) {
    size_t len = 0;
    while (str[len] != '\0') {
        len++;
    }
    return len;
}

/* strcpy: kaynak stringi hedefe kopyalar */
char *strcpy(char *dest, const char *src) {
    char *ret = dest;
    while ((*dest++ = *src++));
    return ret;
}

/* strncpy: kaynaktan n karakter kopyalar */
char *strncpy(char *dest, const char *src, size_t n) {
    char *ret = dest;
    while (n-- && (*dest++ = *src++));
    while (n-- > 0) *dest++ = '\0';
    return ret;
}

/* strcat: kaynak stringi hedefin sonuna ekler */
char *strcat(char *dest, const char *src) {
    char *ret = dest;
    while (*dest) dest++;
    while ((*dest++ = *src++));
    return ret;
}

/* strcmp: iki stringi karşılaştırır */
int strcmp(const char *s1, const char *s2) {
    while (*s1 && (*s1 == *s2)) {
        s1++;
        s2++;
    }
    return *(const unsigned char*)s1 - *(const unsigned char*)s2;
}

/* strncmp: iki stringin ilk n karakterini karşılaştırır */
int strncmp(const char *s1, const char *s2, size_t n) {
    while (n-- && *s1 && (*s1 == *s2)) {
        s1++;
        s2++;
    }
    return n == 0 ? 0 : (*(const unsigned char*)s1 - *(const unsigned char*)s2);
}

/* memmove: bellek alanını taşır (üst üste bölgelerde güvenli) */
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

/* memset: bellek alanını belirli bir değerle doldurur */
void *memset(void *s, int c, size_t n) {
    unsigned char *p = s;
    while (n--) {
        *p++ = (unsigned char)c;
    }
    return s;
}

/* memcpy: bellek alanını kopyalar */
void *memcpy(void *dest, const void *src, size_t n) {
    char *d = dest;
    const char *s = src;
    while (n--) {
        *d++ = *s++;
    }
    return dest;
}