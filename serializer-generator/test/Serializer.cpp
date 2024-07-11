#include "Serializer.hpp"

#include "test.h"

void Serializer::operator()(const Another &object)
{
    (*this)(object.name);
}

void Unserializer::operator()(Another &object)
{
    (*this)(object.name);
}

void Serializer::operator()(const TestStruct &object)
{
    (*this)(static_cast<const Another&>(object));
    (*this)(object.volume);
    (*this)(object.count);
}

void Unserializer::operator()(TestStruct &object)
{
    (*this)(static_cast<Another&>(object));
    (*this)(object.volume);
    (*this)(object.count);
}

