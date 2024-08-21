#include "json.h"

#include <stdarg.h>
#include <ctype.h>
#include <errno.h>

VEC_IMPLEMENT(VJson, vjson, Json, BY_REF, json_free);
//LUTD_IMPLEMENT(TJson, tjson, JsonKeyVal, BY_REF, json_key_val_hash, json_key_val_cmp, json_key_val_free);
LUTS_IMPLEMENT(TJson, tjson, Str, BY_REF, Json, BY_REF, str_hash, str_cmp, str_free, json_free);

#define ERR_static_json_parse_val(...) "failed parsing json value"
ErrDeclStatic static_json_parse_val(Json *json, Str *str);

int json_key_val_cmp(const JsonKeyVal *a, const JsonKeyVal *b)
{
    ASSERT_ARG(a);
    ASSERT_ARG(b);
    int result = str_cmp(&a->key, &b->key);
    return result;
}

size_t json_key_val_hash(const JsonKeyVal *json)
{
    ASSERT_ARG(json);
    size_t hash = str_hash(&json->key);
    return hash;
}

void json_key_val_free(JsonKeyVal *json)
{
    str_free(&json->key);
    json_free(&json->val);
}

void json_free(Json *json)
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

ErrDecl vjson_fmt(VJson *vjson, Str *str, JsonOptions *options)
{
    ASSERT_ARG(vjson);
    ASSERT_ARG(str);
    ASSERT_ARG(options);
    /* beginning of vector & spacing */
    TRYC(str_fmt(str, "%*s[\n", options->fmt.spacing, ""));
    options->fmt.spacing += options->fmt.tabs;
    /* format vector */
    for(size_t i = 0; i < vjson_length(vjson); ++i) {
        Json *json = vjson_get_at(vjson, i);
        /* actually format */
        if(i) {
            TRYC(str_fmt(str, ",\n"));
        }
        if(json->id != JSON_ARR && json->id != JSON_OBJ) {
            TRYC(str_fmt(str, "%*s", options->fmt.spacing, ""));
        }
        TRYC(json_fmt(json, str, options, 0));
    }
    /* spacing & end of vector */
    options->fmt.spacing -= options->fmt.tabs;
    TRYC(str_fmt(str, "\n%*s]", options->fmt.spacing, ""));
    return 0;
error:
    return -1;
}

ErrDecl tjson_fmt(TJson *tjson, Str *str, JsonOptions *options)
{
    ASSERT_ARG(tjson);
    ASSERT_ARG(str);
    ASSERT_ARG(options);
    /* beginning of object & spacing */
    TRYC(str_fmt(str, "%*s{\n", options->fmt.spacing, ""));
    options->fmt.spacing += options->fmt.tabs;
    /* format object */
    bool first = true;
#if 1
    for(size_t ii = 0; ii < LUTS_CAP(tjson->width); ++ii) {
        TJsonItem *jsonkv = tjson->buckets[ii];
        if(!jsonkv) continue;
        if(!first) {
            TRYC(str_fmt(str, ",\n"));
        }
        TRYC(str_fmt(str, "%*s\"%.*s\": ", options->fmt.spacing, "", STR_F(jsonkv->key)));
        TRYC(json_fmt(jsonkv->val, str, options, 0));
        first = false;
    }
#else
    for(size_t ii = 0; ii < (1ULL << (tjson->width - 1)); ++ii) {
        for(size_t jj = 0; jj < tjson->buckets[ii].len; ++jj) {
            if(!first) {
                TRYC(str_fmt(str, ",\n"));
            }
            JsonKeyVal *jsonkv = tjson->buckets[ii].items[jj];
            TRYC(str_fmt(str, "%*s\"%.*s\": ", options->fmt.spacing, "", STR_F(&jsonkv->key)));
            TRYC(json_fmt(&jsonkv->val, str, options, 0));
            first = false;
        }
    }
#endif
    /* spacing & end of object */
    options->fmt.spacing -= options->fmt.tabs;
    TRYC(str_fmt(str, "\n%*s}", options->fmt.spacing, ""));
    return 0;
error:
    return -1;
}

JsonOptions json_options_default(void)
{
    JsonOptions options = {
        .fmt.tabs = JSON_OPTIONS_TABS,
    };
    return options;
}

