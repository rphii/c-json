#include "json.h"

#include <stdarg.h>
#include <ctype.h>
#include <errno.h>

VEC_INCLUDE(VrJson, vrjson, Json, BY_REF, BASE);
VEC_IMPLEMENT(VrJson, vrjson, Json, BY_REF, BASE, 0);

VEC_IMPLEMENT(VJson, vjson, Json, BY_REF, BASE, json_free);

size_t str_hash2(const Str *s) {
    return str_hash(*s);
}
int str_cmp2(const Str *a, const Str *b) {
    return str_cmp(*a, *b);
}
void str_free2(Str s) {
    str_free(&s);
}

LUT_IMPLEMENT(TJson, tjson, Str, BY_REF, Json, BY_REF, str_hash2, str_cmp2, str_free, json_free);

ErrDecl json_fmt_str(Str *out, JsonOptions *options, Str str);

#define ERR_static_json_parse_val(...) "failed parsing json value"
ErrImplStatic static_json_parse_val(Json *json, RStr *str, JsonOptions *options);

void static_json_parse_skip_ws(RStr *str);
JsonList static_json_parse_detect(Json *json, RStr *str);

inline void json_zero(Json *json)
{
    ASSERT_ARG(json);
    memset(json, 0, sizeof(*json));
}

inline void json_free(Json *json)
{
    switch(json->id) {
        case JSON_ARR: {
            vjson_free(&json->arr);
        } break; 
        case JSON_OBJ: {
            tjson_free(&json->obj);
        } break;
        case JSON_STR: {
            str_free(&json->str);
        } break;
        default: {} break;
    }
    memset(json, 0, sizeof(*json));
}

#define ERR_static_json_fmt_spacing(...) "failed formatting spacing"
ErrImplStatic static_json_fmt_spacing(Str *str, size_t n, JsonOptions *options)
{
    ASSERT_ARG(str);
    ASSERT_ARG(options);
    for(size_t i = 0; i < n; ++i) {
        TRYC_P(options->fmt.print_err, str_push_back(str, ' '));
    }
    return 0;
error:
    return -1;
}

ErrImpl vjson_fmt(VJson vjson, Str *str, JsonOptions *options)
{
    ASSERT_ARG(str);
    ASSERT_ARG(options);
    /* beginning of vector & spacing */
#if 0
    TRYC_P(str_fmt(str, "%*s[\n", options->fmt.spacing, ""));
#else
    TRYC_P(options->fmt.print_err, static_json_fmt_spacing(str, options->fmt.spacing, options));
    TRYC_P(options->fmt.print_err, str_push_back(str, '['));
    TRYC_P(options->fmt.print_err, str_push_back(str, '\n'));
#endif
    options->fmt.spacing += options->fmt.tabs;
    /* format vector */
    for(size_t i = 0; i < vjson_length(vjson); ++i) {
        Json *json = vjson_get_at(vjson, i);
        /* actually format */
        if(i) {
            //TRYC_P(str_fmt(str, ",\n"));
            TRYC_P(options->fmt.print_err, str_push_back(str, ','));
            TRYC_P(options->fmt.print_err, str_push_back(str, '\n'));
        }
        if(json->id != JSON_ARR && json->id != JSON_OBJ) {
            TRYC_P(options->fmt.print_err, static_json_fmt_spacing(str, options->fmt.spacing, options));
            //TRYC_P(str_fmt(str, "%*s", options->fmt.spacing, ""));
        }
        TRYC_P(options->fmt.print_err, json_fmt(json, str, options, 0));
    }
    /* spacing & end of vector */
    options->fmt.spacing -= options->fmt.tabs;
#if 0
    TRYC_P(str_fmt(str, "\n%*s]", options->fmt.spacing, ""));
#else
    TRYC_P(options->fmt.print_err, str_push_back(str, '\n'));
    TRYC_P(options->fmt.print_err, static_json_fmt_spacing(str, options->fmt.spacing, options));
    TRYC_P(options->fmt.print_err, str_push_back(str, ']'));
#endif
    return 0;
error:
    return -1;
}

