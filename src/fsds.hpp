#pragma once
#ifndef FSDS_HPP_HEADER_GUARD
#define FSDS_HPP_HEADER_GUARD

#include "../lib/fast-thread-syncronization/src/fts.hpp"

#include "fsds_options.hpp"

#include "seperate_data_list.hpp"
#include "inline_list.hpp"
#include "chunk_list.hpp"
#include "linked_list.hpp"
#include "spsc_queue.hpp"
#include "spmc_queue.hpp"
#include "mpsc_queue.hpp"
#include "finite_pqueue.hpp"

#include "utf-8.hpp"
#include "static_string.hpp"
#include "dynamic_string.hpp"
#include "string_builder.hpp"

namespace fsds
{
	template<typename T, typename Allocator = std::allocator<T>>
	using List = fsds::SeperateDataList<T, Allocator>;
}

#endif //#ifndef FSDS_HPP_HEADER_GUARD