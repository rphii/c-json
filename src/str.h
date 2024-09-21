#ifndef STR_H

#define STR_DEFAULT_SIZE 32

#include <stdarg.h>
#include <stdbool.h>
#include <stdint.h>

#include "err.h"

typedef uint8_t V3u8[3];

/* configuration, inclusion and de-configuration of vector */

#define VEC_SETTINGS_DEFAULT_SIZE STR_DEFAULT_SIZE
#define VEC_SETTINGS_KEEP_ZERO_END 1
#define VEC_SETTINGS_STRUCT_ITEMS s
#include "vec.h"

VEC_INCLUDE(Str, str, char, BY_VAL, BASE);

#undef VEC_SETTINGS_STRUCT_ITEMS
#undef VEC_SETTINGS_KEEP_ZERO_END
#undef VEC_SETTINGS_DEFAULT_SIZE

#define STR(string)             (Str){.s = (char *)string, .last = sizeof(string)/sizeof(*string)-1}
#define STR_L(string)           (Str){.s = (char *)string, .last = strlen(string ? string : "")}
#define STR_LL(string, length)  (Str){.s = (char *)string, .last = length}

#define RSTR(string)             (RStr){.s = (char *)string, .last = sizeof(string)/sizeof(*string)-1}
#define RSTR_L(string)           (RStr){.s = (char *)string, .last = strlen(string ? string : "")}
#define RSTR_LL(string, length)  (RStr){.s = (char *)string, .last = length}

#define STR_F(s)                (int)str_length(s), str_iter_begin(s)
#define STR_I0(str, i0)         (const Str){.s = (str).s, .first = ((str).first + i0 < (str).last) ? (str).first + i0 : (str).last, .last = (str).last}
#define STR_IE(str, iE)         (const Str){.s = (str).s, .first = (str).first, .last = (str).first + iE}

#define ERR_str_extend_back(...)    "failed appending string to other string"
#define ERR_str_push_back(...)      "failed appending character to string"
#define ERR_str_copy(...)           "failed copying string"

#define RSTR_F(s)               (int)rstr_length(s), rstr_iter_begin(s)
#define RSTR_I0(str, i0)        (const RStr){.s = (str).s, .first = ((str).first + i0 < (str).last) ? (str).first + i0 : (str).last, .last = (str).last}
#define RSTR_IE(str, iE)        (const RStr){.s = (str).s, .first = (str).first, .last = (str).first + iE}

//#define ERR_str_extend_back(...)    "failed extending string"
#define ERR_rstr_extend_back(...)    "failed extending string"

//#define ERR_str_copy(v1, v2)    "failed copying string"
#define ERR_rstr_copy(v1, v2)   "failed copying string"

/* other functions */

#define STR_INCLUDE_MOD(type, name, ...)  \
    type str_##name(Str *str, ##__VA_ARGS__); \
    type rstr_##name(RStr *str, ##__VA_ARGS__);

#define STR_INCLUDE_CONST(type, name, ...)  \
    type str_##name(const Str str, ##__VA_ARGS__); \
    type rstr_##name(const RStr str, ##__VA_ARGS__);



STR_INCLUDE_MOD(void, pop_back_char);
STR_INCLUDE_MOD(void, pop_back_word);
STR_INCLUDE_MOD(void, triml);
STR_INCLUDE_MOD(void, trimr);
STR_INCLUDE_MOD(void, trim);

STR_INCLUDE_MOD(void, remove_trailing_ch, char ch, char ch_escape);

STR_INCLUDE_CONST(void, cstr, char *cstr, size_t len);
STR_INCLUDE_MOD(void, clear_to_last);

#define ERR_str_fmt_va(str, format, argp)   "failed formatting string"
#define ERR_rstr_fmt_va(str, format, argp)  "failed formatting string"
STR_INCLUDE_MOD(ErrDecl, fmt_va, const char *format, va_list argp);

#define ERR_str_fmt(str, format, ...)       "failed formatting string"
#define ERR_rstr_fmt(str, format, ...)      "failed formatting string"
STR_INCLUDE_MOD(ErrDecl, fmt, const char *format, ...);

STR_INCLUDE_CONST(RStr, get_ext);
STR_INCLUDE_CONST(RStr, get_noext);
STR_INCLUDE_CONST(RStr, get_dir);
STR_INCLUDE_CONST(RStr, get_nodir);
STR_INCLUDE_CONST(RStr, get_basename);
STR_INCLUDE_CONST(RStr, get_line, RStr *prev);


#define ERR_str_fmt_fgbg(out, text, ...) "failed applying foreground/background to string '%.*s'", STR_F(text)
ErrDecl str_fmt_fgbg(Str *out, const Str text, const V3u8 fg, const V3u8 bg, bool bold, bool italic, bool underline);

#define ERR_STR_GET_STR     "failed getting string from user"
ErrDecl str_get_str(Str *str);

#define ERR_str_expand_path(path, base, home, ...) "failed expanding path"
//ErrDecl str_expand_path(Str *path, const Str *base, const Str *current, const Str *home);
ErrDecl str_expand_path(Str *path, const Str base, const Str home);


int str_cmp(const Str a, const Str b);
int rstr_cmp(const RStr a, const RStr b);
int str_rstr_cmp(const Str a, const RStr b);

int str_cmp_sortable(const Str a, const Str b);
int rstr_cmp_sortable(const RStr a, const RStr b);
int str_rstr_cmp_sortable(const Str a, const RStr b);

int str_cmp_ci(const Str a, const Str b);
int rstr_cmp_ci(const RStr a, const RStr b);
int str_rstr_cmp_ci(const Str a, const RStr b);

STR_INCLUDE_CONST(size_t, nch, char ch, size_t n);
STR_INCLUDE_CONST(size_t, ch, char ch, size_t n);
STR_INCLUDE_CONST(size_t, ch_from, char ch, size_t n, size_t from);
STR_INCLUDE_CONST(size_t, ch_pair, char c1);
STR_INCLUDE_CONST(size_t, find_ws);
STR_INCLUDE_CONST(size_t, find_nws);
STR_INCLUDE_CONST(size_t, find_rnws);
STR_INCLUDE_CONST(size_t, rch, char ch, size_t n);
STR_INCLUDE_CONST(size_t, rnch, char ch, size_t n);
STR_INCLUDE_CONST(size_t, count_ch, char ch);
STR_INCLUDE_CONST(size_t, irch, size_t iE, char ch, size_t n);

STR_INCLUDE_CONST(size_t, hash);
STR_INCLUDE_CONST(size_t, hash_ci);

RStr str_splice(const Str *to_splice, RStr *prev_splice, char sep);
RStr rstr_splice(const RStr *to_splice, RStr *prev_splice, char sep);




/* TODO implement for bot rstr and str */

size_t str_hash_esci(const Str a);
int str_cmp_esci(const Str a, const Str b);
int str_cmp_ci_any(const Str a, const Str *b, size_t len);
size_t str_count_overlap(const Str a, const Str b, bool ignorecase);
size_t str_find_substring(const Str str, const Str sub);
size_t str_find_any(const Str str, const Str any);
size_t str_find_nany(const Str str, const Str any);
// TODO size_t str_find_rnany(const Str *str, const Str *any);
#define ERR_str_remove_escapes(out, in) "failed removing escape sequences"
ErrDecl str_remove_escapes(Str *out, const Str in);

#define STR_H
#endif