ErrImpl tjson_fmt(TJson *tjson, Str *str, JsonOptions *options)
{
    ASSERT_ARG(tjson);
    ASSERT_ARG(str);
    ASSERT_ARG(options);
    /* beginning of object & spacing */
#if 0
    TRYC_P(str_fmt(str, "%*s{\n", options->fmt.spacing, ""));
#else
    TRYC_P(options->fmt.print_err, static_json_fmt_spacing(str, options->fmt.spacing, options));
    TRYC_P(options->fmt.print_err, str_push_back(str, '{'));
    TRYC_P(options->fmt.print_err, str_push_back(str, '\n'));
#endif
    options->fmt.spacing += options->fmt.tabs;
    /* format object */
    bool first = true;
    size_t n = 0;
    for(size_t ii = 0; ii < LUT_CAP(tjson->width); ++ii) {
        TJsonItem *jsonkv = tjson->buckets[ii];
        if(n >= tjson->used) break;
        if(!jsonkv) continue;
        if(jsonkv->hash == LUT_EMPTY) continue;
        if(!first) {
            //TRYC_P(str_fmt(str, ",\n"));
            TRYC_P(options->fmt.print_err, str_push_back(str, ','));
            TRYC_P(options->fmt.print_err, str_push_back(str, '\n'));
        }
        ++n;
#if 0
        TRYC_P(str_fmt(str, "%*s\"%.*s\": ", options->fmt.spacing, "", STR_F(jsonkv->key)));
#else
        TRYC_P(options->fmt.print_err, static_json_fmt_spacing(str, options->fmt.spacing, options));
        TRYC_P(options->fmt.print_err, str_push_back(str, '"'));
        TRYC_P(options->fmt.print_err, str_extend_back(str, *jsonkv->key));
        TRYC_P(options->fmt.print_err, str_extend_back(str, STR("\": ")));
#endif
        TRYC_P(options->fmt.print_err, json_fmt(jsonkv->val, str, options, 0));
        first = false;
    }
    /* spacing & end of object */
    options->fmt.spacing -= options->fmt.tabs;
#if 0
    TRYC_P(str_fmt(str, "\n%*s}", options->fmt.spacing, ""));
#else
    TRYC_P(options->fmt.print_err, str_push_back(str, '\n'));
    TRYC_P(options->fmt.print_err, static_json_fmt_spacing(str, options->fmt.spacing, options));
    TRYC_P(options->fmt.print_err, str_push_back(str, '}'));
#endif
    return 0;
error:
    return -1;
}

inline JsonOptions json_options_default(void)
{
    JsonOptions options = {
        .fmt.tabs = JSON_OPTIONS_TABS,
    };
    return options;
}

#define ERR_json_fmt_str(...)   "failed formatting json string"
ErrImpl json_fmt_str(Str *out, JsonOptions *options, Str str)
{
    ASSERT_ARG(out);
    ASSERT_ARG(options);
    for(size_t i = 0; i < str_length(str); ++i) {
        char c = str_get_at(str, i);
        if(c == '"') {
            TRYC(str_push_back(out, '\\'));
        }
        TRYC(str_push_back(out, c));
    }
    return 0;
error:
    return -1;
}

