#include "./lru.h"

LRUNode::LRUNode() {
	this->prev = nullptr;
	this->next = nullptr;
	this->node = nullptr;
	this->index = -1;
	this->end = false;
}

LRU::LRU(uint32_t maxSize) {
	LRUNode* last = new LRUNode();
	LRUNode* head = new LRUNode();
	head->next = last;
	last->prev = head;
	last->end = true;
	this->head = head;
	this->maxSize = maxSize;
}

void LRU::add(uint32_t index, void* node) {
	// TODO
}

void* LRU::get(uint32_t index) {
	LRUNode* temp = this->head;
	for (uint32_t i = 0; i < this->maxSize; i++) {
		temp = this->head;
		if (temp->end) {
			break;
		} else if (temp->index == index) {
			return temp->node;
			break;
		}
	}
	return nullptr;
}
