#pragma once

#include <string>
#include <cstddef>

class COWVector {
public:
    COWVector();
    ~COWVector();

    COWVector(const COWVector& other);
    COWVector& operator=(const COWVector& other);

    size_t Size() const;

    void Resize(size_t size);

    const std::string& Get(size_t at) const;
    const std::string& Back() const;

    void PushBack(const std::string& value);

    void Set(size_t at, const std::string& value);

private:
};
