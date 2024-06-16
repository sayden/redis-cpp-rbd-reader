#ifndef VALUE_H
#define VALUE_H

// A Redis value starts with an optional byte 0xFC or 0xFD if there's an expiration time for the key
// Then, the value type is specified by a byte that can be one of the following:
enum class ValueType
{
    STRING,
    LIST,
    SET,
    SORTED_SET,
    HASH,
    ZIP_MAP,
    ZIP_LIST,
    INT_SET,
    STREAM,
    SORTED_SET_ZIP_LIST,
    HASH_ZIP_LIST,
    LIST_QUICK_LIST,
    UNKNOWN,
};


namespace valuetype
{
ValueType get(char **);
}

#endif