ErrImpl json_fmt(Json *json, Str *str, JsonOptions *options, Str *path)
{
    ASSERT_ARG(json);
    ASSERT_ARG(str);

    JsonOptions opts = {0};
    if(!options) {
        opts = json_options_default();
        options = &opts;
    };

    switch(json->id) {
        case JSON_ARR: {
            TRYC_P(options->fmt.print_err, vjson_fmt(json->arr, str, options));
        } break;
        case JSON_STR: {
#if 0
            TRYC_P(str_fmt(str, "\"%.*s\"", STR_F(&json->str)));
#else
            TRYC_P(options->fmt.print_err, str_push_back(str, '"'));
            TRYC_P(options->fmt.print_err, json_fmt_str(str, options, json->str));
            //TRYC_P(options->fmt.print_err, str_extend_back(str, json->str));
            TRYC_P(options->fmt.print_err, str_push_back(str, '"'));
#endif
        } break;
        case JSON_OBJ: {
            TRYC_P(options->fmt.print_err, tjson_fmt(&json->obj, str, options));
        } break;
        case JSON_INT: {
            TRYC_P(options->fmt.print_err, str_fmt(str, "%lli", json->i));
        } break;
        case JSON_BOOL: {
#if 0
            TRYC_P(str_fmt(str, "%s", json->b ? "true" : "false"));
#else
            if(json->b) {
                TRYC_P(options->fmt.print_err, str_extend_back(str, STR("true")));
            } else {
                TRYC_P(options->fmt.print_err, str_extend_back(str, STR("false")));
            }
#endif
        } break;
        case JSON_NONE: {
            THROW("can't format invalid json struct");
        } break;
        case JSON_NULL: {
#if 0
            TRYC_P(str_fmt(str, "null"));
#else
            TRYC_P(options->fmt.print_err, str_extend_back(str, STR("null")));
#endif
        } break;
        case JSON_FLOAT: {
            TRYC_P(options->fmt.print_err, str_fmt(str, "%lf", json->f));
        } break;
    }

    return 0;
error:
    return -1;
}

inline void static_json_parse_skip_ws(RStr *str)
{
    while(rstr_length(*str) && isspace(rstr_get_front(*str))) {
        /////++str->first;
        rstr_pop_front(str, 0);
    }
}

#define ERR_static_json_parse_detect(...) "failed detection of next value"
inline JsonList static_json_parse_detect(Json *json, RStr *str)
{
    ASSERT_ARG(json);
    ASSERT_ARG(str);
    /* skip whitespace */
    static_json_parse_skip_ws(str);
    /* now detect */
    if(!rstr_length(*str)) {
        return JSON_NONE;
    }
    char c = rstr_get_front(*str);
    if(c == '[') {
        return JSON_ARR;
    } else if(c == '{') {
        return JSON_OBJ;
    } else if((c >= '0' && c <= '9') || (c == '-')) {
        return JSON_INT;
        /* return JSON_FLOAT; */
    } else if(c == '"') {
        return JSON_STR;
    } else if(c == 't' || c == 'f') {
        return JSON_BOOL;
    } else if(c == 'n') {
        return JSON_NULL;
    }
    return JSON_NONE;
}

#define ERR_static_json_parse_single_char(str, c) "failed parsing single character '%c' @ %zu", c, str->first
ErrImplStatic static_json_parse_single_char(RStr *str, char c)
{
    /* verify begin */
    if(!(rstr_length(*str) && rstr_get_front(*str) == c)) {
        return -1;
    }
    /////++str->first;
    rstr_pop_front(str, 0);
    return 0;
}
    

#define ERR_static_json_parse_arr(...) "failed parsing json array"
ErrImplStatic static_json_parse_arr(Json *json, RStr *str, JsonOptions *options)
{
    ASSERT_ARG(json);
    ASSERT_ARG(str);
    ASSERT_ARG(options);
    /* verify begin */
    TRYC_P(options->parse.print_err, static_json_parse_single_char(str, '['));
    /* create vector */
    json->id = JSON_ARR;
    /* are we already at the end? */
    static_json_parse_skip_ws(str);
    if(!static_json_parse_single_char(str, ']')) {
        return 0;
    }
    for(;;) {
        /* parse value */
        Json val = {0};
        TRYC_P(options->parse.print_err, static_json_parse_val(&val, str, options));
        if(val.id) {
            TRY_P(options->parse.print_err, vjson_push_back(&json->arr, &val), ERR_VEC_PUSH_BACK);
        }
        /* check for next value */
        static_json_parse_skip_ws(str);
        if(static_json_parse_single_char(str, ',')) {
            break;
        }
    }
    /* verify end */
    static_json_parse_skip_ws(str);
    TRYC_P(options->parse.print_err, static_json_parse_single_char(str, ']'));
    return 0;
error:
    return -1;
}

