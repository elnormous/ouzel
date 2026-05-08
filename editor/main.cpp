#include <memory>
#include <vector>
#include "../engine/core/Application.hpp"
#include "../engine/core/Engine.hpp"   // declares fluxion::main
#include "EditorApp.hpp"

// Editor entry point. Same shape as the samples' main.cpp: Fluxion calls
// fluxion::main(args) once after the engine and window are up; we hand
// back our Application subclass and the engine drives it from there.
//
// Cross-platform note: this file is identical for every desktop target
// because the engine's platform-specific WinMain / NSApplicationMain /
// linux main are all in libfluxion and dispatch through fluxion::main.
std::unique_ptr<fluxion::Application> fluxion::main(const std::vector<std::string>& /*args*/)
{
    return std::make_unique<fluxion::editor::EditorApp>();
}