ErrDecl json_fmt(Json *json, Str *str, JsonOptions *options, Str *path)
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
            TRYC(vjson_fmt(&json->arr, str, options));
        } break;
        case JSON_STR: {
            TRYC(str_fmt(str, "\"%.*s\"", STR_F(&json->str)));
        } break;
        case JSON_OBJ: {
            TRYC(tjson_fmt(&json->obj, str, options));
        } break;
        case JSON_INT: {
            TRYC(str_fmt(str, "%lli", json->i));
        } break;
        case JSON_BOOL: {
            TRYC(str_fmt(str, "%s", json->b ? "true" : "false"));
        } break;
        case JSON_NONE: {
            THROW("can't format invalid json struct");
        } break;
        case JSON_NULL: {
            TRYC(str_fmt(str, "null"));
        } break;
        case JSON_FLOAT: {
            TRYC(str_fmt(str, "%lf", json->f));
        } break;
    }

    return 0;
error:
    return -1;
}

void static_json_parse_skip_ws(Str *str)
{
    while(str_length(str) && isspace(str_get_front(str))) {
        ++str->first;
    }
}

#define ERR_static_json_parse_detect(...) "failed detection of next value"
JsonList static_json_parse_detect(Json *json, Str *str)
{
    ASSERT_ARG(json);
    ASSERT_ARG(str);
    /* skip whitespace */
    static_json_parse_skip_ws(str);
    /* now detect */
    if(!str_length(str)) {
        return JSON_NONE;
    }
    char c = str_get_front(str);
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

#define ERR_static_json_parse_single_char(str, c) "failed parsing single character '%c'", c
ErrDeclStatic static_json_parse_single_char(Str *str, char c)
{
    /* verify begin */
    if(!(str_length(str) && str_get_front(str) == c)) {
        return -1;
    }
    ++str->first;
    return 0;
}
    

#define ERR_static_json_parse_arr(...) "failed parsing json array"
ErrDeclStatic static_json_parse_arr(Json *json, Str *str)
{
    ASSERT_ARG(json);
    ASSERT_ARG(str);
    /* verify begin */
    TRYC(static_json_parse_single_char(str, '['));
    /* create vector */
    json->id = JSON_ARR;
    for(;;) {
        /* parse value */
        Json val = {0};
        TRYC(static_json_parse_val(&val, str));
        if(val.id) {
            TRY(vjson_push_back(&json->arr, &val), ERR_VEC_PUSH_BACK);
        }
        /* check for next value */
        static_json_parse_skip_ws(str);
        if(static_json_parse_single_char(str, ',')) {
            break;
        }
    }
    /* verify end */
    static_json_parse_skip_ws(str);
    TRYC(static_json_parse_single_char(str, ']'));
    return 0;
error:
    return -1;
}

#define ERR_static_json_parse_str(...) "failed parsing json string"
ErrDeclStatic static_json_parse_str(Str *value, Str *str)
{
    ASSERT_ARG(value);
    ASSERT_ARG(str);
    /* verify begin */
    TRYC(static_json_parse_single_char(str, '"'));
    /* create string */
    //json->id = JSON_STR;
    Str val = {0};
    Str pending = *str;
    pending.last = pending.first;
    /* find end of string... TODO: rework */
    unsigned int escape = 0;
    while(str_length(str)) {
        char c = str_get_front(str);
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
                case 'u': { THROW("TODO:IMPLEMENT"); } break;
            }
            if(c2) {
                TRY(str_push_back(&val, c2), ERR_VEC_PUSH_BACK);
            }
            pending.first = str->first + 1;
            pending.last = pending.first;
        } else {
            if(c == '\\') {
                /* TODO: DRY vvvv */
                if(str_length(&pending)) {
                    TRYC(str_fmt(&val, "%.*s", STR_F(&pending)));
                    pending.first = pending.last;
                }
                /* TODO: DRY ^^^^ */
                escape = 1;
            } else if(c == '"') {
                /* TODO: DRY vvvv */
                if(str_length(&pending)) {
                    TRYC(str_fmt(&val, "%.*s", STR_F(&pending)));
                    pending.first = pending.last;
                }
                /* TODO: DRY ^^^^ */
                break;
            } else {
                ++pending.last;
                //TRY(str_push_back(&val, c), ERR_VEC_PUSH_BACK);
            }
        }
        ++str->first;
    }
    //val.last = str->first;
    /* verify end */
    static_json_parse_skip_ws(str);
    TRYC(static_json_parse_single_char(str, '"'));
    /* set string */
    *value = val;
    return 0;