#define ERR_static_json_parse_str(...) "failed parsing json string"
ErrImplStatic static_json_parse_str(Str *value, RStr *str, JsonOptions *options)
{
    ASSERT_ARG(value);
    ASSERT_ARG(str);
    ASSERT_ARG(options);
    /* verify begin */
    TRYC_P(options->parse.print_err, static_json_parse_single_char(str, '"'));
    /* create string */
    //json->id = JSON_STR;
    Str val = {0};
    RStr pending = *str;
    pending.last = pending.first;
    /* find end of string... TODO: rework */
    unsigned int escape = 0;
    while(rstr_length(*str)) {
        char c = rstr_get_front(*str);
        if(escape) {
            --escape;
            char c2 = 0;
            switch(c) {
                case '"': { c2 = '"'; } break;
                case '\\': { c2 = '\\'; } break;
                case '/': { c2 = '/'; } break;
                case 'b': { c2 = '\b'; } break;
                case 'f': { c2 = '\f'; } break;
                case 'n': { c2 = '\n'; } break;
                case 'r': { c2 = '\r'; } break;
                case 't': { c2 = '\t'; } break;
                case 'u': { 
                    for(size_t i = 0; i < 4; ++i) {
                        // TODO: fix, this is horrible.
                        if(!rstr_length(*str)) {
                            THROW_P(options->parse.print_err, "not enough characters after \\u escape");
                        }
                        /////++str->first;
                        rstr_pop_front(str, 0);
                    }
                } break;
            }
            if(c2) {
                TRY_P(options->parse.print_err, str_push_back(&val, c2), ERR_VEC_PUSH_BACK);
            }
            pending.first = str->first + 1;
            pending.last = pending.first;
        } else {
            if(c == '\\') {
                /* TODO: DRY vvvv */
                if(rstr_length(pending)) {
                    //TRYC_P(str_fmt(&val, "%.*s", STR_F(&pending)));
                    TRYC_P(options->parse.print_err, rstr_extend_back(&val, pending));
                    pending.first = pending.last;
                }
                /* TODO: DRY ^^^^ */
                escape = 1;
            } else if(c == '"') {
                /* TODO: DRY vvvv */
                if(rstr_length(pending)) {
                    //TRYC_P(str_fmt(&val, "%.*s", STR_F(&pending)));
                    TRYC_P(options->parse.print_err, rstr_extend_back(&val, pending));
                    pending.first = pending.last;
                }
                /* TODO: DRY ^^^^ */
                break;
            } else {
                ++pending.last;
                //TRY_P(str_push_back(&val, c), ERR_VEC_PUSH_BACK);
            }
        }
        /////++str->first;
        rstr_pop_front(str, 0);
    }
    //val.last = str->first;
    /* verify end */
    static_json_parse_skip_ws(str);
    TRYC_P(options->parse.print_err, static_json_parse_single_char(str, '"'));
    /* set string */
    *value = val;
    return 0;
error:
    return -1;
}

#define ERR_static_json_parse_obj(...) "failed parsing json object"
ErrImplStatic static_json_parse_obj(Json *json, RStr *str, JsonOptions *options)
{
    ASSERT_ARG(json);
    ASSERT_ARG(str);
    ASSERT_ARG(options);
    /* verify begin */
    TRYC_P(options->parse.print_err, static_json_parse_single_char(str, '{'));
    /* create object */
    json->id = JSON_OBJ;
    /* are we already at the end? */
    static_json_parse_skip_ws(str);
    if(!static_json_parse_single_char(str, '}')) {
        return 0;
    }
    /* go over key / items */
    for(;;) {
        Str key = {0};
        Json val = {0};
        /* parse string */
        static_json_parse_skip_ws(str);
        TRYC_P(options->parse.print_err, static_json_parse_str(&key, str, options));
        /* parse colon */
        static_json_parse_skip_ws(str);
        TRYC_P(options->parse.print_err, static_json_parse_single_char(str, ':'));
        /* parse value */
        TRYC_P(options->parse.print_err, static_json_parse_val(&val, str, options));
        //if(!json->obj.width) {
            //TRY_P(tjson_init(&json->obj, 8), ERR_LUTD_INIT);
        //}
        if(val.id) {
            //TRY_P(tjson_add(&json->obj, &jsonkv), ERR_LUTD_ADD);
            //TRY_P(vjson_push_back(&json->arr, &&jsonkv.val), ERR_VEC_PUSH_BACK);
            //printff("  set [%.*s]", STR_F(&key));
            TRY_P(options->parse.print_err, tjson_set(&json->obj, &key, &val), ERR_LUT_SET);
            //printff("  ... ok");
        }
        /* check for next value */
        static_json_parse_skip_ws(str);
        if(static_json_parse_single_char(str, ',')) {
            break;
        }
    }
    /* verify end */
    static_json_parse_skip_ws(str);
    TRYC_P(options->parse.print_err, static_json_parse_single_char(str, '}'));
    return 0;
error:
    //str_print_verbose(str);
    return -1;
}

