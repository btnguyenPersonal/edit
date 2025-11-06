#pragma once

#include <string>
#include <cstdint>

class LRUNode {
	public:
		LRUNode* prev;
		LRUNode* next;
		uint32_t index;
		bool end;
		void* node;
		LRUNode();
};

class LRU {
	public:
		LRUNode* head;
		uint32_t maxSize;
		LRU(uint32_t maxSize);
		void add(uint32_t index, void* node);
		void* get(uint32_t index);
};
