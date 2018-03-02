/*
 * List
 *
 *  Created on: 20170918
 *      Author: sfeq
 */

#ifndef LIST_
#define LIST_
#include<cstdio>
struct ListNode {
	size_t val;
	ListNode* next;
	ListNode() {
		next = NULL;
	}
	ListNode(const size_t& val) {
		next = NULL;
		this->val = val;
	}
};

struct List {
	ListNode* front;
	ListNode* back;
	List() {
		front = NULL;
		back = NULL;
	}
	~List() {
		clear();
	}

	void deleteValue(const size_t& val) {
		ListNode* nowpos;
		ListNode* nextpos;
		if (front == NULL) {
			return;
		}
		if (front->val == val) {
			if (front == back) {
				back = NULL;
			}
			nowpos = front->next;
			delete front;
			front = nowpos;
			return;
		}
		nowpos = front;
		while (1) {
			if (nowpos->next == NULL) {
				return;
			}
			if (nowpos->next->val == val) {
				nextpos = nowpos->next->next;
				delete nowpos->next;
				nowpos->next = nextpos;
				return;
			}
			nowpos = nowpos->next;
		}
	}
	void push_back(const size_t& val) {
		if (back == NULL) {
			front = back = new ListNode(val);
		} else {
			back->next = new ListNode(val);
			back = back->next;
		}
	}

	void clear() {
		if (front == NULL)
			return;

		ListNode* tmpson;
		while (front->next != NULL) {
			tmpson = front->next->next;
			delete front->next;
			front->next = tmpson;
		}
		delete front;
		front = NULL;
		back = NULL;
	}

	bool empty() {
		return front == NULL;
	}

};

#endif /* LIST_ */