#define ERR_static_json_parse_num(...) "failed parsing json num"
ErrImplStatic static_json_parse_num(Json *json, RStr *str, JsonOptions *options)
{
    ASSERT_ARG(json);
    ASSERT_ARG(str);
    ASSERT_ARG(options);
    /* determine number format */
    bool is_float = false;
    RStr val = *str;
    /* may be positive or negative */
    (void) static_json_parse_single_char(&val, '-'); // TODO: make way to remove (void)
    /* if the number begins with 0, instantly mark as float */
    if(rstr_length(val) && rstr_get_front(val) == '0') {
        is_float = true;
    } else if(rstr_length(val)) {
        /////++val.first;
        rstr_pop_front(&val, 0);
        for(size_t i = 0; i < rstr_length(val); ++i) {
            char c = rstr_get_front(val);
            /////++val.first;
            rstr_pop_front(&val, 0);
            if(c == '.' || c == 'E' || c == 'e') {
                is_float = true;
                break;
            } else if(!(c >= '0' && c <= '9')) {
                break;
            }
        }
    }
    /* get number */
    if(is_float) {
        //printff("  FLOAT!");
        char *endptr = 0;
        errno = 0;
        double f = strtod(rstr_iter_begin(*str), &endptr); // TODO: not really quite official json
        if(errno) {
            THROW_P(options->parse.print_err, "strtod() failed: %s", strerror(errno));
        }
        json->id = JSON_FLOAT;
        json->f = f;
        /////str->first += (endptr - str_iter_begin(str));
        ptrdiff_t n = endptr - rstr_iter_begin(*str);
        for(ptrdiff_t i = 0; i < n; ++i) {
            rstr_pop_front(str, 0);
        }
        return 0;
    } else {
        //printff("  INT!");
        char *endptr = 0;
        errno = 0;
        int i = strtol(rstr_iter_begin(*str), &endptr, 0); // TODO: not really quite official json
        if(errno) {
            THROW_P(options->parse.print_err, "strtol() failed: %s", strerror(errno));
        }
        json->id = JSON_INT;
        json->i = i;
        /////str->first += (endptr - str_iter_begin(str));
        ptrdiff_t n = endptr - rstr_iter_begin(*str);
        for(ptrdiff_t i = 0; i < n; ++i) {
            rstr_pop_front(str, 0);
        }
        return 0;
    }
    THROW_P(options->parse.print_err, "couldn't parse number");
error:
    return -1;
}

#define ERR_static_json_parse_bool(...) "failed parsing json bool"
ErrImplStatic static_json_parse_bool(Json *json, RStr *str, JsonOptions *options)
{
    ASSERT_ARG(json);
    ASSERT_ARG(str);
    ASSERT_ARG(options);
    RStr t = RSTR("true");
    RStr f = RSTR("false");
    if(rstr_length(*str) >= rstr_length(t) && !rstr_cmp(RSTR_IE(*str, rstr_length(t)), t)) {
        json->id = JSON_BOOL;
        json->b = true;
        /////str->first += str_length(&t);
        for(size_t i = 0; i < rstr_length(t); ++i) {
            rstr_pop_front(str, 0);
        }
        return 0;
    }
    if(rstr_length(*str) >= rstr_length(f) && !rstr_cmp(RSTR_IE(*str, rstr_length(f)), f)) {
        json->id = JSON_BOOL;
        json->b = false;
        /////str->first += str_length(&f);
        for(size_t i = 0; i < rstr_length(f); ++i) {
            rstr_pop_front(str, 0);
        }
        return 0;
    }
    THROW_P(options->parse.print_err, "'t' or 'f' character found, expecting 'true' or 'false', nothing found");
error:
    return -1;
}

