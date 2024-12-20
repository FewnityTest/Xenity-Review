#include "memory_tracker.h"

#include <engine/assertions/assertions.h>

MemoryTracker::MemoryTracker(const std::string& name) : m_name(name)
{
	XASSERT(!name.empty(), "[MemoryTracker::MemoryTracker] Name is empty");
}

void MemoryTracker::Allocate(size_t size)
{
	m_allocatedMemory += size;
	m_allocCount++;
}

void MemoryTracker::Deallocate(size_t size)
{
	m_deallocatedMemory += size;
	m_deallocCount++;
}
