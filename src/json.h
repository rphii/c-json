#ifndef JSON_H

#include "err.h"
#include "str.h"
#include "vec.h"
#include "lutd.h"
#include "luts.h"
#include "vstr.h"

typedef enum {
    JSON_NONE,      // keep as first
    JSON_STR,
    JSON_INT,
    JSON_OBJ,
    JSON_ARR,
    JSON_NULL,
    JSON_BOOL,
    JSON_FLOAT,
} JsonList;

typedef struct Json Json;
typedef struct TJson TJson;
typedef struct JsonKeyVal JsonKeyVal;

VEC_INCLUDE(VJson, vjson, Json, BY_REF);
LUTS_INCLUDE(TJson, tjson, Str, BY_REF, Json, BY_REF);

//VEC_INCLUDE(TJson, vjson, TJson, BY_REF);
//LUTD_INCLUDE(TJson, tjson, JsonKeyVal, BY_REF);

typedef struct Json {
    JsonList id;
    union {
        Str str;
        VJson arr;  // vector of json BY_REF
        TJson obj;  // lookup of json BY_REF
        bool b;
        int i;
        double f;
    };
} Json;

typedef struct JsonKeyVal {
    Str key;
    Json val;
} JsonKeyVal;

typedef union JsonPathlet {
    Str k;
    size_t i;
} JsonPathlet;

typedef struct JsonPath {
    size_t n;
    JsonPathlet *p;
} JsonPath;

typedef struct JsonOptions {
    struct {
        bool sort_array;
    } parse;
    struct {
        int tabs;
        int spacing; // used internally tbh
        int level;
        bool sort_object;
        bool compact; // TODO rename/add_newlines???
    } fmt;
    //struct {
    //} x; // internal struct
} JsonOptions;
 
#define JSON_OPTIONS_TABS   (2)

JsonOptions json_options_default(void);

int json_key_val_cmp(const JsonKeyVal *a, const JsonKeyVal *b);
size_t json_key_val_hash(const JsonKeyVal *json);
void json_key_val_free(JsonKeyVal *json);

void json_sort(Json *json);
void vjson_sort(VJson *vjson);

#define ERR_vjson_fmt(...) "failed formatting json vector"
ErrDecl vjson_fmt(VJson *vjson, Str *str, JsonOptions *options);
#define ERR_tjson_fmt(...) "failed formatting json vector"
ErrDecl tjson_fmt(TJson *tjson, Str *str, JsonOptions *options);

void json_free(Json *json);

#define ERR_json_fmt(...) "failed formatting json"
ErrDecl json_fmt(Json *json, Str *str, JsonOptions *options, Str *path);
#define ERR_json_parse(...) "failed parsing json"
ErrDecl json_parse(Json *json, Str *str, JsonOptions *options, Str *path);

#define ERR_json_get(...) "failed getting json path"
ErrDecl json_get(Json *json, Json *out, JsonPath *path);
//Json json_get(Json *json, Str *path);

ErrDecl json_set_str(Json *json, bool val, Str *path);
ErrDecl json_set_obj(Json *json, Json , Str *path);
ErrDecl json_set_arr(Json *json, bool val, Str *path);
ErrDecl json_set_int(Json *json, int val, Str *path);
ErrDecl json_set_null(Json *json, bool val, Str *path);
ErrDecl json_set_bool(Json *json, bool val, Str *path);
ErrDecl json_set_float(Json *json, double val, Str *path);



#define JSON_H
#endif

