/*
 * Copyright (©) 2015 Nate Rosenblum
 *
 * Permission is hereby granted, free of charge, to any person obtaining a copy
 * of this software and associated documentation files (the "Software"), to deal
 * in the Software without restriction, including without limitation the rights
 * to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
 * copies of the Software, and to permit persons to whom the Software is
 * furnished to do so, subject to the following conditions:
 *
 *
 * THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
 * IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
 * FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
 * AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
 * LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
 * OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE
 * SOFTWARE.
 */

#include "c-maven-utils/maven-version.h"

#include <ctype.h>
#include <inttypes.h>
#include <stdlib.h>
#include <string.h>

#include "comparable-version.h"

struct maven_version {
    int major;
    int minor;
    int incremental;
    int build;
    struct comparable_version *comparable;
    char qualifier[0];
};

static struct maven_version* alloc_version(size_t qualifier_len) {
    struct maven_version *ret = (struct maven_version*) malloc(
        sizeof(struct maven_version) + qualifier_len + 1);
    memset(ret, 0, sizeof(struct maven_version) + qualifier_len + 1);
    ret->major = ret->minor = ret->incremental = ret->build = -1;
    return ret;
}

struct parsed_int {
    int value;
    int valid;
};

static struct parsed_int parse_int_limit(const char *str, const char *end,
        int base) {
    struct parsed_int ret = { 0, 0};
    char *last = NULL;
    ret.value = strtol(str, &last, base);
    ret.valid = (*str != '\0' && last == end);
    return ret;
}

static struct parsed_int parse_int(const char *str, int base) {
    return parse_int_limit(str, str + strlen(str), base);
}

static int starts_with_digit(const char *str) {
    return isdigit(*str);
}

/*
 * Implements the parsing algorithm from DefaultArtifactVersion in Maven 3.
 */
struct maven_version* mv_parse(const char *version) {
    const char *dash = strchr(version, '-');
    const char *part2 = dash ? dash + 1 : NULL;
    const char *part1;
    int release_p1 = 0;

    if (dash) {
        /* Simplifies use of null-termination-expecting methods below */
        part1 = strndup(version, dash - version);
        release_p1 = 1;
    } else {
        part1 = version;
    }

    int major, minor, incremental, build;
    major = minor = incremental = build = -1;
    const char *qualifier = NULL;

    if (part2) {
        if (strlen(part2) == 1 || *part2 != '0') {
            struct parsed_int p = parse_int(part2, /*base=*/ 10);
            if (p.valid) {
                build = p.value;
            } else {
                qualifier = part2;
            }
        }
    }

    if (!strchr(part1, '.') && *part1 != '0') {
        struct parsed_int p = parse_int(part1, /*base=*/ 10);
        if (p.valid) {
            major = p.value;
        } else {
            qualifier = version;
            build = -1;
        }
    } else {
        int fallback = 0;

        for (;;) {
            const char *cur = part1;
            const char *sep = strchr(cur, '.');

            struct parsed_int p = sep ?
                parse_int_limit(cur, sep, 10) : parse_int(cur, 10);
            if (p.valid) {
                major = p.value;
            } else {
                fallback = 1;
                break;
            }
            if (!sep) { break; }

            cur = sep + 1;
            sep = strchr(cur, '.');
            p = sep ? parse_int_limit(cur, sep, 10) : parse_int(cur, 10);
            if (p.valid) {
                minor = p.value;
            } else {
                fallback = 1;
                break;
            }
            if (!sep) { break; }

            cur = sep + 1;
            sep = strchr(cur, '.');
            p = sep ? parse_int_limit(cur, sep,10) : parse_int(cur, 10);
            if (p.valid) {
                incremental = p.value;
            } else {
                fallback = 1;
                break;
            }

            if (sep) {
                qualifier = sep + 1;
                fallback = starts_with_digit(qualifier);
            }

            break;
        }

        if (fallback) {
            qualifier = version;
            major = minor = incremental = build = -1;
        }
    }

    if (release_p1) {
        free((char *) part1);
    }

    struct maven_version *ret = alloc_version(
        qualifier ? strlen(qualifier) : 0);
    if (!ret) {
        return NULL;
    }
    ret->major = major;
    ret->minor = minor;
    ret->incremental = incremental;
    ret->build = build;

    if (qualifier) {
        memcpy(ret->qualifier, qualifier, strlen(qualifier));
    }

    ret->comparable = mv_internal_parse_comparable(version);

    return ret;
}

void mv_free(struct maven_version *version) {
    mv_internal_free_comparable(version->comparable);
    free(version);
}

int mv_major(struct maven_version *version) {
    return version->major;
}

int mv_minor(struct maven_version *version) {
    return version->minor;
}

int mv_incremental(struct maven_version *version) {
    return version->incremental;
}

int mv_build(struct maven_version *version) {
    return version->build;
}

const char* mv_qualifier(struct maven_version *version) {
    return version->qualifier;
}

int mv_compare(const struct maven_version *a, const struct maven_version *b) {
    return mv_internal_compare(a->comparable, b->comparable);
}
