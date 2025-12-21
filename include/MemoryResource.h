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

    // Вспомогательная функция для выравнивания адреса
    static void* align_pointer(void* ptr, std::size_t alignment) {
        std::uintptr_t p = reinterpret_cast<std::uintptr_t>(ptr);
        std::size_t remainder = p % alignment;
        if (remainder != 0) {
            p += alignment - remainder;
        }
        return reinterpret_cast<void*>(p);
    }

    // Вычисление необходимого размера с учетом выравнивания
    static std::size_t aligned_size(std::size_t size, std::size_t alignment) {
        return size + alignment - 1;
    }

public:
    explicit MemoryResource(std::size_t total_size);
    ~MemoryResource() override;
    
    MemoryResource(const MemoryResource &) = delete;
    MemoryResource &operator=(const MemoryResource &) = delete;

    // Для отладки
    void dump() const {
        std::cout << "=== MemoryResource Dump ===" << std::endl;
        std::cout << "Total buffer size: " << buffer_size_ << " bytes" << std::endl;
        std::cout << "Buffer address: " << buffer_ << std::endl;
        
        std::cout << "\nAllocated blocks (" << allocated_blocks_.size() << "):" << std::endl;
        for (const auto& [addr, info] : allocated_blocks_) {
            std::cout << "  " << addr << " - " << info.size << " bytes" << std::endl;
        }
        
        std::cout << "\nFree blocks (" << free_blocks_.size() << "):" << std::endl;
        for (const auto& [addr, info] : free_blocks_) {
            std::cout << "  " << addr << " - " << info.size << " bytes" << std::endl;
        }
        std::cout << "=========================" << std::endl;
    }

protected:
    void *do_allocate(std::size_t bytes, std::size_t alignment) override;
    void do_deallocate(void *p, std::size_t bytes, std::size_t alignment) override;
    bool do_is_equal(const std::pmr::memory_resource &other) const noexcept override;
};

#endif // MEMORY_RESOURCE_H