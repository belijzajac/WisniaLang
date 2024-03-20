// Copyright (C) 2019-2024 Tautvydas Povilaitis (belijzajac)
// SPDX-License-Identifier: GPL-3.0

#ifndef WISNIALANG_REGISTER_HPP
#define WISNIALANG_REGISTER_HPP

#include <unordered_map>

namespace Wisnia::Basic {

enum register_t : uint8_t {
  // RAX - RDI
  RAX, RCX, RDX, RBX, RSP, RBP, RSI, RDI,
  // R8 - R15
  R8, R9, R10, R11, R12, R13, R14, R15,
  // Other
  EDX, ESI, DL, SPILLED
};

static inline std::unordered_map<register_t, std::string_view> Register2Str {
  {register_t::SPILLED, "[SPILLED]"},
  // RAX - RDI
  {register_t::RAX, "RAX"},
  {register_t::RCX, "RCX"},
  {register_t::RDX, "RDX"},
  {register_t::RBX, "RBX"},
  {register_t::RSP, "RSP"},
  {register_t::RBP, "RBP"},
  {register_t::RSI, "RSI"},
  {register_t::RDI, "RDI"},
  // R8 - R15
  {register_t::R8,  "R8" },
  {register_t::R9,  "R9" },
  {register_t::R10, "R10"},
  {register_t::R11, "R11"},
  {register_t::R12, "R12"},
  {register_t::R13, "R13"},
  {register_t::R14, "R14"},
  {register_t::R15, "R15"},
  // Other
  {register_t::EDX, "EDX"},
  {register_t::ESI, "ESI"},
  {register_t::DL,  "DL" },
};

}  // namespace Wisnia::Basic

#endif  // WISNIALANG_REGISTER_HPP
