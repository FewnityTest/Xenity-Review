// SPDX-License-Identifier: MIT
//
// Copyright (c) 2022-2024 Gregory Machefer (Fewnity)
//
// This file is part of Xenity Engine

#include "iDrawable.h"

#include "graphics.h"

void IDrawable::RemoveReferences()
{
	Graphics::RemoveDrawable(this);
}
