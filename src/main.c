//usr/bin/env tcc -DPROC_COUNT=4 $(ls *.c | grep -v main.c) -run "$0" "$@" ; exit $?

#include <stdio.h>
#include <math.h>

#include "str.h"

#include "info.h"
#include "file.h"
#include "json.h"

typedef struct ListTags {
    Str out;
    size_t n;
} ListTags;

void list_not_downloaded(void)
{
    Str out = {0};
    Str in = {0};
    Str filename = {0};
    size_t index = 0;

    Json ids = {0};
    ids.id = JSON_OBJ;

    Json val = {0};
    val.id = JSON_STR;

#if 1
    for(size_t i = 1; i <= 41; ++i) {
        Json json = {0};
        str_clear(&in);
        str_clear(&filename);
        TRYC(str_fmt(&filename, "/home/rphii/dev/c/wallhaven-tags/favorites/fav_%zu.json", i));
        //printff("[%zu] %.*s", i, STR_F(&filename));
        TRYC(file_str_read(&filename, &in));
        TRYC(json_parse(&json, &in, 0, 0));

        Json array = json_get(&json, &(JsonPath){ .n = 1, .p = (JsonPathlet[]){
                (JsonPathlet){.k = STR("data")},
            }});

        for(size_t i = 0; i < vjson_length(&array.arr); ++i) {
            //printff("%zu", id.id);
            Json *item = vjson_get_at(&array.arr, i);
            Json id = json_get(item, &(JsonPath){ .n = 1, .p = (JsonPathlet[]){
                    (JsonPathlet){.k = STR("id")},
                }});
            Json url = json_get(item, &(JsonPath){ .n = 1, .p = (JsonPathlet[]){
                    (JsonPathlet){.k = STR("path")},
                }});

            //Str x = {0};
            //json_fmt(&id, &x, 0, 0);
            //printf(">>>%.*s\n", STR_F(&x));
            //str_free(&x);
            str_zero(&val.str);
            TRYC(str_fmt(&val.str, "%.*s", STR_F(&url.str)));
            Str key = {0};
            TRYC(str_fmt(&key, "%.*s", STR_F(&id.str)));
            //tjson_set(&

            /*Json url = json_get(&json, &(JsonPath){ .n = 2, .p = (JsonPathlet[]){
              (JsonPathlet){.k = STR("data")},
              (JsonPathlet){.k = STR("url")},
              }});*/

            TRY(tjson_set(&ids.obj, &key, &val), "failed setting lut");
        }

        json_free(&json);
        //break;
    }
    str_free(&filename);

    /* go over shit */
    VStr files = {0};
    TRYC(file_dir_read(&STR("/home/rphii/Images/Wallhaven/totag"), &files));
    Str basename = {0};
    for(size_t i = 0; i < vstr_length(&files); ++i) {
        Str *file = vstr_get_at(&files, i);
        str_free(&basename);
        TRYC(rstr_extend_back(&basename, str_get_basename(file)));
        Str cmp = STR("wallhaven-");
        if(str_cmp(&STR_IE(basename, str_length(&cmp)), &cmp)) {
            continue;
        }
        Str id = STR_I0(basename, str_length(&cmp));
        Json *have = tjson_get(&ids.obj, &id);
        if(!have) {
            //printff("%u keep : %.*s %p", ids.id, STR_F(&id), have);
            continue;
        }
        tjson_del(&ids.obj, &id);
    }
    str_free(&basename);
    vstr_free(&files);

#endif

    /* format */

    //TRYC(json_fmt(&ids, &out, 0, 0));
    //printf("%.*s\n", STR_F(&out));
    for(size_t i = 0; i < LUTS_CAP(ids.obj.width); ++i) {
        TJsonItem *item = ids.obj.buckets[i];
        if(!item) continue;
        Json *val = item->val;
        if(!val) continue;
        if(item->hash == LUTS_EMPTY) continue;
        printf("%.*s\n", STR_F(&val->str));
    }

error:
    return;
}

//#include <ctype.h>

