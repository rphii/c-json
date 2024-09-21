#include <stdio.h>
#include <time.h>


#include "str.h"
#include "file.h"
#include "json.h"

char *commaize(unsigned int n) {
    char s1[64];
    char *s2 = (char *)malloc(64);
    assert(s2);
    memset(s2, 0, sizeof(64));
    snprintf(s1, sizeof(s1), "%d", n);
    int i = strlen(s1)-1; 
    int j = 0;
    while (i >= 0) {
        if (j%3 == 0 && j != 0) {
            memmove(s2+1, s2, strlen(s2)+1);
            s2[0] = ',';
        }
        memmove(s2+1, s2, strlen(s2)+1);
        s2[0] = s1[i];
        i--;
        j++;
    }
    return s2;
}

double now(void) {
    struct timespec now;
    clock_gettime(CLOCK_MONOTONIC, &now);
    return (now.tv_sec*1e9 + now.tv_nsec) / 1e9;
}

#define bench(name, nsecs, fn) \
    int count = 0; \
    double start = now(); \
    size_t isize = 0; \
    size_t tsize = 0; \
    while (now()-start < (nsecs)) { \
        for (int i=0;i<10;i++) { \
            isize = len; \
            fn; \
            tsize += isize; \
            count++; \
        } \
    } \
    double elapsed = now()-start; \
    char *bsize[] = { "MB", "GB", "TB" }; int bidx = 0; \
    double bsecs = ((double)tsize)/elapsed/1024/1024; \
    while(bsecs > 1024 && bidx < 3) { bsecs /= 1024; bidx++; }; \
    printf("%-35s %s ops/sec ",(name), commaize(count/elapsed)); \
    if (elapsed/count*1e9 < 10) printf("%.1f ns/op ", elapsed/count*1e9); \
    else printf("%.0f ns/op ", elapsed/count*1e9); \
    printf("%.2f %s/s\n", bsecs, bsize[bidx]);

int main(void) {
    int err = 0;
    
    Str in = {0};
    Str out = {0};
    Json json = {0};

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
        Str filename = files[i];
        //printff("%.*s", STR_F(&filename));
        size_t len = in.last;
        TRYC(file_str_read(filename, &in));
        bench(str_iter_begin(filename), 0.10, {
            TRYC(json_parse(&json, &in, 0, 0));
            //TRYC(json_fmt(&json, &out, 0, 0));
            json_free(&json);
            isize = str_length(in);
        });
        //printf("%.*s\n", STR_F(&out));

    }

clean:
    str_free(&in);
    str_free(&out);
    json_free(&json);
    return err;
error:
    ERR_CLEAN;
}

