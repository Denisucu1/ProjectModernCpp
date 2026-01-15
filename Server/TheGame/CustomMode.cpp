#include "CustomMode.h"

CustomMode::CustomMode() : neutralPile(0)
{
}

Card CustomMode::GetNeutralPile() const
{
	return neutralPile;
}
