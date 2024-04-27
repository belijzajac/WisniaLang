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
  {SPILLED, "[SPILLED]"},
  // RAX - RDI
  {RAX, "RAX"},
  {RCX, "RCX"},
  {RDX, "RDX"},
  {RBX, "RBX"},
  {RSP, "RSP"},
  {RBP, "RBP"},
  {RSI, "RSI"},
  {RDI, "RDI"},
  // R8 - R15
  {R8,  "R8" },
  {R9,  "R9" },
  {R10, "R10"},
  {R11, "R11"},
  {R12, "R12"},
  {R13, "R13"},
  {R14, "R14"},
  {R15, "R15"},
  // Other
  {EDX, "EDX"},
  {ESI, "ESI"},
  {DL,  "DL" },
};

}  // namespace Wisnia::Basic

#endif  // WISNIALANG_REGISTER_HPP
