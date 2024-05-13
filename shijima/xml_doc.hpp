#pragma once
// Do not include in headers

#include <rapidxml/rapidxml.hpp>
#include <vector>

using namespace rapidxml;

#define xml_doc(var, str, opts) \
    xml_document<> var; \
    std::vector<char> var##_##backingVector(str.begin(), str.end()); \
    var##_##backingVector.push_back('\0'); \
    var.parse<opts>((char *)&var##_##backingVector[0])