#define ERR_static_json_parse_null(...) "failed parsing json null"
ErrImplStatic static_json_parse_null(Json *json, RStr *str, JsonOptions *options)
{
    ASSERT_ARG(json);
    ASSERT_ARG(str);
    ASSERT_ARG(options);
    RStr n = RSTR("null");
    if(rstr_length(*str) >= rstr_length(n) && !rstr_cmp(RSTR_IE(*str, rstr_length(n)), n)) {
        json->id = JSON_NULL;
        /////str->first += str_length(&n);
        for(size_t i = 0; i < rstr_length(n); ++i) {
            rstr_pop_front(str, 0);
        }
        return 0;
    }
    THROW_P(options->parse.print_err, "'n' found, expecting 'null', nothing found");
error:
    return -1;
}

ErrImplStatic static_json_parse_val(Json *json, RStr *str, JsonOptions *options)
{
    ASSERT_ARG(json);
    ASSERT_ARG(str);
    ASSERT_ARG(options);
    JsonList id = static_json_parse_detect(json, str);
    //printff("id %u / [%.50s]:%zu-%zu", id, str_iter_begin(str), str->first, str_length(str));
    switch(id) {
        case JSON_FLOAT: // redundant
        case JSON_INT: {
            //printff("PARSING NUM %zu[%.50s]", str->first, str_iter_begin(str));
            TRYC_P(options->parse.print_err, static_json_parse_num(json, str, options));
        } break;
        case JSON_ARR: {
            //printff("PARSING ARR %zu[%.50s]", str->first, str_iter_begin(str));
            TRYC_P(options->parse.print_err, static_json_parse_arr(json, str, options));
        } break;
        case JSON_OBJ: {
            //printff("PARSING OBJ %zu[%.50s]", str->first, str_iter_begin(str));
            TRYC_P(options->parse.print_err, static_json_parse_obj(json, str, options));
        } break;
        case JSON_BOOL: {
            //printff("PARSING BOOL %zu[%.50s]", str->first, str_iter_begin(str));
            TRYC_P(options->parse.print_err, static_json_parse_bool(json, str, options));
        } break;
        case JSON_NULL: {
            //printff("PARSING NULL %zu[%.50s]", str->first, str_iter_begin(str));
            TRYC_P(options->parse.print_err, static_json_parse_null(json, str, options));
        } break;
        case JSON_STR: {
            //printff("PARSING STR %zu[%.50s]", str->first, str_iter_begin(str));
            TRYC_P(options->parse.print_err, static_json_parse_str(&json->str, str, options));
            json->id = id;
        } break;
        case JSON_NONE: {} break;
    }
    //printff("   parse val done");
    return 0;
error:
    return -1;
}

ErrImpl json_parse(Json *json, Str *str, JsonOptions *options, Str *path)
{
    ASSERT_ARG(json);
    ASSERT_ARG(str);

    JsonOptions opts = {0};
    if(!options) {
        opts = json_options_default();
        options = &opts;
    };

    RStr parsing = str_rstr(*str);

    do {
        TRYC_P(options->parse.print_err, static_json_parse_val(json, &parsing, options));
    } while(rstr_length(parsing));

    return 0;
error:
    return -1;
}

Json json_get(Json *json, JsonPath *path)
{
    ASSERT_ARG(json);
    ASSERT_ARG(path); // -> if no path just ..return 0 if a valid json all in all lol

    Json result = *json;

    for(size_t i = 0; i < path->n; ++i) {
        switch(result.id) {
            case JSON_OBJ: {
                result = *tjson_get(&result.obj, &path->p[i].k);
            } break;
            case JSON_ARR: {
                result = *vjson_get_at(result.arr, path->p[i].i);
            } break;
            default: {
                result = (Json){.id = JSON_NONE};
            } break;
        }
    }
    return result;
}


