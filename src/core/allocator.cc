#include "core/allocator.h"
#include <utility>

namespace infini
{
    Allocator::Allocator(Runtime runtime) : runtime(runtime)
    {
        // used是使用了的内存量
        used = 0;
        peak = 0;
        ptr = nullptr;

        // 'alignment' defaults to sizeof(uint64_t), because it is the length of
        // the longest data type currently supported by the DataType field of   alignment涉及的是硬件存储上的对齐，分配的size需要是某整数倍
        // the tensor
        alignment = sizeof(uint64_t);
    }

    Allocator::~Allocator()
    {
        if (this->ptr != nullptr)
        {
            runtime->dealloc(this->ptr);
        }
    }

    size_t Allocator::alloc(size_t size) {
        IT_ASSERT(this->ptr == nullptr);
        // pad the size to the multiple of alignment
        size = this->getAlignedSize(size);

        // =================================== 作业
        // ===================================
        // TODO: 设计一个算法来分配内存，返回起始地址偏移量
        // =================================== 作业
        // ===================================
        this->used += size;
        for (auto it = free_blocks.begin(); it != free_blocks.end(); it++) { // 遍历空闲块
            if (it->second >= size) {
            size_t addr = it->first;
            if (it->second > size) {
                free_blocks[addr + size] = it->second - size;
            }
            free_blocks.erase(it);
            return addr;
            }
        }
        // 在没有空闲块的情况下 peak = used
        this->peak += size;
        return this->peak - size;

    }

    void Allocator::free(size_t addr, size_t size)
    {
        IT_ASSERT(this->ptr == nullptr);
        size = getAlignedSize(size);

        // =================================== 作业 ===================================
        // TODO: 设计一个算法来回收内存
        // =================================== 作业 ===================================
        
        this->used -= size;
        if (addr + size == this->peak) { // 释放最后一个块
            this->peak -= size;
            return;
        }
        // 查询是否有相邻的空闲块
        for (auto it = free_blocks.begin(); it != free_blocks.end(); it++) {
            if (it->first + it->second == addr) { // 和前面的空闲块相邻
            it->second += size;
            return;
            }
            if (addr + size == it->first) { // 和后面的空闲块相邻
            free_blocks[addr] = size + it->second;
            free_blocks.erase(it);
            return;
            }
        }
        free_blocks[addr] = size; // 其余添加到空闲list中。


    }

    void *Allocator::getPtr()
    {
        if (this->ptr == nullptr)
        {
            this->ptr = runtime->alloc(this->peak);
            printf("Allocator really alloc: %p %lu bytes\n", this->ptr, peak);
        }
        return this->ptr;
    }

    size_t Allocator::getAlignedSize(size_t size)
    {
        return ((size - 1) / this->alignment + 1) * this->alignment;
    }

    void Allocator::info()
    {
        std::cout << "Used memory: " << this->used
                  << ", peak memory: " << this->peak << std::endl;
    }
}
