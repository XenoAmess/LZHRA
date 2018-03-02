/*
 * Map.h
 *
 *  Created on: 2017年9月18日
 *      Author: sfeq
 */

#ifndef MAP_H_
#define MAP_H_
#include"Common.h"

struct MapNode {
	MapNode* father;
	MapNode* sons[2];
	List* val;

	MapNode* buildSon(unsigned char index) {
		sons[index] = new MapNode(this);
		return sons[index];
	}

	MapNode(MapNode* father = NULL) {
		val = NULL;
		this->father = father;
		sons[0] = NULL;
		sons[1] = NULL;
	}

	void clear() {
		if (sons[0] != NULL) {
			sons[0]->clear();
			delete sons[0];
		}
		if (sons[1] != NULL) {
			sons[1]->clear();
			delete sons[1];
		}
		if (val != NULL) {
			val->clear();
			delete val;
		}
		sons[0] = NULL;
		sons[1] = NULL;
	}
};
struct Map {
	MapNode* head;

	Map() {
		head = new MapNode(NULL);
	}

	~Map() {
		clear();
	}

	void clear() {
		head->clear();
	}

	List & operator [](size_t val) {
		MapNode* nowpos = head;

		unsigned char nowbit;

		do {
			nowbit = (val & (size_t) 1);
			val >>= 1;
			if (nowpos->sons[nowbit] == NULL) {
				nowpos->buildSon(nowbit);
			}
			nowpos = nowpos->sons[nowbit];
		} while (val);

		if (nowpos->val == NULL)
			nowpos->val = new List();
		return *nowpos->val;
	}

//	List & operator [](size_t val) {
//		MapNode* nowfather = head;
//		unsigned int nowbit = 0;
//		MapNode* nowpos = nowfather->sons[nowbit];
//		while (val) {
//			nowbit = (val & (size_t) 1) ? 1 : 0;
//			//std::cout << val << " " << nowbit << std::endl;
//			val >>= 1;
//			if (nowpos == NULL) {
//				nowpos = nowfather->buildSon(nowbit);
//			}
//			nowfather = nowpos;
//			nowpos = nowfather->sons[nowbit];
//		}
//		if (nowpos == NULL) {
//			nowpos = nowfather->buildSon(nowbit);
//		}
//		if (nowpos->val == NULL)
//			nowpos->val = new List();
//		std::cout << std::endl << std::endl;
//		return *nowpos->val;
//	}

	bool find(size_t val) {
		MapNode* nowpos = head;

		unsigned char nowbit;

		do {
			nowbit = (val & (size_t) 1);
			val >>= 1;
			if (nowpos->sons[nowbit] == NULL) {
				return false;
			}
			nowpos = nowpos->sons[nowbit];
		} while (val);

		return true;
	}

};

#endif /* MAP_H_ */
