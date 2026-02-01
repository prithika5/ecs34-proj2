#ifndef DATASINK_H
#define DATASINK_H

#include <vector>

class CDataSink{
    public:
        virtual ~CDataSink() = default; // LCOV_EXCL_LINE
        virtual bool Put(const char &ch) noexcept = 0; // LCOV_EXCL_LINE
        virtual bool Write(const std::vector<char> &buf) noexcept = 0; // LCOV_EXCL_LINE
};

#endif
