#pragma once
#include "pch.h"

namespace world {
	template<typename DataType>
	struct GraphNode
	{
		GraphNode(DataType&& data);
		GraphNode(DataType& data);
		DataType Data;
		vector<shared_ptr<GraphNode>> AdjacentNodes;
	};
	template<typename DataType>
	inline GraphNode<DataType>::GraphNode(DataType&& data) : Data(std::move(data))
	{
	}
	template<typename DataType>
	inline GraphNode<DataType>::GraphNode(DataType& data) : Data(data)
	{
	}
}