/*
	Yoyo-Algorithm : processing of yoyo-algorithm for message complexity.
    Copyright (C) 2014  Arthur Bourjac

    This program is free software: you can redistribute it and/or modify
    it under the terms of the GNU General Public License as published by
    the Free Software Foundation, either version 3 of the License, or
    (at your option) any later version.

    This program is distributed in the hope that it will be useful,
    but WITHOUT ANY WARRANTY; without even the implied warranty of
    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
    GNU General Public License for more details.

    You should have received a copy of the GNU General Public License
    along with this program.  If not, see <http://www.gnu.org/licenses/>.
*/

#ifndef DEF_GRAPH
#define DEF_GRAPH

#include <unordered_map>
#include <memory>
#include <mutex>

#include "node.h"

class Node;

class Graph
{
	public:
		/* --------------- ATTRIBUTES --------------- */
		
		/* --------------- FUNCTIONS --------------- */
		Graph(std::string p_filePath);
		std::unordered_multimap<std::shared_ptr<Node>,unsigned long>* Nodes();
		std::unordered_multimap<std::shared_ptr<Node>,unsigned long>::iterator findNode(Node* p_node);
		unsigned long countNodes(Node* p_nodep_node);
		
		void yoyo(bool p_verb);
		
		void buildDAG();
		void sendIdDAG(Node& p_from, unsigned long p_id, Node& p_to);
		void sendIdYoyo(Node& p_from, unsigned long p_id, Node& p_to);
		void sendAnswerYoyo(Node& p_from, bool p_answer, bool p_prune, Node& p_to);

		unsigned long MsgCount();
		void assignLeader(Node& p_leader);

#ifndef NDEBUG
		std::mutex m_mtxDebug;
		void writeDebug(std::string p_msg);
#endif

	private:
		/* --------------- ATTRIBUTES --------------- */
		std::unordered_multimap<std::shared_ptr<Node>,unsigned long> m_nodes;
		unsigned long m_msgCount = 0;
		std::mutex m_mtxMsgCount;
		bool m_leaderIsFound = false;
		Node* m_leader;

		/* --------------- FUNCTIONS --------------- */
};

#endif