ErrDecl list_tags(Str *filename, void *args)
{
    RStr basename = str_get_basename(filename);
    RStr begin = RSTR("wallhaven-");
    if(rstr_length(&basename) < rstr_length(&begin)) return 0;

    RStr starts = RSTR_IE(basename, rstr_length(&begin));
    if(rstr_cmp(&starts, &begin)) return 0;

    RStr ext = str_get_ext(filename);
    if(rstr_cmp(&ext, &RSTR(".json"))) return 0;

    ListTags *lt = args;
    Str *out = &lt->out;

    Str in = {0};
    //Str out = {0};
    //for(size_t i = 0; i <= SIZE_ARRAY(files)-1; ++i) {
        str_clear(&in);
        //Str *filename = &files[i];
        //printff("[%zu] parsing '%.*s'", i, STR_F(filename));
        //Str fname = STR_LL(filename.s, rstr_length(&filename));
        TRYC(file_str_read(filename, &in));
        Json json = {0};
        TRYC(json_parse(&json, &in, 0, 0));

        Json name = json_get(&json, &(JsonPath){ .n = 2, .p = (JsonPathlet[]){
                (JsonPathlet){.k = STR("data")},
                (JsonPathlet){.k = STR("path")},
            }});

        Json array = json_get(&json, &(JsonPath){ .n = 2, .p = (JsonPathlet[]){
                (JsonPathlet){.k = STR("data")},
                (JsonPathlet){.k = STR("tags")},
            }});

        TRYC(rstr_extend_back(out, str_get_nodir(&name.str)));

        for(size_t i = 0; i < vjson_length(&array.arr); ++i) {
            TRYC(str_fmt(out, ","));
            Json *tag = vjson_get_at(&array.arr, i);
            Json name = json_get(tag, &(JsonPath){.n = 1, .p = (JsonPathlet[]){(JsonPathlet){.k = STR("name")}}});
            Json purity = json_get(tag, &(JsonPath){.n = 1, .p = (JsonPathlet[]){(JsonPathlet){.k = STR("purity")}}});
            Json category = json_get(tag, &(JsonPath){.n = 1, .p = (JsonPathlet[]){(JsonPathlet){.k = STR("category")}}});
            if(str_cmp(&purity.str, &STR("sfw"))) {
                TRYC(str_fmt(out, "%.*s:%.*s:%.*s", STR_F(&purity.str), STR_F(&category.str), STR_F(&name.str)));
            } else {
                TRYC(str_fmt(out, "%.*s:%.*s", STR_F(&category.str), STR_F(&name.str)));
            }
        }

        json_free(&json);
        TRYC(str_fmt(out, "\n"));
    //}
    //printf("%.*s", STR_F(&out));
    //str_free(&in);
    //str_free(&out);

    ++lt->n;

    return 0;
error:
    return -1;
}

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

        JsonPath path_url = (JsonPath){ .n = 2, .p = (JsonPathlet []){
                    (JsonPathlet){.i = i},
                    (JsonPathlet){.k = STR("id")},
                }
        };
        Json url = json_get(&data, &path_url);
        
        //TRYC(json_fmt(&portion, &out, 0, 0));
        //TRYC(str_fmt(out, "[%zu] ", (*index)++));
        //TRYC(json_fmt(&url, out, 0, 0));
        TRYC(str_fmt(out, "    '%.*s'", STR_F(&url.str)));
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

#if 0
    info_disable_all(INFO_LEVEL_ALL);
    info_enable_all(INFO_LEVEL_TEXT);

    Json json = {0};
    Str lines = {0};
    Str filename = STR("../jsonfiles.txt");
    TRYC(file_str_read(&filename, &lines));
    Str line = {.s = lines.s};
    size_t line_nr = 1;
    for(;;) {
#if 0
        str_get_line(&lines, &line.first, &line.last);
        if(line.first >= line.last) break;
        if(str_get_front(&line) != '#') {
            info(INFO_parsing_file, "%zu:%.*s", line_nr, STR_F(&line));
            str_clear(&in);
            str_clear(&out);
            TRYC(file_str_read(&line, &in));
            if(!json_parse(&json, &in, 0, 0)) {
                info_check(INFO_parsing_file, true);
                //if(!json_fmt(&json, &out, 0, 0)) {
                //    printf("%.*s\n", STR_F(&out));
                //}
            } else {
                info_check(INFO_parsing_file, false);
            }
            json_free(&json);
        }
        ++line_nr;
        line.first = line.last + 1;
#endif
    }
    str_free(&lines);
#endif

