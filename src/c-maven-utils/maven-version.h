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

#ifndef MAVEN_VERSION_H_
#define MAVEN_VERSION_H_

#ifdef __cplusplus
extern "C" {
#endif

struct maven_version;

/**
 * Parse a string as a Maven version.
 *
 * Conformant Maven version strings are composed of a _major version_ and one
 * or more optional numerical components (_minor_, _incremental_, _build_) and
 * an optional string _qualifier_) [1].
 *
 * Callers must free the returned resource with `mv_free`.
 *
 * [1] http://www.mojohaus.org/versions-maven-plugin/version-rules.html
 *
 * @return an allocated version
 */
struct maven_version* mv_parse(const char *str);

/** Release an object allocated with `mv_parse`. */
void mv_free(struct maven_version*);

/** @return the major version number, or -1 if no major version is set. */
int mv_major(struct maven_version *);

/** @return the minor version number, or -1 if no minor version is set. */
int mv_minor(struct maven_version *);

/** @return the incremental version number, or -1 if no incremental version is set. */
int mv_incremental(struct maven_version *);

/** @return the build number, or -1 if no build number is set. */
int mv_build(struct maven_version *);

/** @return the qualifier, or NULL. */
const char* mv_qualifier(struct maven_version *);

#ifdef __cplusplus
}
#endif

#endif /* MAVEN_VERSION_H_ */
