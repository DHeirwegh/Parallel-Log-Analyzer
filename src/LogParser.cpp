// LogParser.cpp
#include "LogParser.h"
#include <sstream>
#include <vector>

bool parse_int(const char* begin, const char* end, int& out)
{
    if (begin == end)
        return false; // empty
    long long value = 0;
    for (const char* p = begin; p < end; ++p)
    {
        unsigned char c = static_cast<unsigned char>(*p);
        if (c < '0' || c > '9')
            return false;
        value = value * 10 + (c - '0');
        if (value > std::numeric_limits<int>::max())
            return false;
    }
    out = static_cast<int>(value);
    return true;
}


std::optional<LogEntry> parseLine(const std::string &line)
{
    // Expect exactly9 fields separated by '|'
    const char* data = line.data();
    const size_t n = line.size();

    size_t fieldIndex = 0;
    size_t start = 0;

    LogEntry entry; // we'll fill as we go

    auto commitField = [&](size_t idx, size_t s, size_t e) -> bool {
        const char* b = data + s;
        const char* t = data + e;
        const size_t len = static_cast<size_t>(t - b);

        // All fields must be present (non-empty)
        if (len == 0)
            return false;
        switch (idx)
        {
        case 0:
            entry.timestamp.assign(b, len);
            break;
        case 1:
            entry.logLevel.assign(b, len);
            break;
        case 2:
            entry.requestId.assign(b, len);
            break;
        case 3:
            entry.sourceIp.assign(b, len);
            break;
        case 4:
            entry.httpMethod.assign(b, len);
            break;
        case 5:
            entry.endpoint.assign(b, len);
            break;
        case 6:
            if(!parse_int(b, t, entry.statusCode))
				return false;
            break;
        case 7:
            if (!parse_int(b,t,entry.responseTimeMs))
                return false;
            break;
        case 8:
            entry.message.assign(b, len);
            break;
        default:
            return false; // too many fields
        }
        return true;
        };

    for (size_t i = 0; i <= n; ++i)
    {
        if (i == n || data[i] == '|')
        {
            // token is [start, i)
            if (!commitField(fieldIndex, start, i))
                return std::nullopt;
            ++fieldIndex;
            start = i + 1;
        }
    }

    if (fieldIndex != 9)
    {
        // malformed (too few or too many fields)
        return std::nullopt;
    }
   
    return entry;

}