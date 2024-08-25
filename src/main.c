//usr/bin/env tcc -DPROC_COUNT=4 $(ls *.c | grep -v main.c) -run "$0" "$@" ; exit $?

#include <stdio.h>
#include <math.h>

#include "str.h"

#include "file.h"
#include "json.h"

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

    for(size_t i = 1; i <= 25; ++i) {
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
        TRYC(str_fmt_basename(&basename, file));
        Str cmp = STR("wallhaven-");
        if(str_cmp(&STR_IE(basename, str_length(&cmp)), &cmp)) {
            continue;
        }
        Str id = STR_I0(basename, str_length(&cmp));
        Json *have = tjson_get(&ids.obj, &id);
        if(!have) {
            printff("%u keep : %.*s %p", ids.id, STR_F(&id), have);
            continue;
        }
        tjson_del(&ids.obj, &id);
    }
    str_free(&basename);
    vstr_free(&files);


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

ErrDecl list_tags(void)
{
    Str files[] = {
        STR("/home/rphii/dev/c/wallhaven-tags/favorites/wallhaven-01wg3v.json"),
        STR("/home/rphii/dev/c/wallhaven-tags/favorites/wallhaven-0jl5vp.json"),
        STR("/home/rphii/dev/c/wallhaven-tags/favorites/wallhaven-0qkko7.json"),
        STR("/home/rphii/dev/c/wallhaven-tags/favorites/wallhaven-13657g.json"),
        STR("/home/rphii/dev/c/wallhaven-tags/favorites/wallhaven-13g5zv.json"),
        STR("/home/rphii/dev/c/wallhaven-tags/favorites/wallhaven-13jg33.json"),
        STR("/home/rphii/dev/c/wallhaven-tags/favorites/wallhaven-13m7yv.json"),
        STR("/home/rphii/dev/c/wallhaven-tags/favorites/wallhaven-1jee1v.json"),
        STR("/home/rphii/dev/c/wallhaven-tags/favorites/wallhaven-1jm9wv.json"),
        STR("/home/rphii/dev/c/wallhaven-tags/favorites/wallhaven-1k1z39.json"),
        STR("/home/rphii/dev/c/wallhaven-tags/favorites/wallhaven-1k2qm1.json"),
        STR("/home/rphii/dev/c/wallhaven-tags/favorites/wallhaven-1kpk8g.json"),
        STR("/home/rphii/dev/c/wallhaven-tags/favorites/wallhaven-1kryxg.json"),
        STR("/home/rphii/dev/c/wallhaven-tags/favorites/wallhaven-1kv8j3.json"),
        STR("/home/rphii/dev/c/wallhaven-tags/favorites/wallhaven-1kvj33.json"),
        STR("/home/rphii/dev/c/wallhaven-tags/favorites/wallhaven-1p2m2w.json"),
        STR("/home/rphii/dev/c/wallhaven-tags/favorites/wallhaven-1p377g.json"),
        STR("/home/rphii/dev/c/wallhaven-tags/favorites/wallhaven-1p8lq3.json"),
        STR("/home/rphii/dev/c/wallhaven-tags/favorites/wallhaven-1pgpr9.json"),
        STR("/home/rphii/dev/c/wallhaven-tags/favorites/wallhaven-1pol63.json"),
        STR("/home/rphii/dev/c/wallhaven-tags/favorites/wallhaven-1pqq1w.json"),
        STR("/home/rphii/dev/c/wallhaven-tags/favorites/wallhaven-1py5j1.json"),
        STR("/home/rphii/dev/c/wallhaven-tags/favorites/wallhaven-1pyk79.json"),
        STR("/home/rphii/dev/c/wallhaven-tags/favorites/wallhaven-1pyp7v.json"),
        STR("/home/rphii/dev/c/wallhaven-tags/favorites/wallhaven-289z6y.json"),
        STR("/home/rphii/dev/c/wallhaven-tags/favorites/wallhaven-28rr6y.json"),
        STR("/home/rphii/dev/c/wallhaven-tags/favorites/wallhaven-28ryjm.json"),
        STR("/home/rphii/dev/c/wallhaven-tags/favorites/wallhaven-28wjr6.json"),
        STR("/home/rphii/dev/c/wallhaven-tags/favorites/wallhaven-28y9yx.json"),
        STR("/home/rphii/dev/c/wallhaven-tags/favorites/wallhaven-2eomd6.json"),
        STR("/home/rphii/dev/c/wallhaven-tags/favorites/wallhaven-2eqwpm.json"),
        STR("/home/rphii/dev/c/wallhaven-tags/favorites/wallhaven-2y2o1m.json"),
        STR("/home/rphii/dev/c/wallhaven-tags/favorites/wallhaven-2y68yy.json"),
        STR("/home/rphii/dev/c/wallhaven-tags/favorites/wallhaven-2y6pgy.json"),
        STR("/home/rphii/dev/c/wallhaven-tags/favorites/wallhaven-2yd7lx.json"),
        STR("/home/rphii/dev/c/wallhaven-tags/favorites/wallhaven-2ykz7y.json"),
        STR("/home/rphii/dev/c/wallhaven-tags/favorites/wallhaven-2yl8rg.json"),
        STR("/home/rphii/dev/c/wallhaven-tags/favorites/wallhaven-2ylx3g.json"),
        STR("/home/rphii/dev/c/wallhaven-tags/favorites/wallhaven-2yvyz9.json"),
        STR("/home/rphii/dev/c/wallhaven-tags/favorites/wallhaven-2ywo9g.json"),
        STR("/home/rphii/dev/c/wallhaven-tags/favorites/wallhaven-2yx2w9.json"),
        STR("/home/rphii/dev/c/wallhaven-tags/favorites/wallhaven-34xwv0.json"),
        STR("/home/rphii/dev/c/wallhaven-tags/favorites/wallhaven-395ved.json"),
        STR("/home/rphii/dev/c/wallhaven-tags/favorites/wallhaven-39y5k9.json"),
        STR("/home/rphii/dev/c/wallhaven-tags/favorites/wallhaven-39y6y6.json"),
        STR("/home/rphii/dev/c/wallhaven-tags/favorites/wallhaven-3l1oqy.json"),
        STR("/home/rphii/dev/c/wallhaven-tags/favorites/wallhaven-3l26qd.json"),
        STR("/home/rphii/dev/c/wallhaven-tags/favorites/wallhaven-3l27wd.json"),
        STR("/home/rphii/dev/c/wallhaven-tags/favorites/wallhaven-3l69pv.json"),
        STR("/home/rphii/dev/c/wallhaven-tags/favorites/wallhaven-3l6d6y.json"),
        STR("/home/rphii/dev/c/wallhaven-tags/favorites/wallhaven-3l6ex6.json"),
        STR("/home/rphii/dev/c/wallhaven-tags/favorites/wallhaven-3l6x1d.json"),
        STR("/home/rphii/dev/c/wallhaven-tags/favorites/wallhaven-3l7x2v.json"),
        STR("/home/rphii/dev/c/wallhaven-tags/favorites/wallhaven-3l7x7v.json"),
        STR("/home/rphii/dev/c/wallhaven-tags/favorites/wallhaven-3l8w56.json"),
        STR("/home/rphii/dev/c/wallhaven-tags/favorites/wallhaven-3l9g2v.json"),
        STR("/home/rphii/dev/c/wallhaven-tags/favorites/wallhaven-3lgply.json"),
        STR("/home/rphii/dev/c/wallhaven-tags/favorites/wallhaven-3lj1qv.json"),
        STR("/home/rphii/dev/c/wallhaven-tags/favorites/wallhaven-3lj226.json"),
        STR("/home/rphii/dev/c/wallhaven-tags/favorites/wallhaven-3lkxpy.json"),
        STR("/home/rphii/dev/c/wallhaven-tags/favorites/wallhaven-3lrmm9.json"),
        STR("/home/rphii/dev/c/wallhaven-tags/favorites/wallhaven-3lv37y.json"),
        STR("/home/rphii/dev/c/wallhaven-tags/favorites/wallhaven-3lv52y.json"),
        STR("/home/rphii/dev/c/wallhaven-tags/favorites/wallhaven-3lv5ev.json"),
        STR("/home/rphii/dev/c/wallhaven-tags/favorites/wallhaven-3lvo2d.json"),
        STR("/home/rphii/dev/c/wallhaven-tags/favorites/wallhaven-3lvw56.json"),
        STR("/home/rphii/dev/c/wallhaven-tags/favorites/wallhaven-3zjo7d.json"),
        STR("/home/rphii/dev/c/wallhaven-tags/favorites/wallhaven-3zmrk3.json"),
        STR("/home/rphii/dev/c/wallhaven-tags/favorites/wallhaven-3zqvm9.json"),
        STR("/home/rphii/dev/c/wallhaven-tags/favorites/wallhaven-42lovm.json"),
        STR("/home/rphii/dev/c/wallhaven-tags/favorites/wallhaven-42p26x.json"),
        STR("/home/rphii/dev/c/wallhaven-tags/favorites/wallhaven-43mr2v.json"),
        STR("/home/rphii/dev/c/wallhaven-tags/favorites/wallhaven-45o1l8.json"),
        STR("/home/rphii/dev/c/wallhaven-tags/favorites/wallhaven-48w1j2.json"),
        STR("/home/rphii/dev/c/wallhaven-tags/favorites/wallhaven-49e2mx.json"),
        STR("/home/rphii/dev/c/wallhaven-tags/favorites/wallhaven-4dlq1j.json"),
        STR("/home/rphii/dev/c/wallhaven-tags/favorites/wallhaven-4dodkl.json"),
        STR("/home/rphii/dev/c/wallhaven-tags/favorites/wallhaven-4dqepg.json"),
        STR("/home/rphii/dev/c/wallhaven-tags/favorites/wallhaven-4gmloe.json"),
        STR("/home/rphii/dev/c/wallhaven-tags/favorites/wallhaven-4gpdxq.json"),
        STR("/home/rphii/dev/c/wallhaven-tags/favorites/wallhaven-4l2l1y.json"),
        STR("/home/rphii/dev/c/wallhaven-tags/favorites/wallhaven-4om9zm.json"),
        STR("/home/rphii/dev/c/wallhaven-tags/favorites/wallhaven-4v8y5l.json"),
        STR("/home/rphii/dev/c/wallhaven-tags/favorites/wallhaven-4x22kv.json"),
        STR("/home/rphii/dev/c/wallhaven-tags/favorites/wallhaven-5785x1.json"),
        STR("/home/rphii/dev/c/wallhaven-tags/favorites/wallhaven-57lxd1.json"),
        STR("/home/rphii/dev/c/wallhaven-tags/favorites/wallhaven-57y7j3.json"),
        STR("/home/rphii/dev/c/wallhaven-tags/favorites/wallhaven-5g3p18.json"),
        STR("/home/rphii/dev/c/wallhaven-tags/favorites/wallhaven-5g5oq1.json"),
        STR("/home/rphii/dev/c/wallhaven-tags/favorites/wallhaven-5g6ez8.json"),
        STR("/home/rphii/dev/c/wallhaven-tags/favorites/wallhaven-5gdx99.json"),
        STR("/home/rphii/dev/c/wallhaven-tags/favorites/wallhaven-5gm9j1.json"),
        STR("/home/rphii/dev/c/wallhaven-tags/favorites/wallhaven-5go671.json"),
        STR("/home/rphii/dev/c/wallhaven-tags/favorites/wallhaven-5gorx9.json"),
        STR("/home/rphii/dev/c/wallhaven-tags/favorites/wallhaven-5goxo5.json"),
        STR("/home/rphii/dev/c/wallhaven-tags/favorites/wallhaven-5gozk3.json"),
        STR("/home/rphii/dev/c/wallhaven-tags/favorites/wallhaven-5gp1w5.json"),
        STR("/home/rphii/dev/c/wallhaven-tags/favorites/wallhaven-5gpw68.json"),
        STR("/home/rphii/dev/c/wallhaven-tags/favorites/wallhaven-5gqwd5.json"),
        STR("/home/rphii/dev/c/wallhaven-tags/favorites/wallhaven-5gvpg7.json"),
        STR("/home/rphii/dev/c/wallhaven-tags/favorites/wallhaven-5gvzp8.json"),
        STR("/home/rphii/dev/c/wallhaven-tags/favorites/wallhaven-5w13p9.json"),
        STR("/home/rphii/dev/c/wallhaven-tags/favorites/wallhaven-5wrmq3.json"),
        STR("/home/rphii/dev/c/wallhaven-tags/favorites/wallhaven-5wwgv3.json"),
        STR("/home/rphii/dev/c/wallhaven-tags/favorites/wallhaven-6d13oq.json"),
        STR("/home/rphii/dev/c/wallhaven-tags/favorites/wallhaven-6d315w.json"),
        STR("/home/rphii/dev/c/wallhaven-tags/favorites/wallhaven-6d35jw.json"),
        STR("/home/rphii/dev/c/wallhaven-tags/favorites/wallhaven-6d3ww6.json"),
        STR("/home/rphii/dev/c/wallhaven-tags/favorites/wallhaven-6d69kx.json"),
        STR("/home/rphii/dev/c/wallhaven-tags/favorites/wallhaven-6d6xr7.json"),
        STR("/home/rphii/dev/c/wallhaven-tags/favorites/wallhaven-6dek3q.json"),
        STR("/home/rphii/dev/c/wallhaven-tags/favorites/wallhaven-6dgmvl.json"),
        STR("/home/rphii/dev/c/wallhaven-tags/favorites/wallhaven-6dj6vl.json"),
        STR("/home/rphii/dev/c/wallhaven-tags/favorites/wallhaven-6dlwzw.json"),
        STR("/home/rphii/dev/c/wallhaven-tags/favorites/wallhaven-6dp98l.json"),
        STR("/home/rphii/dev/c/wallhaven-tags/favorites/wallhaven-6dpo57.json"),
        STR("/home/rphii/dev/c/wallhaven-tags/favorites/wallhaven-6dwp2l.json"),
        STR("/home/rphii/dev/c/wallhaven-tags/favorites/wallhaven-6dyd57.json"),
        STR("/home/rphii/dev/c/wallhaven-tags/favorites/wallhaven-6dyez7.json"),
        STR("/home/rphii/dev/c/wallhaven-tags/favorites/wallhaven-6k2xk6.json"),
        STR("/home/rphii/dev/c/wallhaven-tags/favorites/wallhaven-6k2z96.json"),
        STR("/home/rphii/dev/c/wallhaven-tags/favorites/wallhaven-6kv7zq.json"),
        STR("/home/rphii/dev/c/wallhaven-tags/favorites/wallhaven-6o2mz6.json"),
        STR("/home/rphii/dev/c/wallhaven-tags/favorites/wallhaven-6o7637.json"),
        STR("/home/rphii/dev/c/wallhaven-tags/favorites/wallhaven-6oo9jl.json"),
        STR("/home/rphii/dev/c/wallhaven-tags/favorites/wallhaven-6ovopl.json"),
        STR("/home/rphii/dev/c/wallhaven-tags/favorites/wallhaven-6owmg6.json"),
        STR("/home/rphii/dev/c/wallhaven-tags/favorites/wallhaven-6qvodq.json"),
        STR("/home/rphii/dev/c/wallhaven-tags/favorites/wallhaven-6qxx3x.json"),
        STR("/home/rphii/dev/c/wallhaven-tags/favorites/wallhaven-726je9.json"),
        STR("/home/rphii/dev/c/wallhaven-tags/favorites/wallhaven-72pwj3.json"),
        STR("/home/rphii/dev/c/wallhaven-tags/favorites/wallhaven-735gko.json"),
        STR("/home/rphii/dev/c/wallhaven-tags/favorites/wallhaven-73xzzo.json"),
        STR("/home/rphii/dev/c/wallhaven-tags/favorites/wallhaven-762jg3.json"),
        STR("/home/rphii/dev/c/wallhaven-tags/favorites/wallhaven-7p1m3y.json"),
        STR("/home/rphii/dev/c/wallhaven-tags/favorites/wallhaven-7p2r6y.json"),
        STR("/home/rphii/dev/c/wallhaven-tags/favorites/wallhaven-7p7kyy.json"),
        STR("/home/rphii/dev/c/wallhaven-tags/favorites/wallhaven-7p7wrv.json"),
        STR("/home/rphii/dev/c/wallhaven-tags/favorites/wallhaven-7pe7py.json"),
        STR("/home/rphii/dev/c/wallhaven-tags/favorites/wallhaven-7pekqe.json"),
        STR("/home/rphii/dev/c/wallhaven-tags/favorites/wallhaven-7pezo9.json"),
        STR("/home/rphii/dev/c/wallhaven-tags/favorites/wallhaven-7pkkre.json"),
        STR("/home/rphii/dev/c/wallhaven-tags/favorites/wallhaven-7pl6q3.json"),
        STR("/home/rphii/dev/c/wallhaven-tags/favorites/wallhaven-7plrd3.json"),
        STR("/home/rphii/dev/c/wallhaven-tags/favorites/wallhaven-7pm1qy.json"),
        STR("/home/rphii/dev/c/wallhaven-tags/favorites/wallhaven-7po33o.json"),
        STR("/home/rphii/dev/c/wallhaven-tags/favorites/wallhaven-7pqgyo.json"),
        STR("/home/rphii/dev/c/wallhaven-tags/favorites/wallhaven-7pr129.json"),
        STR("/home/rphii/dev/c/wallhaven-tags/favorites/wallhaven-7pv159.json"),
        STR("/home/rphii/dev/c/wallhaven-tags/favorites/wallhaven-7pv31y.json"),
        STR("/home/rphii/dev/c/wallhaven-tags/favorites/wallhaven-7pvk1e.json"),
        STR("/home/rphii/dev/c/wallhaven-tags/favorites/wallhaven-7pvwmy.json"),
        STR("/home/rphii/dev/c/wallhaven-tags/favorites/wallhaven-7pxd3v.json"),
        STR("/home/rphii/dev/c/wallhaven-tags/favorites/wallhaven-8312m1.json"),
        STR("/home/rphii/dev/c/wallhaven-tags/favorites/wallhaven-83vk21.json"),
        STR("/home/rphii/dev/c/wallhaven-tags/favorites/wallhaven-83w56k.json"),
        STR("/home/rphii/dev/c/wallhaven-tags/favorites/wallhaven-858x21.json"),
        STR("/home/rphii/dev/c/wallhaven-tags/favorites/wallhaven-858z32.json"),
        STR("/home/rphii/dev/c/wallhaven-tags/favorites/wallhaven-85dwdk.json"),
        STR("/home/rphii/dev/c/wallhaven-tags/favorites/wallhaven-85dzyj.json"),
        STR("/home/rphii/dev/c/wallhaven-tags/favorites/wallhaven-85j9v2.json"),
        STR("/home/rphii/dev/c/wallhaven-tags/favorites/wallhaven-85kglo.json"),
        STR("/home/rphii/dev/c/wallhaven-tags/favorites/wallhaven-85meoy.json"),
        STR("/home/rphii/dev/c/wallhaven-tags/favorites/wallhaven-85mge2.json"),
        STR("/home/rphii/dev/c/wallhaven-tags/favorites/wallhaven-85mqmy.json"),
        STR("/home/rphii/dev/c/wallhaven-tags/favorites/wallhaven-85my81.json"),
        STR("/home/rphii/dev/c/wallhaven-tags/favorites/wallhaven-85p87k.json"),
        STR("/home/rphii/dev/c/wallhaven-tags/favorites/wallhaven-85rmm1.json"),
        STR("/home/rphii/dev/c/wallhaven-tags/favorites/wallhaven-85v7w1.json"),
        STR("/home/rphii/dev/c/wallhaven-tags/favorites/wallhaven-85v9j2.json"),
        STR("/home/rphii/dev/c/wallhaven-tags/favorites/wallhaven-85w2go.json"),
        STR("/home/rphii/dev/c/wallhaven-tags/favorites/wallhaven-8o1kqj.json"),
        STR("/home/rphii/dev/c/wallhaven-tags/favorites/wallhaven-8o5o7k.json"),
        STR("/home/rphii/dev/c/wallhaven-tags/favorites/wallhaven-8o8qx2.json"),
        STR("/home/rphii/dev/c/wallhaven-tags/favorites/wallhaven-8o975y.json"),
        STR("/home/rphii/dev/c/wallhaven-tags/favorites/wallhaven-8oelmk.json"),
        STR("/home/rphii/dev/c/wallhaven-tags/favorites/wallhaven-8ojwxo.json"),
        STR("/home/rphii/dev/c/wallhaven-tags/favorites/wallhaven-8oox62.json"),
        STR("/home/rphii/dev/c/wallhaven-tags/favorites/wallhaven-8ozzxk.json"),
        STR("/home/rphii/dev/c/wallhaven-tags/favorites/wallhaven-8x5r7o.json"),
        STR("/home/rphii/dev/c/wallhaven-tags/favorites/wallhaven-8xoq71.json"),
        STR("/home/rphii/dev/c/wallhaven-tags/favorites/wallhaven-9572pk.json"),
        STR("/home/rphii/dev/c/wallhaven-tags/favorites/wallhaven-95jrz8.json"),
        STR("/home/rphii/dev/c/wallhaven-tags/favorites/wallhaven-95kjxd.json"),
        STR("/home/rphii/dev/c/wallhaven-tags/favorites/wallhaven-95ql7d.json"),
        STR("/home/rphii/dev/c/wallhaven-tags/favorites/wallhaven-96o86k.json"),
        STR("/home/rphii/dev/c/wallhaven-tags/favorites/wallhaven-96rwd8.json"),
        STR("/home/rphii/dev/c/wallhaven-tags/favorites/wallhaven-96v11d.json"),
        STR("/home/rphii/dev/c/wallhaven-tags/favorites/wallhaven-9d2pww.json"),
        STR("/home/rphii/dev/c/wallhaven-tags/favorites/wallhaven-9d8drx.json"),
        STR("/home/rphii/dev/c/wallhaven-tags/favorites/wallhaven-9d8g9w.json"),
        STR("/home/rphii/dev/c/wallhaven-tags/favorites/wallhaven-9d8ryx.json"),
        STR("/home/rphii/dev/c/wallhaven-tags/favorites/wallhaven-9dellx.json"),
        STR("/home/rphii/dev/c/wallhaven-tags/favorites/wallhaven-9dkdrd.json"),
        STR("/home/rphii/dev/c/wallhaven-tags/favorites/wallhaven-9doozw.json"),
        STR("/home/rphii/dev/c/wallhaven-tags/favorites/wallhaven-9dplok.json"),
        STR("/home/rphii/dev/c/wallhaven-tags/favorites/wallhaven-9dplvx.json"),
        STR("/home/rphii/dev/c/wallhaven-tags/favorites/wallhaven-9dppex.json"),
        STR("/home/rphii/dev/c/wallhaven-tags/favorites/wallhaven-9dprm1.json"),
        STR("/home/rphii/dev/c/wallhaven-tags/favorites/wallhaven-9dpyz1.json"),
        STR("/home/rphii/dev/c/wallhaven-tags/favorites/wallhaven-9drdkd.json"),
        STR("/home/rphii/dev/c/wallhaven-tags/favorites/wallhaven-9drvp1.json"),
        STR("/home/rphii/dev/c/wallhaven-tags/favorites/wallhaven-9mq26d.json"),
        STR("/home/rphii/dev/c/wallhaven-tags/favorites/wallhaven-9mv7jk.json"),
        STR("/home/rphii/dev/c/wallhaven-tags/favorites/wallhaven-9my21w.json"),
        STR("/home/rphii/dev/c/wallhaven-tags/favorites/wallhaven-d6dyx3.json"),
        STR("/home/rphii/dev/c/wallhaven-tags/favorites/wallhaven-d6je5l.json"),
        STR("/home/rphii/dev/c/wallhaven-tags/favorites/wallhaven-d6jedo.json"),
        STR("/home/rphii/dev/c/wallhaven-tags/favorites/wallhaven-d6op7m.json"),
        STR("/home/rphii/dev/c/wallhaven-tags/favorites/wallhaven-d6oy6o.json"),
        STR("/home/rphii/dev/c/wallhaven-tags/favorites/wallhaven-d6v5pg.json"),
        STR("/home/rphii/dev/c/wallhaven-tags/favorites/wallhaven-d6vp7m.json"),
        STR("/home/rphii/dev/c/wallhaven-tags/favorites/wallhaven-d6y12l.json"),
        STR("/home/rphii/dev/c/wallhaven-tags/favorites/wallhaven-d6y7vl.json"),
        STR("/home/rphii/dev/c/wallhaven-tags/favorites/wallhaven-d6yjpm.json"),
        STR("/home/rphii/dev/c/wallhaven-tags/favorites/wallhaven-d6zjpm.json"),
        STR("/home/rphii/dev/c/wallhaven-tags/favorites/wallhaven-dgeexm.json"),
        STR("/home/rphii/dev/c/wallhaven-tags/favorites/wallhaven-dp5k5o.json"),
        STR("/home/rphii/dev/c/wallhaven-tags/favorites/wallhaven-dp6epm.json"),
        STR("/home/rphii/dev/c/wallhaven-tags/favorites/wallhaven-dp7gkm.json"),
        STR("/home/rphii/dev/c/wallhaven-tags/favorites/wallhaven-dp9p93.json"),
        STR("/home/rphii/dev/c/wallhaven-tags/favorites/wallhaven-dpze2o.json"),
        STR("/home/rphii/dev/c/wallhaven-tags/favorites/wallhaven-eol79o.json"),
        STR("/home/rphii/dev/c/wallhaven-tags/favorites/wallhaven-ex3z9l.json"),
        STR("/home/rphii/dev/c/wallhaven-tags/favorites/wallhaven-ex6yl8.json"),
        STR("/home/rphii/dev/c/wallhaven-tags/favorites/wallhaven-exejgw.json"),
        STR("/home/rphii/dev/c/wallhaven-tags/favorites/wallhaven-exg33k.json"),
        STR("/home/rphii/dev/c/wallhaven-tags/favorites/wallhaven-exgyok.json"),
        STR("/home/rphii/dev/c/wallhaven-tags/favorites/wallhaven-exl7eo.json"),
        STR("/home/rphii/dev/c/wallhaven-tags/favorites/wallhaven-exlmdo.json"),
        STR("/home/rphii/dev/c/wallhaven-tags/favorites/wallhaven-exrkqr.json"),
        STR("/home/rphii/dev/c/wallhaven-tags/favorites/wallhaven-exvkkl.json"),
        STR("/home/rphii/dev/c/wallhaven-tags/favorites/wallhaven-exwzvo.json"),
        STR("/home/rphii/dev/c/wallhaven-tags/favorites/wallhaven-ey7owl.json"),
        STR("/home/rphii/dev/c/wallhaven-tags/favorites/wallhaven-eyy16o.json"),
        STR("/home/rphii/dev/c/wallhaven-tags/favorites/wallhaven-g733oe.json"),
        STR("/home/rphii/dev/c/wallhaven-tags/favorites/wallhaven-g7m8zq.json"),
        STR("/home/rphii/dev/c/wallhaven-tags/favorites/wallhaven-g8le27.json"),
        STR("/home/rphii/dev/c/wallhaven-tags/favorites/wallhaven-gj7kjd.json"),
        STR("/home/rphii/dev/c/wallhaven-tags/favorites/wallhaven-gjg75e.json"),
        STR("/home/rphii/dev/c/wallhaven-tags/favorites/wallhaven-gp18g3.json"),
        STR("/home/rphii/dev/c/wallhaven-tags/favorites/wallhaven-gp2z1e.json"),
        STR("/home/rphii/dev/c/wallhaven-tags/favorites/wallhaven-gp692l.json"),
        STR("/home/rphii/dev/c/wallhaven-tags/favorites/wallhaven-gp6gp7.json"),
        STR("/home/rphii/dev/c/wallhaven-tags/favorites/wallhaven-gp6zyd.json"),
        STR("/home/rphii/dev/c/wallhaven-tags/favorites/wallhaven-gp896d.json"),
        STR("/home/rphii/dev/c/wallhaven-tags/favorites/wallhaven-gpdj37.json"),
        STR("/home/rphii/dev/c/wallhaven-tags/favorites/wallhaven-gpg12d.json"),
        STR("/home/rphii/dev/c/wallhaven-tags/favorites/wallhaven-gpg2z7.json"),
        STR("/home/rphii/dev/c/wallhaven-tags/favorites/wallhaven-gpg8x7.json"),
        STR("/home/rphii/dev/c/wallhaven-tags/favorites/wallhaven-gpgkkl.json"),
        STR("/home/rphii/dev/c/wallhaven-tags/favorites/wallhaven-gpjm3d.json"),
        STR("/home/rphii/dev/c/wallhaven-tags/favorites/wallhaven-gpkrd7.json"),
        STR("/home/rphii/dev/c/wallhaven-tags/favorites/wallhaven-gpl8d3.json"),
        STR("/home/rphii/dev/c/wallhaven-tags/favorites/wallhaven-gpm2j3.json"),
        STR("/home/rphii/dev/c/wallhaven-tags/favorites/wallhaven-gpm6eq.json"),
        STR("/home/rphii/dev/c/wallhaven-tags/favorites/wallhaven-gpmjxe.json"),
        STR("/home/rphii/dev/c/wallhaven-tags/favorites/wallhaven-gpml27.json"),
        STR("/home/rphii/dev/c/wallhaven-tags/favorites/wallhaven-gpv77d.json"),
        STR("/home/rphii/dev/c/wallhaven-tags/favorites/wallhaven-gpv9y3.json"),
        STR("/home/rphii/dev/c/wallhaven-tags/favorites/wallhaven-gpwxrq.json"),
        STR("/home/rphii/dev/c/wallhaven-tags/favorites/wallhaven-gpx87d.json"),
        STR("/home/rphii/dev/c/wallhaven-tags/favorites/wallhaven-gpxxq7.json"),
        STR("/home/rphii/dev/c/wallhaven-tags/favorites/wallhaven-j3o1dp.json"),
        STR("/home/rphii/dev/c/wallhaven-tags/favorites/wallhaven-j3wq5p.json"),
        STR("/home/rphii/dev/c/wallhaven-tags/favorites/wallhaven-j3xkxm.json"),
        STR("/home/rphii/dev/c/wallhaven-tags/favorites/wallhaven-j5g2ly.json"),
        STR("/home/rphii/dev/c/wallhaven-tags/favorites/wallhaven-j5wglm.json"),
        STR("/home/rphii/dev/c/wallhaven-tags/favorites/wallhaven-j5yg5q.json"),
        STR("/home/rphii/dev/c/wallhaven-tags/favorites/wallhaven-j85zom.json"),
        STR("/home/rphii/dev/c/wallhaven-tags/favorites/wallhaven-j861vm.json"),
        STR("/home/rphii/dev/c/wallhaven-tags/favorites/wallhaven-j87p7q.json"),
        STR("/home/rphii/dev/c/wallhaven-tags/favorites/wallhaven-jx12vq.json"),
        STR("/home/rphii/dev/c/wallhaven-tags/favorites/wallhaven-jx1wry.json"),
        STR("/home/rphii/dev/c/wallhaven-tags/favorites/wallhaven-jx1xjq.json"),
        STR("/home/rphii/dev/c/wallhaven-tags/favorites/wallhaven-jx7ddq.json"),
        STR("/home/rphii/dev/c/wallhaven-tags/favorites/wallhaven-jx7evp.json"),
        STR("/home/rphii/dev/c/wallhaven-tags/favorites/wallhaven-jx7j2p.json"),
        STR("/home/rphii/dev/c/wallhaven-tags/favorites/wallhaven-jx7rx5.json"),
        STR("/home/rphii/dev/c/wallhaven-tags/favorites/wallhaven-jxd9eq.json"),
        STR("/home/rphii/dev/c/wallhaven-tags/favorites/wallhaven-jxd9yp.json"),
        STR("/home/rphii/dev/c/wallhaven-tags/favorites/wallhaven-jxdlvw.json"),
        STR("/home/rphii/dev/c/wallhaven-tags/favorites/wallhaven-jxe5dy.json"),
        STR("/home/rphii/dev/c/wallhaven-tags/favorites/wallhaven-jxelq5.json"),
        STR("/home/rphii/dev/c/wallhaven-tags/favorites/wallhaven-jxjkgm.json"),
        STR("/home/rphii/dev/c/wallhaven-tags/favorites/wallhaven-jxjr85.json"),
        STR("/home/rphii/dev/c/wallhaven-tags/favorites/wallhaven-jxjrkw.json"),
        STR("/home/rphii/dev/c/wallhaven-tags/favorites/wallhaven-jxjv7p.json"),
        STR("/home/rphii/dev/c/wallhaven-tags/favorites/wallhaven-jxjywy.json"),
        STR("/home/rphii/dev/c/wallhaven-tags/favorites/wallhaven-jxk1kw.json"),
        STR("/home/rphii/dev/c/wallhaven-tags/favorites/wallhaven-jxl3qp.json"),
        STR("/home/rphii/dev/c/wallhaven-tags/favorites/wallhaven-jxl56w.json"),
        STR("/home/rphii/dev/c/wallhaven-tags/favorites/wallhaven-jxl6dm.json"),
        STR("/home/rphii/dev/c/wallhaven-tags/favorites/wallhaven-jxle5w.json"),
        STR("/home/rphii/dev/c/wallhaven-tags/favorites/wallhaven-jxowop.json"),
        STR("/home/rphii/dev/c/wallhaven-tags/favorites/wallhaven-jxp1wq.json"),
        STR("/home/rphii/dev/c/wallhaven-tags/favorites/wallhaven-jxqrx5.json"),
        STR("/home/rphii/dev/c/wallhaven-tags/favorites/wallhaven-jxr8wq.json"),
        STR("/home/rphii/dev/c/wallhaven-tags/favorites/wallhaven-jxrz3p.json"),
        STR("/home/rphii/dev/c/wallhaven-tags/favorites/wallhaven-jxvwvy.json"),
        STR("/home/rphii/dev/c/wallhaven-tags/favorites/wallhaven-jxyo6q.json"),
        STR("/home/rphii/dev/c/wallhaven-tags/favorites/wallhaven-k7x951.json"),
        STR("/home/rphii/dev/c/wallhaven-tags/favorites/wallhaven-k7zp5d.json"),
        STR("/home/rphii/dev/c/wallhaven-tags/favorites/wallhaven-k92167.json"),
        STR("/home/rphii/dev/c/wallhaven-tags/favorites/wallhaven-kwdoj6.json"),
        STR("/home/rphii/dev/c/wallhaven-tags/favorites/wallhaven-kwp6r1.json"),
        STR("/home/rphii/dev/c/wallhaven-tags/favorites/wallhaven-kx1q21.json"),
        STR("/home/rphii/dev/c/wallhaven-tags/favorites/wallhaven-kx3r9m.json"),
        STR("/home/rphii/dev/c/wallhaven-tags/favorites/wallhaven-kx516d.json"),
        STR("/home/rphii/dev/c/wallhaven-tags/favorites/wallhaven-kx5qvm.json"),
        STR("/home/rphii/dev/c/wallhaven-tags/favorites/wallhaven-kx65o7.json"),
        STR("/home/rphii/dev/c/wallhaven-tags/favorites/wallhaven-kx6kkm.json"),
        STR("/home/rphii/dev/c/wallhaven-tags/favorites/wallhaven-kxedr7.json"),
        STR("/home/rphii/dev/c/wallhaven-tags/favorites/wallhaven-kxjmoq.json"),
        STR("/home/rphii/dev/c/wallhaven-tags/favorites/wallhaven-kxjrz6.json"),
        STR("/home/rphii/dev/c/wallhaven-tags/favorites/wallhaven-kxlzw6.json"),
        STR("/home/rphii/dev/c/wallhaven-tags/favorites/wallhaven-kxrv16.json"),
        STR("/home/rphii/dev/c/wallhaven-tags/favorites/wallhaven-kxrwj1.json"),
        STR("/home/rphii/dev/c/wallhaven-tags/favorites/wallhaven-l3rqzr.json"),
        STR("/home/rphii/dev/c/wallhaven-tags/favorites/wallhaven-l829gp.json"),
        STR("/home/rphii/dev/c/wallhaven-tags/favorites/wallhaven-l82w3q.json"),
        STR("/home/rphii/dev/c/wallhaven-tags/favorites/wallhaven-l87d7r.json"),
        STR("/home/rphii/dev/c/wallhaven-tags/favorites/wallhaven-l87vr2.json"),
        STR("/home/rphii/dev/c/wallhaven-tags/favorites/wallhaven-l899ql.json"),
        STR("/home/rphii/dev/c/wallhaven-tags/favorites/wallhaven-l8dj1p.json"),
        STR("/home/rphii/dev/c/wallhaven-tags/favorites/wallhaven-l8dj1r.json"),
        STR("/home/rphii/dev/c/wallhaven-tags/favorites/wallhaven-l8g9kp.json"),
        STR("/home/rphii/dev/c/wallhaven-tags/favorites/wallhaven-l8gegl.json"),
        STR("/home/rphii/dev/c/wallhaven-tags/favorites/wallhaven-l8gp8l.json"),
        STR("/home/rphii/dev/c/wallhaven-tags/favorites/wallhaven-l8j2ql.json"),
        STR("/home/rphii/dev/c/wallhaven-tags/favorites/wallhaven-l8jrzl.json"),
        STR("/home/rphii/dev/c/wallhaven-tags/favorites/wallhaven-l8jw2y.json"),
        STR("/home/rphii/dev/c/wallhaven-tags/favorites/wallhaven-l8o5jq.json"),
        STR("/home/rphii/dev/c/wallhaven-tags/favorites/wallhaven-l8oxzq.json"),
        STR("/home/rphii/dev/c/wallhaven-tags/favorites/wallhaven-l8pjpp.json"),
        STR("/home/rphii/dev/c/wallhaven-tags/favorites/wallhaven-l8pr8q.json"),
        STR("/home/rphii/dev/c/wallhaven-tags/favorites/wallhaven-l8pweq.json"),
        STR("/home/rphii/dev/c/wallhaven-tags/favorites/wallhaven-l8xxep.json"),
        STR("/home/rphii/dev/c/wallhaven-tags/favorites/wallhaven-lq3x3r.json"),
        STR("/home/rphii/dev/c/wallhaven-tags/favorites/wallhaven-lqgqpq.json"),
        STR("/home/rphii/dev/c/wallhaven-tags/favorites/wallhaven-lqj6dq.json"),
        STR("/home/rphii/dev/c/wallhaven-tags/favorites/wallhaven-m31rd8.json"),
        STR("/home/rphii/dev/c/wallhaven-tags/favorites/wallhaven-m35678.json"),
        STR("/home/rphii/dev/c/wallhaven-tags/favorites/wallhaven-m35e5k.json"),
        STR("/home/rphii/dev/c/wallhaven-tags/favorites/wallhaven-m35pxm.json"),
        STR("/home/rphii/dev/c/wallhaven-tags/favorites/wallhaven-m35q7m.json"),
        STR("/home/rphii/dev/c/wallhaven-tags/favorites/wallhaven-m367vy.json"),
        STR("/home/rphii/dev/c/wallhaven-tags/favorites/wallhaven-m3e3xy.json"),
        STR("/home/rphii/dev/c/wallhaven-tags/favorites/wallhaven-m3ejkm.json"),
        STR("/home/rphii/dev/c/wallhaven-tags/favorites/wallhaven-m3evd1.json"),
        STR("/home/rphii/dev/c/wallhaven-tags/favorites/wallhaven-m3gy99.json"),
        STR("/home/rphii/dev/c/wallhaven-tags/favorites/wallhaven-m3gzvk.json"),
        STR("/home/rphii/dev/c/wallhaven-tags/favorites/wallhaven-m3mr58.json"),
        STR("/home/rphii/dev/c/wallhaven-tags/favorites/wallhaven-m3p878.json"),
        STR("/home/rphii/dev/c/wallhaven-tags/favorites/wallhaven-m3q7d1.json"),
        STR("/home/rphii/dev/c/wallhaven-tags/favorites/wallhaven-m3qw38.json"),
        STR("/home/rphii/dev/c/wallhaven-tags/favorites/wallhaven-m3r1m8.json"),
        STR("/home/rphii/dev/c/wallhaven-tags/favorites/wallhaven-m3rj98.json"),
        STR("/home/rphii/dev/c/wallhaven-tags/favorites/wallhaven-m3vd3k.json"),
        STR("/home/rphii/dev/c/wallhaven-tags/favorites/wallhaven-m3vzlm.json"),
        STR("/home/rphii/dev/c/wallhaven-tags/favorites/wallhaven-m3w288.json"),
        STR("/home/rphii/dev/c/wallhaven-tags/favorites/wallhaven-m3y67m.json"),
        STR("/home/rphii/dev/c/wallhaven-tags/favorites/wallhaven-m93z2y.json"),
        STR("/home/rphii/dev/c/wallhaven-tags/favorites/wallhaven-m97qp8.json"),
        STR("/home/rphii/dev/c/wallhaven-tags/favorites/wallhaven-mddm28.json"),
        STR("/home/rphii/dev/c/wallhaven-tags/favorites/wallhaven-mdp1p8.json"),
        STR("/home/rphii/dev/c/wallhaven-tags/favorites/wallhaven-mdx6gk.json"),
        STR("/home/rphii/dev/c/wallhaven-tags/favorites/wallhaven-mdxd7k.json"),
        STR("/home/rphii/dev/c/wallhaven-tags/favorites/wallhaven-n6xm7n.json"),
        STR("/home/rphii/dev/c/wallhaven-tags/favorites/wallhaven-nex8qk.json"),
        STR("/home/rphii/dev/c/wallhaven-tags/favorites/wallhaven-nkm9lq.json"),
        STR("/home/rphii/dev/c/wallhaven-tags/favorites/wallhaven-nm587k.json"),
        STR("/home/rphii/dev/c/wallhaven-tags/favorites/wallhaven-nmdx3m.json"),
        STR("/home/rphii/dev/c/wallhaven-tags/favorites/wallhaven-nrkp51.json"),
        STR("/home/rphii/dev/c/wallhaven-tags/favorites/wallhaven-nz2d3y.json"),
        STR("/home/rphii/dev/c/wallhaven-tags/favorites/wallhaven-nzwjvg.json"),
        STR("/home/rphii/dev/c/wallhaven-tags/favorites/wallhaven-o335ll.json"),
        STR("/home/rphii/dev/c/wallhaven-tags/favorites/wallhaven-o526l5.json"),
        STR("/home/rphii/dev/c/wallhaven-tags/favorites/wallhaven-o587o9.json"),
        STR("/home/rphii/dev/c/wallhaven-tags/favorites/wallhaven-o58mop.json"),
        STR("/home/rphii/dev/c/wallhaven-tags/favorites/wallhaven-o58or7.json"),
        STR("/home/rphii/dev/c/wallhaven-tags/favorites/wallhaven-o59l5p.json"),
        STR("/home/rphii/dev/c/wallhaven-tags/favorites/wallhaven-o5qkml.json"),
        STR("/home/rphii/dev/c/wallhaven-tags/favorites/wallhaven-o5qyr5.json"),
        STR("/home/rphii/dev/c/wallhaven-tags/favorites/wallhaven-o5v1wp.json"),
        STR("/home/rphii/dev/c/wallhaven-tags/favorites/wallhaven-od3v75.json"),
        STR("/home/rphii/dev/c/wallhaven-tags/favorites/wallhaven-odqp77.json"),
        STR("/home/rphii/dev/c/wallhaven-tags/favorites/wallhaven-odzzrp.json"),
        STR("/home/rphii/dev/c/wallhaven-tags/favorites/wallhaven-oxdoy7.json"),
        STR("/home/rphii/dev/c/wallhaven-tags/favorites/wallhaven-oxgjvl.json"),
        STR("/home/rphii/dev/c/wallhaven-tags/favorites/wallhaven-oxr9l5.json"),
        STR("/home/rphii/dev/c/wallhaven-tags/favorites/wallhaven-oxwe99.json"),
        STR("/home/rphii/dev/c/wallhaven-tags/favorites/wallhaven-oxxw29.json"),
        STR("/home/rphii/dev/c/wallhaven-tags/favorites/wallhaven-p8go3m.json"),
        STR("/home/rphii/dev/c/wallhaven-tags/favorites/wallhaven-p8k76e.json"),
        STR("/home/rphii/dev/c/wallhaven-tags/favorites/wallhaven-p8x6mm.json"),
        STR("/home/rphii/dev/c/wallhaven-tags/favorites/wallhaven-p913ep.json"),
        STR("/home/rphii/dev/c/wallhaven-tags/favorites/wallhaven-p9177e.json"),
        STR("/home/rphii/dev/c/wallhaven-tags/favorites/wallhaven-p91wdj.json"),
        STR("/home/rphii/dev/c/wallhaven-tags/favorites/wallhaven-p91z89.json"),
        STR("/home/rphii/dev/c/wallhaven-tags/favorites/wallhaven-p935oe.json"),
        STR("/home/rphii/dev/c/wallhaven-tags/favorites/wallhaven-p93y6p.json"),
        STR("/home/rphii/dev/c/wallhaven-tags/favorites/wallhaven-p955gm.json"),
        STR("/home/rphii/dev/c/wallhaven-tags/favorites/wallhaven-p95693.json"),
        STR("/home/rphii/dev/c/wallhaven-tags/favorites/wallhaven-p95wd9.json"),
        STR("/home/rphii/dev/c/wallhaven-tags/favorites/wallhaven-p97l5e.json"),
        STR("/home/rphii/dev/c/wallhaven-tags/favorites/wallhaven-p9g68e.json"),
        STR("/home/rphii/dev/c/wallhaven-tags/favorites/wallhaven-p9l8lm.json"),
        STR("/home/rphii/dev/c/wallhaven-tags/favorites/wallhaven-p9mq6m.json"),
        STR("/home/rphii/dev/c/wallhaven-tags/favorites/wallhaven-p9ppxp.json"),
        STR("/home/rphii/dev/c/wallhaven-tags/favorites/wallhaven-p9v89e.json"),
        STR("/home/rphii/dev/c/wallhaven-tags/favorites/wallhaven-p9x1ee.json"),
        STR("/home/rphii/dev/c/wallhaven-tags/favorites/wallhaven-p9y8jp.json"),
        STR("/home/rphii/dev/c/wallhaven-tags/favorites/wallhaven-p9z38e.json"),
        STR("/home/rphii/dev/c/wallhaven-tags/favorites/wallhaven-p9z9qm.json"),
        STR("/home/rphii/dev/c/wallhaven-tags/favorites/wallhaven-p9zjrj.json"),
        STR("/home/rphii/dev/c/wallhaven-tags/favorites/wallhaven-p9zmd3.json"),
        STR("/home/rphii/dev/c/wallhaven-tags/favorites/wallhaven-pkjlxe.json"),
        STR("/home/rphii/dev/c/wallhaven-tags/favorites/wallhaven-pklqz9.json"),
        STR("/home/rphii/dev/c/wallhaven-tags/favorites/wallhaven-pkv5q3.json"),
        STR("/home/rphii/dev/c/wallhaven-tags/favorites/wallhaven-pkvyrj.json"),
        STR("/home/rphii/dev/c/wallhaven-tags/favorites/wallhaven-q21g3r.json"),
        STR("/home/rphii/dev/c/wallhaven-tags/favorites/wallhaven-q22kwq.json"),
        STR("/home/rphii/dev/c/wallhaven-tags/favorites/wallhaven-q2oxdd.json"),
        STR("/home/rphii/dev/c/wallhaven-tags/favorites/wallhaven-q2xrxq.json"),
        STR("/home/rphii/dev/c/wallhaven-tags/favorites/wallhaven-q61wlq.json"),
        STR("/home/rphii/dev/c/wallhaven-tags/favorites/wallhaven-q62eol.json"),
        STR("/home/rphii/dev/c/wallhaven-tags/favorites/wallhaven-q68wqq.json"),
        STR("/home/rphii/dev/c/wallhaven-tags/favorites/wallhaven-q6omvd.json"),
        STR("/home/rphii/dev/c/wallhaven-tags/favorites/wallhaven-qz37ql.json"),
        STR("/home/rphii/dev/c/wallhaven-tags/favorites/wallhaven-qz5g95.json"),
        STR("/home/rphii/dev/c/wallhaven-tags/favorites/wallhaven-qzkkwr.json"),
        STR("/home/rphii/dev/c/wallhaven-tags/favorites/wallhaven-qzleeq.json"),
        STR("/home/rphii/dev/c/wallhaven-tags/favorites/wallhaven-qzll9l.json"),
        STR("/home/rphii/dev/c/wallhaven-tags/favorites/wallhaven-qzo2od.json"),
        STR("/home/rphii/dev/c/wallhaven-tags/favorites/wallhaven-qzp6kl.json"),
        STR("/home/rphii/dev/c/wallhaven-tags/favorites/wallhaven-qzp8dr.json"),
        STR("/home/rphii/dev/c/wallhaven-tags/favorites/wallhaven-qzqgw5.json"),
        STR("/home/rphii/dev/c/wallhaven-tags/favorites/wallhaven-qzqxdr.json"),
        STR("/home/rphii/dev/c/wallhaven-tags/favorites/wallhaven-qzr6gq.json"),
        STR("/home/rphii/dev/c/wallhaven-tags/favorites/wallhaven-qzv6rd.json"),
        STR("/home/rphii/dev/c/wallhaven-tags/favorites/wallhaven-r25gzw.json"),
        STR("/home/rphii/dev/c/wallhaven-tags/favorites/wallhaven-r2j7pq.json"),
        STR("/home/rphii/dev/c/wallhaven-tags/favorites/wallhaven-r2ygp7.json"),
        STR("/home/rphii/dev/c/wallhaven-tags/favorites/wallhaven-rd7j9m.json"),
        STR("/home/rphii/dev/c/wallhaven-tags/favorites/wallhaven-rdmdrj.json"),
        STR("/home/rphii/dev/c/wallhaven-tags/favorites/wallhaven-rdr2vw.json"),
        STR("/home/rphii/dev/c/wallhaven-tags/favorites/wallhaven-rdxr6j.json"),
        STR("/home/rphii/dev/c/wallhaven-tags/favorites/wallhaven-rr1yoq.json"),
        STR("/home/rphii/dev/c/wallhaven-tags/favorites/wallhaven-rr316q.json"),
        STR("/home/rphii/dev/c/wallhaven-tags/favorites/wallhaven-rr5y5j.json"),
        STR("/home/rphii/dev/c/wallhaven-tags/favorites/wallhaven-rr6v5w.json"),
        STR("/home/rphii/dev/c/wallhaven-tags/favorites/wallhaven-rr99r1.json"),
        STR("/home/rphii/dev/c/wallhaven-tags/favorites/wallhaven-rr9gqq.json"),
        STR("/home/rphii/dev/c/wallhaven-tags/favorites/wallhaven-rrj5xm.json"),
        STR("/home/rphii/dev/c/wallhaven-tags/favorites/wallhaven-rrjmx1.json"),
        STR("/home/rphii/dev/c/wallhaven-tags/favorites/wallhaven-rroml1.json"),
        STR("/home/rphii/dev/c/wallhaven-tags/favorites/wallhaven-rrwqq7.json"),
        STR("/home/rphii/dev/c/wallhaven-tags/favorites/wallhaven-rrx65q.json"),
        STR("/home/rphii/dev/c/wallhaven-tags/favorites/wallhaven-rrywdj.json"),
        STR("/home/rphii/dev/c/wallhaven-tags/favorites/wallhaven-v98dj8.json"),
        STR("/home/rphii/dev/c/wallhaven-tags/favorites/wallhaven-v99mv5.json"),
        STR("/home/rphii/dev/c/wallhaven-tags/favorites/wallhaven-v99yy8.json"),
        STR("/home/rphii/dev/c/wallhaven-tags/favorites/wallhaven-v9eq78.json"),
        STR("/home/rphii/dev/c/wallhaven-tags/favorites/wallhaven-v9qyql.json"),
        STR("/home/rphii/dev/c/wallhaven-tags/favorites/wallhaven-vg5l6p.json"),
        STR("/home/rphii/dev/c/wallhaven-tags/favorites/wallhaven-vg6v78.json"),
        STR("/home/rphii/dev/c/wallhaven-tags/favorites/wallhaven-vml6j5.json"),
        STR("/home/rphii/dev/c/wallhaven-tags/favorites/wallhaven-vmzgv3.json"),
        STR("/home/rphii/dev/c/wallhaven-tags/favorites/wallhaven-vq27el.json"),
        STR("/home/rphii/dev/c/wallhaven-tags/favorites/wallhaven-vq27r8.json"),
        STR("/home/rphii/dev/c/wallhaven-tags/favorites/wallhaven-vq5gr3.json"),
        STR("/home/rphii/dev/c/wallhaven-tags/favorites/wallhaven-vq6xwp.json"),
        STR("/home/rphii/dev/c/wallhaven-tags/favorites/wallhaven-vqdmxm.json"),
        STR("/home/rphii/dev/c/wallhaven-tags/favorites/wallhaven-vqed65.json"),
        STR("/home/rphii/dev/c/wallhaven-tags/favorites/wallhaven-vqevd8.json"),
        STR("/home/rphii/dev/c/wallhaven-tags/favorites/wallhaven-vqlee5.json"),
        STR("/home/rphii/dev/c/wallhaven-tags/favorites/wallhaven-vqlkd8.json"),
        STR("/home/rphii/dev/c/wallhaven-tags/favorites/wallhaven-vqly23.json"),
        STR("/home/rphii/dev/c/wallhaven-tags/favorites/wallhaven-vqy2jp.json"),
        STR("/home/rphii/dev/c/wallhaven-tags/favorites/wallhaven-vqzq98.json"),
        STR("/home/rphii/dev/c/wallhaven-tags/favorites/wallhaven-w8x5eq.json"),
        STR("/home/rphii/dev/c/wallhaven-tags/favorites/wallhaven-we3317.json"),
        STR("/home/rphii/dev/c/wallhaven-tags/favorites/wallhaven-we3ep7.json"),
        STR("/home/rphii/dev/c/wallhaven-tags/favorites/wallhaven-we3pyq.json"),
        STR("/home/rphii/dev/c/wallhaven-tags/favorites/wallhaven-we8wyq.json"),
        STR("/home/rphii/dev/c/wallhaven-tags/favorites/wallhaven-we8xq6.json"),
        STR("/home/rphii/dev/c/wallhaven-tags/favorites/wallhaven-wed2qq.json"),
        STR("/home/rphii/dev/c/wallhaven-tags/favorites/wallhaven-wedp77.json"),
        STR("/home/rphii/dev/c/wallhaven-tags/favorites/wallhaven-wedyd6.json"),
        STR("/home/rphii/dev/c/wallhaven-tags/favorites/wallhaven-weggor.json"),
        STR("/home/rphii/dev/c/wallhaven-tags/favorites/wallhaven-wegjwq.json"),
        STR("/home/rphii/dev/c/wallhaven-tags/favorites/wallhaven-wej9vp.json"),
        STR("/home/rphii/dev/c/wallhaven-tags/favorites/wallhaven-wejr1p.json"),
        STR("/home/rphii/dev/c/wallhaven-tags/favorites/wallhaven-wekv3q.json"),
        STR("/home/rphii/dev/c/wallhaven-tags/favorites/wallhaven-wekvk6.json"),
        STR("/home/rphii/dev/c/wallhaven-tags/favorites/wallhaven-wekzrp.json"),
        STR("/home/rphii/dev/c/wallhaven-tags/favorites/wallhaven-welr6r.json"),
        STR("/home/rphii/dev/c/wallhaven-tags/favorites/wallhaven-wem53q.json"),
        STR("/home/rphii/dev/c/wallhaven-tags/favorites/wallhaven-wer566.json"),
        STR("/home/rphii/dev/c/wallhaven-tags/favorites/wallhaven-wev6vx.json"),
        STR("/home/rphii/dev/c/wallhaven-tags/favorites/wallhaven-wevq5q.json"),
        STR("/home/rphii/dev/c/wallhaven-tags/favorites/wallhaven-wew2lx.json"),
        STR("/home/rphii/dev/c/wallhaven-tags/favorites/wallhaven-wewrer.json"),
        STR("/home/rphii/dev/c/wallhaven-tags/favorites/wallhaven-wq2krx.json"),
        STR("/home/rphii/dev/c/wallhaven-tags/favorites/wallhaven-wq8kyq.json"),
        STR("/home/rphii/dev/c/wallhaven-tags/favorites/wallhaven-wy31gr.json"),
        STR("/home/rphii/dev/c/wallhaven-tags/favorites/wallhaven-wy79zx.json"),
        STR("/home/rphii/dev/c/wallhaven-tags/favorites/wallhaven-wy7vp7.json"),
        STR("/home/rphii/dev/c/wallhaven-tags/favorites/wallhaven-wyjzm7.json"),
        STR("/home/rphii/dev/c/wallhaven-tags/favorites/wallhaven-wyy31x.json"),
        STR("/home/rphii/dev/c/wallhaven-tags/favorites/wallhaven-x177do.json"),
        STR("/home/rphii/dev/c/wallhaven-tags/favorites/wallhaven-x1v73d.json"),
        STR("/home/rphii/dev/c/wallhaven-tags/favorites/wallhaven-x621xo.json"),
        STR("/home/rphii/dev/c/wallhaven-tags/favorites/wallhaven-x62y1z.json"),
        STR("/home/rphii/dev/c/wallhaven-tags/favorites/wallhaven-x67qod.json"),
        STR("/home/rphii/dev/c/wallhaven-tags/favorites/wallhaven-x69l7v.json"),
        STR("/home/rphii/dev/c/wallhaven-tags/favorites/wallhaven-x6gv7z.json"),
        STR("/home/rphii/dev/c/wallhaven-tags/favorites/wallhaven-x6k21l.json"),
        STR("/home/rphii/dev/c/wallhaven-tags/favorites/wallhaven-x6o67l.json"),
        STR("/home/rphii/dev/c/wallhaven-tags/favorites/wallhaven-x6od9v.json"),
        STR("/home/rphii/dev/c/wallhaven-tags/favorites/wallhaven-x6oppv.json"),
        STR("/home/rphii/dev/c/wallhaven-tags/favorites/wallhaven-x6pkpd.json"),
        STR("/home/rphii/dev/c/wallhaven-tags/favorites/wallhaven-x6pp5z.json"),
        STR("/home/rphii/dev/c/wallhaven-tags/favorites/wallhaven-x6pqyd.json"),
        STR("/home/rphii/dev/c/wallhaven-tags/favorites/wallhaven-x6pzdo.json"),
        STR("/home/rphii/dev/c/wallhaven-tags/favorites/wallhaven-x6yk5d.json"),
        STR("/home/rphii/dev/c/wallhaven-tags/favorites/wallhaven-x6yq9d.json"),
        STR("/home/rphii/dev/c/wallhaven-tags/favorites/wallhaven-x88ql3.json"),
        STR("/home/rphii/dev/c/wallhaven-tags/favorites/wallhaven-x8gkvz.json"),
        STR("/home/rphii/dev/c/wallhaven-tags/favorites/wallhaven-x8kdpo.json"),
        STR("/home/rphii/dev/c/wallhaven-tags/favorites/wallhaven-x8pykd.json"),
        STR("/home/rphii/dev/c/wallhaven-tags/favorites/wallhaven-x8xz9d.json"),
        STR("/home/rphii/dev/c/wallhaven-tags/favorites/wallhaven-y82v7d.json"),
        STR("/home/rphii/dev/c/wallhaven-tags/favorites/wallhaven-y8671g.json"),
        STR("/home/rphii/dev/c/wallhaven-tags/favorites/wallhaven-y8ky2d.json"),
        STR("/home/rphii/dev/c/wallhaven-tags/favorites/wallhaven-y8l1dg.json"),
        STR("/home/rphii/dev/c/wallhaven-tags/favorites/wallhaven-y8ogmx.json"),
        STR("/home/rphii/dev/c/wallhaven-tags/favorites/wallhaven-y8q2g7.json"),
        STR("/home/rphii/dev/c/wallhaven-tags/favorites/wallhaven-y8z52d.json"),
        STR("/home/rphii/dev/c/wallhaven-tags/favorites/wallhaven-yjz5zk.json"),
        STR("/home/rphii/dev/c/wallhaven-tags/favorites/wallhaven-ym7qg7.json"),
        STR("/home/rphii/dev/c/wallhaven-tags/favorites/wallhaven-ymk8vk.json"),
        STR("/home/rphii/dev/c/wallhaven-tags/favorites/wallhaven-ymlkxx.json"),
        STR("/home/rphii/dev/c/wallhaven-tags/favorites/wallhaven-ymmp5g.json"),
        STR("/home/rphii/dev/c/wallhaven-tags/favorites/wallhaven-ymvopg.json"),
        STR("/home/rphii/dev/c/wallhaven-tags/favorites/wallhaven-ymwv6l.json"),
        STR("/home/rphii/dev/c/wallhaven-tags/favorites/wallhaven-yx9gm7.json"),
        STR("/home/rphii/dev/c/wallhaven-tags/favorites/wallhaven-yxev57.json"),
        STR("/home/rphii/dev/c/wallhaven-tags/favorites/wallhaven-yxg8pl.json"),
        STR("/home/rphii/dev/c/wallhaven-tags/favorites/wallhaven-yxjygg.json"),
        STR("/home/rphii/dev/c/wallhaven-tags/favorites/wallhaven-yxkvzd.json"),
        STR("/home/rphii/dev/c/wallhaven-tags/favorites/wallhaven-yxl11k.json"),
        STR("/home/rphii/dev/c/wallhaven-tags/favorites/wallhaven-yxo5qk.json"),
        STR("/home/rphii/dev/c/wallhaven-tags/favorites/wallhaven-yxodog.json"),
        STR("/home/rphii/dev/c/wallhaven-tags/favorites/wallhaven-yxoj1x.json"),
        STR("/home/rphii/dev/c/wallhaven-tags/favorites/wallhaven-yxowx7.json"),
        STR("/home/rphii/dev/c/wallhaven-tags/favorites/wallhaven-yxr8jg.json"),
        STR("/home/rphii/dev/c/wallhaven-tags/favorites/wallhaven-yxr9rk.json"),
        STR("/home/rphii/dev/c/wallhaven-tags/favorites/wallhaven-yxrd7k.json"),
        STR("/home/rphii/dev/c/wallhaven-tags/favorites/wallhaven-yxvozg.json"),
        STR("/home/rphii/dev/c/wallhaven-tags/favorites/wallhaven-z81z9v.json"),
        STR("/home/rphii/dev/c/wallhaven-tags/favorites/wallhaven-z85gkv.json"),
        STR("/home/rphii/dev/c/wallhaven-tags/favorites/wallhaven-z8ozjg.json"),
        STR("/home/rphii/dev/c/wallhaven-tags/favorites/wallhaven-z8vk9o.json"),
        STR("/home/rphii/dev/c/wallhaven-tags/favorites/wallhaven-z8vwrj.json"),
        STR("/home/rphii/dev/c/wallhaven-tags/favorites/wallhaven-z8zl6o.json"),
        STR("/home/rphii/dev/c/wallhaven-tags/favorites/wallhaven-zm7xjj.json"),
        STR("/home/rphii/dev/c/wallhaven-tags/favorites/wallhaven-zmlw7y.json"),
        STR("/home/rphii/dev/c/wallhaven-tags/favorites/wallhaven-zmr16j.json"),
        STR("/home/rphii/dev/c/wallhaven-tags/favorites/wallhaven-zxk12w.json"),
        STR("/home/rphii/dev/c/wallhaven-tags/favorites/wallhaven-zy2xgw.json"),
        STR("/home/rphii/dev/c/wallhaven-tags/favorites/wallhaven-zy3jqv.json"),
        STR("/home/rphii/dev/c/wallhaven-tags/favorites/wallhaven-zy55go.json"),
        STR("/home/rphii/dev/c/wallhaven-tags/favorites/wallhaven-zy6e1y.json"),
        STR("/home/rphii/dev/c/wallhaven-tags/favorites/wallhaven-zy6y1j.json"),
        STR("/home/rphii/dev/c/wallhaven-tags/favorites/wallhaven-zy7gew.json"),
        STR("/home/rphii/dev/c/wallhaven-tags/favorites/wallhaven-zygljg.json"),
        STR("/home/rphii/dev/c/wallhaven-tags/favorites/wallhaven-zyjkgg.json"),
        STR("/home/rphii/dev/c/wallhaven-tags/favorites/wallhaven-zyjz7o.json"),
        STR("/home/rphii/dev/c/wallhaven-tags/favorites/wallhaven-zyo1jo.json"),
        STR("/home/rphii/dev/c/wallhaven-tags/favorites/wallhaven-zyrr2w.json"),
        STR("/home/rphii/dev/c/wallhaven-tags/favorites/wallhaven-zyvl7j.json"),
    };

    Str in = {0};
    Str out = {0};
    for(size_t i = 0; i <= SIZE_ARRAY(files)-1; ++i) {
        str_clear(&in);
        Str *filename = &files[i];
        //printff("[%zu] parsing '%.*s'", i, STR_F(filename));
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

        TRYC(str_fmt_nodir(&out, &name.str));

        for(size_t i = 0; i < vjson_length(&array.arr); ++i) {
            TRYC(str_fmt(&out, ","));
            Json *tag = vjson_get_at(&array.arr, i);
            Json name = json_get(tag, &(JsonPath){.n = 1, .p = (JsonPathlet[]){(JsonPathlet){.k = STR("name")}}});
            Json purity = json_get(tag, &(JsonPath){.n = 1, .p = (JsonPathlet[]){(JsonPathlet){.k = STR("purity")}}});
            Json category = json_get(tag, &(JsonPath){.n = 1, .p = (JsonPathlet[]){(JsonPathlet){.k = STR("category")}}});
            if(str_cmp(&purity.str, &STR("sfw"))) {
                TRYC(str_fmt(&out, "%.*s:%.*s:%.*s", STR_F(&purity.str), STR_F(&category.str), STR_F(&name.str)));
            } else {
                TRYC(str_fmt(&out, "%.*s:%.*s", STR_F(&category.str), STR_F(&name.str)));
            }
        }

        json_free(&json);
        TRYC(str_fmt(&out, "\n"));
    }
    printf("%.*s", STR_F(&out));
    str_free(&in);
    str_free(&out);

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

#if 1
    //Str filename = STR("../data/sample.json");
    //Str filename = STR("../data/512KB.json");
    //Str filename = STR("../data/5MB-min.json");
    //Str filename = STR("../testfiles/instruments.json");

    Str files[] = {
        STR("../data/5MB-min.json"),
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
        STR("/home/rphii/.local/share/Steam/steamapps/compatdata/1325200/pfx/drive_c/users/steamuser/Desktop/b2b-h2y-y2h.json"),
        STR("/home/rphii/.local/share/Steam/steamapps/compatdata/1325200/pfx/drive_c/users/steamuser/Desktop/boss2boss-yokai2yokai-human2humans.json"),
        STR("/home/rphii/.local/share/Steam/steamapps/compatdata/1325200/pfx/drive_c/users/steamuser/Desktop/default.json"),
        STR("/home/rphii/.local/share/Steam/steamapps/compatdata/1325200/pfx/drive_c/users/steamuser/Desktop/default2.json"),
        STR("/home/rphii/.local/share/Steam/steamapps/compatdata/1325200/pfx/drive_c/users/steamuser/Desktop/empty.json"),
        STR("/home/rphii/.local/share/Steam/steamapps/compatdata/1325200/pfx/drive_c/users/steamuser/Desktop/hb2yb-yb2hb-ye2he-he2ye.json"),
        STR("/home/rphii/.local/share/Steam/steamapps/compatdata/1325200/pfx/drive_c/users/steamuser/Desktop/largefile-original.json"),
        STR("/home/rphii/.local/share/Steam/steamapps/compatdata/1325200/pfx/drive_c/users/steamuser/Desktop/largefile.json"),
        STR("/home/rphii/.local/share/Steam/steamapps/compatdata/1325200/pfx/drive_c/users/steamuser/Desktop/test-v0.0.13.json"),
        STR("/home/rphii/.local/share/Steam/steamapps/compatdata/1325200/pfx/drive_c/users/steamuser/Desktop/test.json"),
        STR("/home/rphii/.local/share/Steam/steamapps/compatdata/1325200/pfx/drive_c/users/steamuser/Desktop/testing.json"),
        STR("/home/rphii/.local/share/Steam/steamapps/compatdata/1325200/pfx/drive_c/users/steamuser/Desktop/wtf-2786327771-hollow.json"),
    };
    
    for(size_t i = 0; i < SIZE_ARRAY(files); ++i) {
        str_clear(&in);
        printff("%.*s", STR_F(&files[i]));
        TRYC(file_str_read(&files[i], &in));
        Json json = {0};
        TRYC(json_parse(&json, &in, 0, 0));

        //str_clear(&out);
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

    //list_tags();
    list_not_downloaded();
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

