#include
"stdlib.h"
#include "memory_manager.h"

typedef struct {
	uint64 base;
	uint64 length;
	uint32 type;
	uint32 acpi_ext_attrs;
} __attribute__((packed)) MemoryMapEntry;

typedef struct {
	phyaddr next;
	phyaddr prev;
	size_t size;
} PhysMemoryBlock;

size_t free_page_count = 0;
phyaddr free_phys_memory_pointer = -1;

void init_memory_manager(void *memory_map) {
	asm("movl %%cr3, %0":"=a"(kernel_page_dir));
	memory_size = 0x100000;
	MemoryMapEntry *entry;
	for (entry = memory_map; entry->type; entry++) {
		if ((entry->type == 1) && (entry->base >= 0x100000)) {
			free_phys_pages(entry->base, entry->length >> PAGE_OFFSET_BITS);
			memory_size += entry->length;
		}
	}
	map_pages(kernel_page_dir, KERNEL_CODE_BASE, get_page_info(kernel_page_dir, KERNEL_CODE_BASE),
		((size_t)KERNEL_DATA_BASE - (size_t)KERNEL_CODE_BASE) >> PAGE_OFFSET_BITS, PAGE_PRESENT | PAGE_GLOBAL);
	map_pages(kernel_page_dir, KERNEL_DATA_BASE, get_page_info(kernel_page_dir, KERNEL_DATA_BASE),
		((size_t)KERNEL_END - (size_t)KERNEL_DATA_BASE) >> PAGE_OFFSET_BITS, PAGE_PRESENT | PAGE_WRITABLE | PAGE_GLOBAL);
	map_pages(kernel_page_dir, KERNEL_PAGE_TABLE, get_page_info(kernel_page_dir, KERNEL_PAGE_TABLE), 1, PAGE_PRESENT | PAGE_WRITABLE | PAGE_GLOBAL);
}

/* Physical memory manager */

size_t get_free_memory_size() {
	return free_page_count << PAGE_OFFSET_BITS;
}

phyaddr alloc_phys_pages(size_t count) {
	if (free_page_count < count) return -1;
	phyaddr result = -1;
	if (free_phys_memory_pointer != -1) {
		phyaddr cur_block = free_phys_memory_pointer;
		do {
			temp_map_page(cur_block);
			if (((volatile PhysMemoryBlock*)TEMP_PAGE)->size == count) {
				phyaddr next = ((volatile PhysMemoryBlock*)TEMP_PAGE)->next;
				phyaddr prev = ((volatile PhysMemoryBlock*)TEMP_PAGE)->prev;
				temp_map_page(next);
				((volatile PhysMemoryBlock*)TEMP_PAGE)->prev = prev;
				temp_map_page(prev);
				((volatile PhysMemoryBlock*)TEMP_PAGE)->next = next;
				if (cur_block == free_phys_memory_pointer) {
					free_phys_memory_pointer = next;
					if (cur_block == free_phys_memory_pointer) {
						free_phys_memory_pointer = -1;
					}
				}
				result = cur_block;
				break;
			} else if (((volatile PhysMemoryBlock*)TEMP_PAGE)->size > count) {
				((volatile PhysMemoryBlock*)TEMP_PAGE)->size -= count;
				result = cur_block + (((volatile PhysMemoryBlock*)TEMP_PAGE)->size << PAGE_OFFSET_BITS);
				break;
			}
			cur_block = ((volatile PhysMemoryBlock*)TEMP_PAGE)->next;
		} while (cur_block != free_phys_memory_pointer);
		if (result != -1) {
			free_page_count -= count;
		} 
	}
	return result;
}

