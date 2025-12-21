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

    // Добавляем дополнительное пространство для выравнивания
    std::size_t required_size = aligned_size(bytes, alignment);

    for (auto it = free_blocks_.begin(); it != free_blocks_.end(); ++it)
    {
        if (it->second.is_free && it->second.size >= required_size)
        {
            void *block_addr = it->first;
            std::size_t block_size = it->second.size;

            // Выравниваем адрес
            void *aligned_addr = align_pointer(block_addr, alignment);
            std::size_t alignment_padding = 
                static_cast<char*>(aligned_addr) - static_cast<char*>(block_addr);
            
            // Если перед выровненным адресом есть свободное пространство
            if (alignment_padding > 0)
            {
                free_blocks_[block_addr] = {alignment_padding, true};
                block_addr = aligned_addr;
                block_size -= alignment_padding;
            }

            free_blocks_.erase(block_addr);

            // Если после аллокации осталось свободное пространство
            if (block_size > required_size)
            {
                void *free_part = static_cast<char *>(block_addr) + required_size;
                std::size_t free_size = block_size - required_size;
                free_blocks_[free_part] = {free_size, true};
            }

            allocated_blocks_[block_addr] = {required_size, false};

            std::cout << "Allocated " << bytes << " bytes at " << block_addr 
                      << " (actual size: " << required_size << " bytes, alignment: " 
                      << alignment << ")" << std::endl;
            return block_addr;
        }
    }

    throw std::bad_alloc();
}

void MemoryResource::do_deallocate(void *p, std::size_t bytes [[maybe_unused]], std::size_t alignment [[maybe_unused]])
{
    auto it = allocated_blocks_.find(p);
    if (it == allocated_blocks_.end())
    {
        throw std::runtime_error("Attempt to deallocate unknown block");
    }

    std::size_t actual_size = it->second.size;
    allocated_blocks_.erase(it);

    free_blocks_[p] = {actual_size, true};

    merge_adjacent_free_blocks();

    std::cout << "Deallocated block at " << p << " (" << actual_size << " bytes)" << std::endl;
}

bool MemoryResource::do_is_equal(const std::pmr::memory_resource &other) const noexcept
{
    return this == &other;
}

void MemoryResource::merge_adjacent_free_blocks()
{
    if (free_blocks_.empty()) return;

    auto current = free_blocks_.begin();
    auto next = std::next(current);

    while (next != free_blocks_.end())
    {
        void *current_end = static_cast<char *>(current->first) + current->second.size;
        
        if (current_end == next->first)
        {
            // Объединяем текущий блок со следующим
            current->second.size += next->second.size;
            
            // Удаляем следующий блок
            free_blocks_.erase(next++);
        }
        else
        {
            ++current;
            ++next;
        }
    }
}