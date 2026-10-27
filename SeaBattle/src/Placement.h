#pragma once
#include "Core.h"
#include "Error.h"


class PlacementValidator {
	public:
		bool canPlace(const Board&, const Ship&, const Rules&);
		void validate(const std::vector<Ship>&, const Rules&);
};
