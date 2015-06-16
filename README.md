Maven utilities for C & C++
---------------------------

Sometimes you have to interact with [Maven](https://maven.apache.org/) even when
you are not working with a Java project _per se_. This can be unfortunate, but
what can you do? Use this library!

## Building and installing

[CMake](http://www.cmake.org/)-based, so:

```
mkdir build
cd build
cmake ..
make && make install
```

## Comparing versions

Maven has a [very complicated
algorithm](https://cwiki.apache.org/confluence/display/MAVENOLD/Versioning) for
comparing version numbers. It has all sorts of magic about extensions like
`-cr` and `-milestone` numbers with `a` prefixes and digit suffixes, etc. Don't
work with these things yourselves; use the utility:

```
    struct maven_version *v1 = mv_parse("3.1.0-SNAPSHOT-ce44f2");
    struct maven_version *v2 = mv_parse("3.1.0");

    assert(0 > mv_compare(v1, v2)); /* snapshots precede releases */

    mv_free(v1);
    mv_free(v2);
```

Some simple C++ bindings are available.

## License

Copyright © 2015 Nathan Rosenblum <flander@gmail.com>

Licensed under the MIT License.
