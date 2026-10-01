#pragma once

namespace pg::updater {

// Repositorio publico del que se leen las releases (GitHub Releases API).
inline constexpr const char *kOwnerRepo = "AngelTrujilloMarrero/PerenkeGain";
inline constexpr const char *kApiBase = "https://api.github.com";
inline constexpr int kTimeoutMs = 8000;

} // namespace pg::updater
