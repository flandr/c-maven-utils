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

#ifndef CPP_MAVEN_VERSION_H_
#define CPP_MAVEN_VERSION_H_

#include <memory>
#include <string>

#include "c-maven-utils/maven-version.h"

namespace mvn {

class Version {
public:
    explicit Version(std::string const& version);
    bool operator<(Version const& o) const;
    bool operator==(Version const& o) const;

    struct Deleter {
        void operator()(struct maven_version *v) const {
            mv_free(v);
        }
    };
private:
    std::shared_ptr<struct maven_version> version_;
};

Version::Version(std::string const& version)
    : version_(mv_parse(version.c_str()), Deleter()) { }

bool Version::operator<(Version const& o) const {
    return mv_compare(version_.get(), o.version_.get()) < 0;
}

bool Version::operator==(Version const& o) const {
    return mv_compare(version_.get(), o.version_.get()) == 0;
}

} // mvn namespace

#endif // CPP_MAVEN_VERSION_H_
