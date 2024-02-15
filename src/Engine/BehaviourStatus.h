#pragma once

enum class BehaviourStatus
{
	INVALID,
	RUNNING,
	SUSPENDED,
	SUCCESS,
	FAILURE,
	ABORTED,

	BSTATUS_COUNT
};

// TODO there's probably a more modern C++ way of getting an enum-string mapping like this
constexpr const char* StatusString[] = {
	"INVALID",
	"RUNNING",
	"SUSPENDED",
	"SUCCESS",
	"FAILURE",
	"ABORTED",
};
static_assert(std::size(StatusString) == (size_t)BehaviourStatus::BSTATUS_COUNT);
