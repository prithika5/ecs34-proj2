#ifndef DATASOURCE_H
#define DATASOURCE_H

#include <vector>

class CDataSource{
    public:
        virtual ~CDataSource() = default; // LCOV_EXCL_LINE
        virtual bool End() const noexcept = 0; // LCOV_EXCL_LINE
        virtual bool Get(char &ch) noexcept = 0; // LCOV_EXCL_LINE
        virtual bool Peek(char &ch) noexcept = 0; // LCOV_EXCL_LINE
        virtual bool Read(std::vector<char> &buf, std::size_t count) noexcept = 0; // LCOV_EXCL_LINE
};


#endif
