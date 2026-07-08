#pragma once
#ifndef FSDS_HPP_HEADER_GUARD
#define FSDS_HPP_HEADER_GUARD

#include "../lib/fast-thread-syncronization/src/fts.hpp"

#include "fsds_options.hpp"

#include "seperate_data_list.hpp"
#include "inline_list.hpp"
#include "linked_list.hpp"

#include "queue/finite_pqueue.hpp"
#include "queue/spsc_queue.hpp"
#include "queue/spmc_queue.hpp"
#include "queue/mpsc_queue.hpp"

#include "string/utf-8.hpp"
#include "string/static_string.hpp"
#include "string/dynamic_string.hpp"
#include "string/string_builder.hpp"

#include "stable_list/stable_list.hpp"

namespace fsds
{
	template<typename T, typename Allocator = std::allocator<T>>
	using List = fsds::SeperateDataList<T, Allocator>;
}

#endif //#ifndef FSDS_HPP_HEADER_GUARD