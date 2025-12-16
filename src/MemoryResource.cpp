#include "MemoryResource.h"
#include <algorithm>
#include <cstring>

MemoryResource::MemoryResource(std::size_t total_size)
    : buffer_(::operator new(total_size)), buffer_size_(total_size)
{
    free_blocks_[buffer_] = {total_size, true};

    std::cout << "MemoryResource created with " << total_size
              << " bytes at " << buffer_ << std::endl;
}

MemoryResource::~MemoryResource()
{
    if (!allocated_blocks_.empty())
    {
        std::cerr << "Warning: MemoryResource destroyed with "
                  << allocated_blocks_.size() << " allocated blocks!" << std::endl;
    }

    ::operator delete(buffer_);
}

void *MemoryResource::do_allocate(std::size_t bytes, std::size_t alignment)
{
    if (bytes == 0)
        bytes = 1;

    for (auto it = free_blocks_.begin(); it != free_blocks_.end(); ++it)
    {
        if (it->second.is_free && it->second.size >= bytes)
        {

            void *block_addr = it->first;
            std::size_t block_size = it->second.size;

            free_blocks_.erase(it);

            if (block_size > bytes)
            {
                void *free_part = static_cast<char *>(block_addr) + bytes;
                std::size_t free_size = block_size - bytes;
                free_blocks_[free_part] = {free_size, true};
            }

            allocated_blocks_[block_addr] = {bytes, false};

            std::cout << "Allocated " << bytes << " bytes at " << block_addr << std::endl;
            return block_addr;
        }
    }

    throw std::bad_alloc();
}

void MemoryResource::do_deallocate(void *p, std::size_t bytes, std::size_t alignment)
{
    (void)bytes;
    (void)alignment;

    auto it = allocated_blocks_.find(p);
    if (it == allocated_blocks_.end())
    {
        throw std::runtime_error("Attempt to deallocate unknown block");
    }

    std::size_t size = it->second.size;
    allocated_blocks_.erase(it);

    free_blocks_[p] = {size, true};

    merge_adjacent_free_blocks();

    std::cout << "Deallocated block at " << p << " (" << size << " bytes)" << std::endl;
}

bool MemoryResource::do_is_equal(const std::pmr::memory_resource &other) const noexcept
{
    return this == &other;
}

void MemoryResource::merge_adjacent_free_blocks()
{
    bool merged;
    do
    {
        merged = false;
        auto it = free_blocks_.begin();

        while (it != free_blocks_.end())
        {
            auto next_it = it;
            ++next_it;

            if (next_it != free_blocks_.end())
            {
                void *current_end = static_cast<char *>(it->first) + it->second.size;

                if (current_end == next_it->first)
                {
                    it->second.size += next_it->second.size;
                    free_blocks_.erase(next_it);
                    merged = true;
                    break;
                }
            }
            ++it;
        }
    } while (merged);
}