// SPDX-License-Identifier: MIT
//
// Copyright (c) 2022-2024 Gregory Machefer (Fewnity)
//
// This file is part of Xenity Engine

#pragma once

#include <engine/component.h>

// GameObject / Transforms
#include <engine/game_elements/gameobject.h>
#include <engine/game_elements/transform.h>
#include <engine/game_elements/rect_transform.h>

// Vectors
#include <engine/vectors/vector2.h>
#include <engine/vectors/vector2_int.h>
#include <engine/vectors/vector3.h>
#include <engine/vectors/vector4.h>
#include <engine/vectors/quaternion.h>

// UI
#include <engine/ui/screen.h>

// Tools
#include <engine/tools/benchmark.h>
#include <engine/tools/curve.h>
#include <engine/tools/math.h>
#include <engine/tools/profiler_benchmark.h>
#include <engine/tools/shape_spawner.h>

// Time
#include <engine/time/time.h>

// Scenes
#include <engine/scene_management/scene.h>
#include <engine/scene_management/scene_manager.h>

// Lighting
#include <engine/lighting/lighting.h>

// inputs
#include <engine/inputs/input_system.h>

// Graphics
#include <engine/graphics/camera.h>
#include <engine/graphics/camera_projection_types.h>
#include <engine/graphics/graphics.h>
#include <engine/graphics/shader.h>
#include <engine/graphics/material.h>
#include <engine/graphics/texture.h>
#include <engine/graphics/skybox.h>
#include <engine/graphics/3d_graphics/mesh_data.h>
#include <engine/graphics/3d_graphics/mesh_renderer.h>
#include <engine/graphics/2d_graphics/sprite_renderer.h>
#include <engine/graphics/2d_graphics/billboard_renderer.h>
#include <engine/graphics/2d_graphics/tile_map.h>
#include <engine/graphics/2d_graphics/line_renderer.h>
#include <engine/graphics/color/color.h>
#include <engine/graphics/ui/text_mesh.h>
#include <engine/graphics/ui/font.h>
#include <engine/graphics/ui/text_renderer.h>
#include <engine/graphics/ui/text_alignments.h>
#include <engine/graphics/ui/canvas.h>

// Files
#include <engine/file_system/file_system.h>
#include <engine/file_system/file.h>
#include <engine/file_system/directory.h>

// Debug
#include <engine/debug/debug.h>
#include <engine/debug/performance.h>

// Audio
#include <engine/audio/audio_clip.h>
#include <engine/audio/audio_source.h>

// IA
#include <engine/pathfinding/astar.h>
// #include <engine/pathfinding/dijkstras.h>

// Noise
#include <engine/noise/noise.h>

// Network
#include <engine/network/network.h>

// Physics
#include <engine/physics/box_collider.h>
#include <engine/physics/rigidbody.h>
#include <engine/physics/raycast.h>
#include <engine/physics/collision_event.h>

// Utils
#include <engine/tools/gameplay_utility.h>

// Event System
#include <engine/event_system/event_system.h>

// Particles
#include <engine/particle_system/particle_system.h>