#if 0
    //Str filename = STR("../data/sample.json");
    //Str filename = STR("../data/512KB.json");
    //Str filename = STR("../data/5MB-min.json");
    //Str filename = STR("../testfiles/instruments.json");

    Str files[] = {
        //STR("../data/missing-colon.json"),
        //STR("../data/test.json"),
        STR("../data/5MB-min.json"),
        //STR("../testfiles/apache_builds.json"),
        //STR("../testfiles/canada.json"),
        //STR("../testfiles/citm_catalog.json"),
        //STR("../testfiles/github_events.json"),
        //STR("../testfiles/gsoc-2018.json"),
        //STR("../testfiles/instruments.json"),
        //STR("../testfiles/marine_ik.json"),
        //STR("../testfiles/mesh.json"),
        //STR("../testfiles/mesh.pretty.json"),
        //STR("../testfiles/numbers.json"),
        //STR("../testfiles/random.json"),
        //STR("../testfiles/twitter.json"),
        //STR("../testfiles/twitterescaped.json"),
        //STR("../testfiles/update-center.json"),
        //STR("/home/rphii/.local/share/Steam/steamapps/compatdata/1325200/pfx/drive_c/users/steamuser/Desktop/b2b-h2y-y2h.json"),
        //STR("/home/rphii/.local/share/Steam/steamapps/compatdata/1325200/pfx/drive_c/users/steamuser/Desktop/boss2boss-yokai2yokai-human2humans.json"),
        //STR("/home/rphii/.local/share/Steam/steamapps/compatdata/1325200/pfx/drive_c/users/steamuser/Desktop/default.json"),
        //STR("/home/rphii/.local/share/Steam/steamapps/compatdata/1325200/pfx/drive_c/users/steamuser/Desktop/default2.json"),
        //STR("/home/rphii/.local/share/Steam/steamapps/compatdata/1325200/pfx/drive_c/users/steamuser/Desktop/empty.json"),
        //STR("/home/rphii/.local/share/Steam/steamapps/compatdata/1325200/pfx/drive_c/users/steamuser/Desktop/hb2yb-yb2hb-ye2he-he2ye.json"),
        //STR("/home/rphii/.local/share/Steam/steamapps/compatdata/1325200/pfx/drive_c/users/steamuser/Desktop/largefile-original.json"),
        //STR("/home/rphii/.local/share/Steam/steamapps/compatdata/1325200/pfx/drive_c/users/steamuser/Desktop/largefile.json"),
        //STR("/home/rphii/.local/share/Steam/steamapps/compatdata/1325200/pfx/drive_c/users/steamuser/Desktop/test-v0.0.13.json"),
        //STR("/home/rphii/.local/share/Steam/steamapps/compatdata/1325200/pfx/drive_c/users/steamuser/Desktop/test.json"),
        //STR("/home/rphii/.local/share/Steam/steamapps/compatdata/1325200/pfx/drive_c/users/steamuser/Desktop/testing.json"),
        //STR("/home/rphii/.local/share/Steam/steamapps/compatdata/1325200/pfx/drive_c/users/steamuser/Desktop/wtf-2786327771-hollow.json"),
    };
    
    for(size_t i = 0; i < SIZE_ARRAY(files); ++i) {
        str_clear(&in);
        printff("%.*s", STR_F(&files[i]));
        TRYC(file_str_read(&files[i], &in));
        Json json = {0};
        TRYC(json_parse(&json, &in, 0, 0));

        str_clear(&out);
        TRYC(json_fmt(&json, &out, 0, 0));
        printf("%.*s\n", STR_F(&out));

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

    str_clear(&out);
    VStr dirfiles = {0};
    Str input = STR("/home/rphii/dev/c/wallhaven-tags/");
    ListTags lt = {0};
    TRYC(file_exec(&input, &dirfiles, true, list_tags, &lt));
    while(vstr_length(&dirfiles)) {
        vstr_pop_back(&dirfiles, &input);
        memset(dirfiles.items[vstr_length(&dirfiles)], 0, sizeof(Str)); // TODO: this should probably happen in my vector!
        TRYC(file_exec(&input, &dirfiles, true, list_tags, &lt));
        str_free(&input);
    }
    printf("%.*s", STR_F(&lt.out));
    printff("========\nprocessed %zu files", lt.n);
    str_clear(&out);

    //list_not_downloaded();
#if 0
    Str filename = {0};
    size_t index = 0;
    for(size_t i = 1; i <= 25; ++i) {
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