void free_phys_pages(phyaddr base, size_t count) {
	if (free_phys_memory_pointer == -1) {
		temp_map_page(base);
		((volatile PhysMemoryBlock*)TEMP_PAGE)->next = base;
		((volatile PhysMemoryBlock*)TEMP_PAGE)->prev = base;
		((volatile PhysMemoryBlock*)TEMP_PAGE)->size = count;
		free_phys_memory_pointer = base;
	} else {
		phyaddr cur_block = free_phys_memory_pointer;
		do {
			temp_map_page(cur_block);
			if (cur_block + (((volatile PhysMemoryBlock*)TEMP_PAGE)->size << PAGE_OFFSET_BITS) == base) {
				((volatile PhysMemoryBlock*)TEMP_PAGE)->size += count;
				if (((volatile PhysMemoryBlock*)TEMP_PAGE)->next == base + (count << PAGE_OFFSET_BITS)) {
					phyaddr next1 = ((volatile PhysMemoryBlock*)TEMP_PAGE)->next;
					temp_map_page(next1);
					phyaddr next2 = ((volatile PhysMemoryBlock*)TEMP_PAGE)->next;
					size_t new_count = ((volatile PhysMemoryBlock*)TEMP_PAGE)->size;
					temp_map_page(next2);
					((volatile PhysMemoryBlock*)TEMP_PAGE)->prev = cur_block;
					temp_map_page(cur_block);
					((volatile PhysMemoryBlock*)TEMP_PAGE)->next = next2;
					((volatile PhysMemoryBlock*)TEMP_PAGE)->size += new_count;
				}
				break;
			} else if (base + (count << PAGE_OFFSET_BITS) == cur_block) {
				size_t old_count = ((volatile PhysMemoryBlock*)TEMP_PAGE)->size;
				phyaddr next = ((volatile PhysMemoryBlock*)TEMP_PAGE)->next;
				phyaddr prev = ((volatile PhysMemoryBlock*)TEMP_PAGE)->prev;
				temp_map_page(next);
				((volatile PhysMemoryBlock*)TEMP_PAGE)->prev = base;
				temp_map_page(prev);
				((volatile PhysMemoryBlock*)TEMP_PAGE)->next = base;
				temp_map_page(base);
				((volatile PhysMemoryBlock*)TEMP_PAGE)->next = next;
				((volatile PhysMemoryBlock*)TEMP_PAGE)->prev = prev;
				((volatile PhysMemoryBlock*)TEMP_PAGE)->size = count + old_count;
				break;
			} else if ((cur_block > base) || (((volatile PhysMemoryBlock*)TEMP_PAGE)->next == free_phys_memory_pointer)) {
				phyaddr prev = ((volatile PhysMemoryBlock*)TEMP_PAGE)->next;
				((volatile PhysMemoryBlock*)TEMP_PAGE)->prev = base;
				temp_map_page(prev);
				((volatile PhysMemoryBlock*)TEMP_PAGE)->next = base;
				temp_map_page(base);
				((volatile PhysMemoryBlock*)TEMP_PAGE)->next = cur_block;
				((volatile PhysMemoryBlock*)TEMP_PAGE)->prev = prev;
				((volatile PhysMemoryBlock*)TEMP_PAGE)->size = count;
				break;
			}
			cur_block = ((volatile PhysMemoryBlock*)TEMP_PAGE)->next;
		} while (cur_block != free_phys_memory_pointer);
		if (base < free_phys_memory_pointer) {
			free_phys_memory_pointer = base;
		}
		
	}
	free_page_count += count;
}

/* Lowlevel virtual memory manager */

static inline void flush_page_cache(void *addr) {
	asm("invlpg (,%0,)"::"a"(addr));
}

void temp_map_page(phyaddr addr) {
	*((phyaddr*)TEMP_PAGE_INFO) = (addr & ~PAGE_OFFSET_MASK) | PAGE_PRESENT | PAGE_WRITABLE;
	flush_page_cache((void*)TEMP_PAGE);
}

bool map_pages(phyaddr page_dir, void *vaddr, phyaddr paddr, size_t count, unsigned int flags) {
	for (; count; count--) {
		phyaddr page_table = page_dir;
		char shift;
		for (shift = PHYADDR_BITS - PAGE_TABLE_INDEX_BITS; shift >= PAGE_OFFSET_BITS; shift -= PAGE_TABLE_INDEX_BITS) {
			unsigned int index = ((size_t)vaddr >> shift) & PAGE_TABLE_INDEX_MASK;
			temp_map_page(page_table);
			if (shift > PAGE_OFFSET_BITS) {
				page_table = ((volatile phyaddr*)TEMP_PAGE)[index];
				if (!(page_table & PAGE_PRESENT)) {
					phyaddr addr = alloc_phys_pages(1);
					if (addr != -1) {
						temp_map_page(paddr);
						memset((void*)TEMP_PAGE, 0, PAGE_SIZE);
						temp_map_page(page_table);
						((volatile phyaddr*)TEMP_PAGE)[index] = addr | PAGE_PRESENT | PAGE_WRITABLE | PAGE_USER;
						page_table = addr;
					} else {
						return false;
					}
				}
			} else {
				((volatile phyaddr*)TEMP_PAGE)[index] = (paddr & ~PAGE_OFFSET_BITS) | flags;
				flush_page_cache(vaddr);
			}
		}
		vaddr += PAGE_SIZE;
		paddr += PAGE_SIZE;
	}
	return true;
}

phyaddr get_page_info(phyaddr page_dir, void *vaddr) {
	phyaddr page_table = page_dir;
	char shift;
	for (shift = PHYADDR_BITS - PAGE_TABLE_INDEX_BITS; shift >= PAGE_OFFSET_BITS; shift -= PAGE_TABLE_INDEX_BITS) {
		unsigned int index = ((size_t)vaddr >> shift) & PAGE_TABLE_INDEX_MASK;
		temp_map_page(page_table);
		if (shift > PAGE_OFFSET_BITS) {
			page_table = ((volatile phyaddr*)TEMP_PAGE)[index];
			if (!(page_table & PAGE_PRESENT)) {
				return 0;
			}
		} else {
			return ((volatile phyaddr*)TEMP_PAGE)[index];
		}
	}
}

/* Highlevel virtual memory manager */

void *alloc_virt_pages(AddressSpace *address_space, void *vaddr, phyaddr paddr, size_t count, unsigned int flags) {
	
}

void free_virt_pages(AddressSpace *address_space, void *vaddr, size_t count, unsigned int flags) {
	
} 