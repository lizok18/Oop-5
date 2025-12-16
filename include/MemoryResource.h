#ifndef MEMORY_RESOURCE_H
#define MEMORY_RESOURCE_H

#include <memory_resource>
#include <map>
#include <new>
#include <stdexcept>
#include <cstddef>
#include <cstdint>
#include <iostream>

class MemoryResource : public std::pmr::memory_resource
{
private:
    struct BlockInfo
    {
        std::size_t size;
        bool is_free;
    };

    void *buffer_;
    std::size_t buffer_size_;
    std::map<void *, BlockInfo> allocated_blocks_;
    std::map<void *, BlockInfo> free_blocks_;

    void merge_adjacent_free_blocks();

public:
    explicit MemoryResource(std::size_t total_size);
    ~MemoryResource() override;
    
    MemoryResource(const MemoryResource &) = delete;
    MemoryResource &operator=(const MemoryResource &) = delete;

protected:
    void *do_allocate(std::size_t bytes, std::size_t alignment) override;
    void do_deallocate(void *p, std::size_t bytes, std::size_t alignment) override;
    bool do_is_equal(const std::pmr::memory_resource &other) const noexcept override;
};

#endif