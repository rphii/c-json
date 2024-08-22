//usr/bin/env tcc -DPROC_COUNT=4 $(ls *.c | grep -v main.c) -run "$0" "$@" ; exit $?

#include <stdio.h>
#include <math.h>

#include "str.h"

#include "file.h"
#include "json.h"

//#include <ctype.h>

ErrDecl list_urls(Str *filename, Str *in, Str *out, size_t *index)
{
    //Str filename = STR("/home/rphii/dev/c/wallhaven-tags/favorites.json");

    int err = 0;

    str_clear(in);
    TRYC(file_str_read(filename, in));
    Json json = {0};
    TRYC(json_parse(&json, in, 0, 0));

    JsonPath path_data = (JsonPath){
        .n = 1,
            .p = (JsonPathlet []){
                (JsonPathlet){.k = STR("data")},
            }
    };

    Json data = json_get(&json, &path_data);
    for(size_t i = 0; i < vjson_length(&data.arr); ++i) {

        JsonPath path_url = (JsonPath){
            .n = 2,
                .p = (JsonPathlet []){
                    (JsonPathlet){.i = i},
                    (JsonPathlet){.k = STR("short_url")},
                }
        };
        Json url = json_get(&data, &path_url);
        
        //TRYC(json_fmt(&portion, &out, 0, 0));
        TRYC(str_fmt(out, "[%zu] ", (*index)++));
        TRYC(json_fmt(&url, out, 0, 0));
        TRYC(str_fmt(out, "\n"));

    }

clean:
    json_free(&json);
    return err;

    return 0;
error:
    return -1;
}

int main(int argc, const char **argv)
{
    int err = 0;
    Str in = {0};
    Str out = {0};

#if 0
    Json json = {0};
    TRYC(json_parse(&json, &STR("[]"), 0, 0));
    TRYC(json_fmt(&json, &out, 0, 0));
    printf("%.*s\n", STR_F(&out));
    json_free(&json);
#endif

#if 1
    //Str filename = STR("../data/sample.json");
    //Str filename = STR("../data/512KB.json");
    //Str filename = STR("../data/5MB-min.json");
    //Str filename = STR("../testfiles/instruments.json");

    Str files[] = {
        STR("../testfiles/apache_builds.json"),
        STR("../testfiles/canada.json"),
        STR("../testfiles/citm_catalog.json"),
        STR("../testfiles/github_events.json"),
        STR("../testfiles/gsoc-2018.json"),
        STR("../testfiles/instruments.json"),
        STR("../testfiles/marine_ik.json"),
        STR("../testfiles/mesh.json"),
        STR("../testfiles/mesh.pretty.json"),
        STR("../testfiles/numbers.json"),
        STR("../testfiles/random.json"),
        STR("../testfiles/twitter.json"),
        STR("../testfiles/twitterescaped.json"),
        STR("../testfiles/update-center.json"),
    };
    
    for(size_t i = 0; i < SIZE_ARRAY(files); ++i) {
        str_clear(&in);
        printff("%.*s", STR_F(&files[i]));
        TRYC(file_str_read(&files[i], &in));
        Json json = {0};
        TRYC(json_parse(&json, &in, 0, 0));
        //TRYC(json_fmt(&json, &out, 0, 0));
        //printf("%.*s\n", STR_F(&out));
        json_free(&json);

    }
    //TRYC(file_str_read(&filename, &in));
    //TRYC(json_fmt(&json, &out, 0, 0));
    //printf("%.*s\n", STR_F(&out));
#endif

#if 0
    Json json = {
        .id = JSON_ARR,
        .arr = (VJson){
            .last = 6,
            .items = (Json *[]){
                &(Json){.id = JSON_INT, .i = 123},
                &(Json){.id = JSON_FLOAT, .f = M_PI},
                &(Json){.id = JSON_BOOL, .b = false},
                &(Json){.id = JSON_BOOL, .b = true},
                &(Json){.id = JSON_STR, .str = STR("hello, world!")},
                &(Json){.id = JSON_OBJ, .obj = (TJson){
                    .width = 0xFF, .used = 2,
                    .buckets = (TJsonItem *[]){
                        &(TJsonItem){
                            .key = &STR("user"),
                            .val = &(Json){.id = JSON_STR, .str = STR("coder")},
                        },
                        &(TJsonItem){
                            .key = &STR("age"),
                            .val = &(Json){.id = JSON_INT, .i = 23},
                        },
                    },
                }},
            },
        },
    };
    TRYC(json_fmt(&json, &out, 0, 0));
    printf("%.*s\n", STR_F(&out));

#endif

#if 0
    Str filename = {0};
    size_t index = 0;
    for(size_t i = 1; i <= 24; ++i) {
        str_clear(&filename);
        TRYC(str_fmt(&filename, "/home/rphii/dev/c/wallhaven-tags/favorites/fav_%zu.json", i));
        printff("[%zu] %.*s", i, STR_F(&filename));
        TRY(list_urls(&filename, &in, &out, &index), "failed jsoning");
        //break;
    }
    str_free(&filename);
    printf("%.*s", STR_F(&out));
#endif

clean:
    str_free(&in);
    str_free(&out);
    return err;
error:
    ERR_CLEAN;
}