error:
    return -1;
}

#define ERR_static_json_parse_obj(...) "failed parsing json object"
ErrDeclStatic static_json_parse_obj(Json *json, Str *str)
{
    ASSERT_ARG(json);
    ASSERT_ARG(str);
    /* verify begin */
    TRYC(static_json_parse_single_char(str, '{'));
    /* create string */
    json->id = JSON_OBJ;
    for(;;) {
#if 1
        Str key = {0};
        Json val = {0};
        /* parse string */
        static_json_parse_skip_ws(str);
        TRYC(static_json_parse_str(&key, str));
        /* parse colon */
        static_json_parse_skip_ws(str);
        TRYC(static_json_parse_single_char(str, ':'));
        /* parse value */
        TRYC(static_json_parse_val(&val, str));
        //if(!json->obj.width) {
            //TRY(tjson_init(&json->obj, 8), ERR_LUTD_INIT);
        //}
        if(val.id) {
            //TRY(tjson_add(&json->obj, &jsonkv), ERR_LUTD_ADD);
            //TRY(vjson_push_back(&json->arr, &&jsonkv.val), ERR_VEC_PUSH_BACK);
            //printff("  set [%.*s]", STR_F(&key));
            TRY(tjson_set(&json->obj, &key, &val), ERR_LUTD_ADD);
            //printff("  ... ok");
#else
        JsonKeyVal jsonkv = {0};
        /* parse string */
        static_json_parse_skip_ws(str);
        TRYC(static_json_parse_str(&jsonkv.key, str));
        /* parse colon */
        static_json_parse_skip_ws(str);
        TRYC(static_json_parse_single_char(str, ':'));
        /* parse value */
        TRYC(static_json_parse_val(&jsonkv.val, str));
        if(!json->obj.width) {
            TRY(tjson_init(&json->obj, 8), ERR_LUTD_INIT);
        }
        if(jsonkv.val.id) {
            TRY(tjson_add(&json->obj, &jsonkv), ERR_LUTD_ADD);
            //TRY(vjson_push_back(&json->arr, &&jsonkv.val), ERR_VEC_PUSH_BACK);
#endif
        }
        /* check for next value */
        static_json_parse_skip_ws(str);
        if(static_json_parse_single_char(str, ',')) {
            break;
        }
    }
    /* verify end */
    static_json_parse_skip_ws(str);
    TRYC(static_json_parse_single_char(str, '}'));
    return 0;
error:
    return -1;
}

#define ERR_static_json_parse_num(...) "failed parsing json num"
ErrDeclStatic static_json_parse_num(Json *json, Str *str)
{
    ASSERT_ARG(json);
    ASSERT_ARG(str);
    /* determine number format */
    bool is_float = false;
    Str val = *str;
    /* may be positive or negative */
    (void) static_json_parse_single_char(&val, '-'); // TODO: make way to remove (void)
    /* if the number begins with 0, instantly mark as float */
    if(str_length(&val) && str_get_front(&val) == '0') {
        is_float = true;
    } else if(str_length(&val)) {
        ++val.first;
        for(size_t i = 0; i < str_length(&val); ++i) {
            char c = str_get_front(&val);
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
        char *endptr = 0;
        char cstr[256];
        str_cstr(str, cstr, 256);
        errno = 0;
        double f = strtod(cstr, &endptr); // TODO: not really quite official json
        if(errno) {
            THROW("strtod() failed: %s", strerror(errno));
        }
        json->id = JSON_FLOAT;
        json->f = f;
        str->first += (endptr - cstr);
        return 0;
    } else {
        char *endptr = 0;
        char cstr[256];
        str_cstr(str, cstr, 256);
        errno = 0;
        int i = strtol(cstr, &endptr, 0); // TODO: not really quite official json
        if(errno) {
            THROW("strtol() failed: %s", strerror(errno));
        }
        json->id = JSON_INT;
        json->i = i;
        str->first += (endptr - cstr);
        return 0;
    }
    THROW("couldn't parse number");
error:
    return -1;
}

#define ERR_static_json_parse_bool(...) "failed parsing json bool"
ErrDeclStatic static_json_parse_bool(Json *json, Str *str)
{
    ASSERT_ARG(json);
    ASSERT_ARG(str);
    Str t = STR("true");
    Str f = STR("false");
    if(str_length(str) >= str_length(&t) && !str_cmp(&STR_IE(*str, str_length(&t)), &t)) {
        json->id = JSON_BOOL;
        json->b = true;
        str->first += str_length(&t);
        return 0;
    }
    if(str_length(str) >= str_length(&f) && !str_cmp(&STR_IE(*str, str_length(&f)), &f)) {
        json->id = JSON_BOOL;
        json->b = false;
        str->first += str_length(&f);
        return 0;
    }
    THROW("'t' or 'f' character found, expecting 'true' or 'false', nothing found");
error:
    return -1;
}

#define ERR_static_json_parse_null(...) "failed parsing json null"
ErrDeclStatic static_json_parse_null(Json *json, Str *str)
{
    ASSERT_ARG(json);
    ASSERT_ARG(str);
    Str n = STR("null");
    if(str_length(str) <= str_length(&n) && !str_cmp(&STR_IE(*str, str_length(&n)), &n)) {
        json->id = JSON_NULL;
        str->first += str_length(&n);
        return 0;
    }
    THROW("'n' found, expecting 'null', nothing found");
error:
    return -1;
}

ErrDeclStatic static_json_parse_val(Json *json, Str *str)
{
    ASSERT_ARG(json);
    ASSERT_ARG(str);
    JsonList id = static_json_parse_detect(json, str);
    switch(id) {
        case JSON_FLOAT: // redundant
        case JSON_INT: {
            //printff("PARSING NUM [%.50s]", str_iter_begin(str));
            TRYC(static_json_parse_num(json, str));
        } break;
        case JSON_ARR: {
            //printff("PARSING ARR [%.50s]", str_iter_begin(str));
            TRYC(static_json_parse_arr(json, str));
        } break;
        case JSON_OBJ: {
            //printff("PARSING OBJ [%.50s]", str_iter_begin(str));
            TRYC(static_json_parse_obj(json, str));
        } break;
        case JSON_BOOL: {
            //printff("PARSING BOOL [%.50s]", str_iter_begin(str));
            TRYC(static_json_parse_bool(json, str));
        } break;
        case JSON_NULL: {
            //printff("PARSING NULL [%.50s]", str_iter_begin(str));
            TRYC(static_json_parse_null(json, str));
        } break;
        case JSON_STR: {
            //printff("PARSING STR [%.50s]", str_iter_begin(str));
            TRYC(static_json_parse_str(&json->str, str));
            json->id = id;
        } break;
        case JSON_NONE: {} break;
    }
    return 0;
error:
    return -1;
}

ErrDecl json_parse(Json *json, Str *str, JsonOptions *options, Str *path)
{
    ASSERT_ARG(json);
    ASSERT_ARG(str);

    JsonOptions opts = {0};
    if(!options) {
        opts = json_options_default();
        options = &opts;
    };

    Str parsing = *str;

    do {
        TRYC(static_json_parse_val(json, &parsing));
    } while(str_length(&parsing));

    return 0;
error:
    return -1;
}

ErrDecl json_get(Json *json, Json *out, JsonPath *path)
{
    ASSERT_ARG(json);
    ASSERT_ARG(out);
    ASSERT_ARG(path); // -> if no path just ..return 0 if a valid json all in all lol

    Json input = *json;
    Json *result = &input;

    for(size_t i = 0; i < path->n; ++i) {
        switch(result->id) {
            case JSON_OBJ: {
#if 1
                result = tjson_get(&result->obj, &path->p[i].k);
#else
                JsonKeyVal search = { .key = path->p[i].k };
                size_t ii, jj;
                TRY(tjson_find(&result->obj, &search, &ii, &jj), ERR_LUTD_FIND " '%.*s'", STR_F(&search.key));
                result = &result->obj.buckets[ii].items[jj]->val;
#endif
            } break;
            case JSON_ARR: {
                result = vjson_get_at(&result->arr, path->p[i].i);
            } break;
            default: {
            } break;
        }
    }
    *out = *result;
    return 0;
error:
    return -1;
}


