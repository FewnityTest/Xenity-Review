// #include "fps_counter.h"
// #include "../xenity.h"

// FpsCounter::FpsCounter()
// {
// 	componentName = "FpsCounter";
// }

// void FpsCounter::Start()
// {
// }
// float lastFps = 0;

// void FpsCounter::Update()
// {
// 	if (textRenderer.lock())
// 	{
// 		lastFps += 1 / Time::GetDeltaTime();
// 		lastFps /= 2.0f;

// 		textRenderer.lock()->SetText("FPS: " + std::to_string(1 / Time::GetDeltaTime()) + "\navg: " + std::to_string(lastFps));
// 	}
// }

// std::unordered_map<std::string, ReflectiveEntry> FpsCounter::GetReflectiveData()
// {
// 	std::unordered_map<std::string, ReflectiveEntry> reflectedVariables;
// 	Reflective::AddVariable(reflectedVariables, textRenderer, "textRenderer", true);
// 	return reflectedVariables;
// }
