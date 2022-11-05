/***

  WisniaLang - A Compiler for an Experimental Programming Language
  Copyright (C) 2022 Tautvydas Povilaitis (belijzajac) and contributors

  This program is free software: you can redistribute it and/or modify
  it under the terms of the GNU General Public License as published by
  the Free Software Foundation, either version 3 of the License, or
  (at your option) any later version.

  This program is distributed in the hope that it will be useful,
  but WITHOUT ANY WARRANTY; without even the implied warranty of
  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the
  GNU General Public License for more details.

  You should have received a copy of the GNU General Public License
  along with this program. If not, see <http://www.gnu.org/licenses/>.

***/

#include <algorithm>
#include <array>
#include <cassert>
#include <iostream>
// Wisnia
#include "CodeGenerator.hpp"
#include "ELF.hpp"
#include "Instruction.hpp"
#include "Token.hpp"

using namespace Wisnia;
using namespace Basic;

static inline std::unordered_map<std::string_view, ByteArray> LeaMachineCode {
  {"rax", ByteArray{std::byte{0x48}, std::byte{0x8d}, std::byte{0x84}, std::byte{0x24}}},
  {"rcx", ByteArray{std::byte{0x48}, std::byte{0x8d}, std::byte{0x8c}, std::byte{0x24}}},
  {"rdx", ByteArray{std::byte{0x48}, std::byte{0x8d}, std::byte{0x94}, std::byte{0x24}}},
  {"rbx", ByteArray{std::byte{0x48}, std::byte{0x8d}, std::byte{0x9c}, std::byte{0x24}}},
  {"rsp", ByteArray{std::byte{0x48}, std::byte{0x8d}, std::byte{0xa4}, std::byte{0x24}}},
  {"rbp", ByteArray{std::byte{0x48}, std::byte{0x8d}, std::byte{0xac}, std::byte{0x24}}},
  {"rsi", ByteArray{std::byte{0x48}, std::byte{0x8d}, std::byte{0xb4}, std::byte{0x24}}},
  {"rdi", ByteArray{std::byte{0x48}, std::byte{0x8d}, std::byte{0xbc}, std::byte{0x24}}},
  {"r8",  ByteArray{std::byte{0x4c}, std::byte{0x8d}, std::byte{0x84}, std::byte{0x24}}},
  {"r9",  ByteArray{std::byte{0x4c}, std::byte{0x8d}, std::byte{0x8c}, std::byte{0x24}}},
  {"r10", ByteArray{std::byte{0x4c}, std::byte{0x8d}, std::byte{0x94}, std::byte{0x24}}},
  {"r11", ByteArray{std::byte{0x4c}, std::byte{0x8d}, std::byte{0x9c}, std::byte{0x24}}},
  {"r12", ByteArray{std::byte{0x4c}, std::byte{0x8d}, std::byte{0xa4}, std::byte{0x24}}},
  {"r13", ByteArray{std::byte{0x4c}, std::byte{0x8d}, std::byte{0xac}, std::byte{0x24}}},
  {"r14", ByteArray{std::byte{0x4c}, std::byte{0x8d}, std::byte{0xb4}, std::byte{0x24}}},
  {"r15", ByteArray{std::byte{0x4c}, std::byte{0x8d}, std::byte{0xbc}, std::byte{0x24}}},
  {"edx", ByteArray{std::byte{0x8d}, std::byte{0x94}, std::byte{0x24}}},
};

static inline std::unordered_map<std::string_view, ByteArray> MovMachineCode {
  {"rax", ByteArray{std::byte{0x48}, std::byte{0xc7}, std::byte{0xc0}}},
  {"rcx", ByteArray{std::byte{0x48}, std::byte{0xc7}, std::byte{0xc1}}},
  {"rdx", ByteArray{std::byte{0x48}, std::byte{0xc7}, std::byte{0xc2}}},
  {"rbx", ByteArray{std::byte{0x48}, std::byte{0xc7}, std::byte{0xc3}}},
  {"rsp", ByteArray{std::byte{0x48}, std::byte{0xc7}, std::byte{0xc4}}},
  {"rbp", ByteArray{std::byte{0x48}, std::byte{0xc7}, std::byte{0xc5}}},
  {"rsi", ByteArray{std::byte{0x48}, std::byte{0xc7}, std::byte{0xc6}}},
  {"rdi", ByteArray{std::byte{0x48}, std::byte{0xc7}, std::byte{0xc7}}},
  {"r8",  ByteArray{std::byte{0x49}, std::byte{0xc7}, std::byte{0xc0}}},
  {"r9",  ByteArray{std::byte{0x49}, std::byte{0xc7}, std::byte{0xc1}}},
  {"r10", ByteArray{std::byte{0x49}, std::byte{0xc7}, std::byte{0xc2}}},
  {"r11", ByteArray{std::byte{0x49}, std::byte{0xc7}, std::byte{0xc3}}},
  {"r12", ByteArray{std::byte{0x49}, std::byte{0xc7}, std::byte{0xc4}}},
  {"r13", ByteArray{std::byte{0x49}, std::byte{0xc7}, std::byte{0xc5}}},
  {"r14", ByteArray{std::byte{0x49}, std::byte{0xc7}, std::byte{0xc6}}},
  {"r15", ByteArray{std::byte{0x49}, std::byte{0xc7}, std::byte{0xc7}}},
};

static inline std::unordered_map<std::string_view, ByteArray> PushMachineCode {
  {"rax", ByteArray{std::byte{0x50}}},
  {"rcx", ByteArray{std::byte{0x51}}},
  {"rdx", ByteArray{std::byte{0x52}}},
  {"rbx", ByteArray{std::byte{0x53}}},
  {"rsp", ByteArray{std::byte{0x54}}},
  {"rbp", ByteArray{std::byte{0x55}}},
  {"rsi", ByteArray{std::byte{0x56}}},
  {"rdi", ByteArray{std::byte{0x57}}},
  {"r8",  ByteArray{std::byte{0x41}, std::byte{0x50}}},
  {"r9",  ByteArray{std::byte{0x41}, std::byte{0x51}}},
  {"r10", ByteArray{std::byte{0x41}, std::byte{0x52}}},
  {"r11", ByteArray{std::byte{0x41}, std::byte{0x53}}},
  {"r12", ByteArray{std::byte{0x41}, std::byte{0x54}}},
  {"r13", ByteArray{std::byte{0x41}, std::byte{0x55}}},
  {"r14", ByteArray{std::byte{0x41}, std::byte{0x56}}},
  {"r15", ByteArray{std::byte{0x41}, std::byte{0x57}}},
};

static inline std::unordered_map<std::string_view, ByteArray> PopMachineCode {
  {"rax", ByteArray{std::byte{0x58}}},
  {"rcx", ByteArray{std::byte{0x59}}},
  {"rdx", ByteArray{std::byte{0x5a}}},
  {"rbx", ByteArray{std::byte{0x5b}}},
  {"rsp", ByteArray{std::byte{0x5c}}},
  {"rbp", ByteArray{std::byte{0x5d}}},
  {"rsi", ByteArray{std::byte{0x5e}}},
  {"rdi", ByteArray{std::byte{0x5f}}},
  {"r8",  ByteArray{std::byte{0x41}, std::byte{0x58}}},
  {"r9",  ByteArray{std::byte{0x41}, std::byte{0x59}}},
  {"r10", ByteArray{std::byte{0x41}, std::byte{0x5a}}},
  {"r11", ByteArray{std::byte{0x41}, std::byte{0x5b}}},
  {"r12", ByteArray{std::byte{0x41}, std::byte{0x5c}}},
  {"r13", ByteArray{std::byte{0x41}, std::byte{0x5d}}},
  {"r14", ByteArray{std::byte{0x41}, std::byte{0x5e}}},
  {"r15", ByteArray{std::byte{0x41}, std::byte{0x5f}}},
};

static inline std::unordered_map<std::string_view, ByteArray> IncMachineCode {
  {"rax", ByteArray{std::byte{0x48}, std::byte{0xff}, std::byte{0xc0}}},
  {"rcx", ByteArray{std::byte{0x48}, std::byte{0xff}, std::byte{0xc1}}},
  {"rdx", ByteArray{std::byte{0x48}, std::byte{0xff}, std::byte{0xc2}}},
  {"rbx", ByteArray{std::byte{0x48}, std::byte{0xff}, std::byte{0xc3}}},
  {"rsp", ByteArray{std::byte{0x48}, std::byte{0xff}, std::byte{0xc4}}},
  {"rbp", ByteArray{std::byte{0x48}, std::byte{0xff}, std::byte{0xc5}}},
  {"rsi", ByteArray{std::byte{0x48}, std::byte{0xff}, std::byte{0xc6}}},
  {"rdi", ByteArray{std::byte{0x48}, std::byte{0xff}, std::byte{0xc7}}},
  {"r8",  ByteArray{std::byte{0x49}, std::byte{0xff}, std::byte{0xc0}}},
  {"r9",  ByteArray{std::byte{0x49}, std::byte{0xff}, std::byte{0xc1}}},
  {"r10", ByteArray{std::byte{0x49}, std::byte{0xff}, std::byte{0xc2}}},
  {"r11", ByteArray{std::byte{0x49}, std::byte{0xff}, std::byte{0xc3}}},
  {"r12", ByteArray{std::byte{0x49}, std::byte{0xff}, std::byte{0xc4}}},
  {"r13", ByteArray{std::byte{0x49}, std::byte{0xff}, std::byte{0xc5}}},
  {"r14", ByteArray{std::byte{0x49}, std::byte{0xff}, std::byte{0xc6}}},
  {"r15", ByteArray{std::byte{0x49}, std::byte{0xff}, std::byte{0xc7}}},
};

static inline std::unordered_map<std::string_view, ByteArray> DecMachineCode {
  {"rax", ByteArray{std::byte{0x48}, std::byte{0xff}, std::byte{0xc8}}},
  {"rcx", ByteArray{std::byte{0x48}, std::byte{0xff}, std::byte{0xc9}}},
  {"rdx", ByteArray{std::byte{0x48}, std::byte{0xff}, std::byte{0xca}}},
  {"rbx", ByteArray{std::byte{0x48}, std::byte{0xff}, std::byte{0xcb}}},
  {"rsp", ByteArray{std::byte{0x48}, std::byte{0xff}, std::byte{0xcc}}},
  {"rbp", ByteArray{std::byte{0x48}, std::byte{0xff}, std::byte{0xcd}}},
  {"rsi", ByteArray{std::byte{0x48}, std::byte{0xff}, std::byte{0xce}}},
  {"rdi", ByteArray{std::byte{0x48}, std::byte{0xff}, std::byte{0xcf}}},
  {"r8",  ByteArray{std::byte{0x49}, std::byte{0xff}, std::byte{0xc8}}},
  {"r9",  ByteArray{std::byte{0x49}, std::byte{0xff}, std::byte{0xc9}}},
  {"r10", ByteArray{std::byte{0x49}, std::byte{0xff}, std::byte{0xca}}},
  {"r11", ByteArray{std::byte{0x49}, std::byte{0xff}, std::byte{0xcb}}},
  {"r12", ByteArray{std::byte{0x49}, std::byte{0xff}, std::byte{0xcc}}},
  {"r13", ByteArray{std::byte{0x49}, std::byte{0xff}, std::byte{0xcd}}},
  {"r14", ByteArray{std::byte{0x49}, std::byte{0xff}, std::byte{0xce}}},
  {"r15", ByteArray{std::byte{0x49}, std::byte{0xff}, std::byte{0xcf}}},
};

static inline std::unordered_map<std::string_view, ByteArray> CmpMachineCode {
  {"rax", ByteArray{std::byte{0x48}, std::byte{0x3d}}},
  {"rcx", ByteArray{std::byte{0x48}, std::byte{0x81}, std::byte{0xf9}}},
  {"rdx", ByteArray{std::byte{0x48}, std::byte{0x81}, std::byte{0xfa}}},
  {"rbx", ByteArray{std::byte{0x48}, std::byte{0x81}, std::byte{0xfb}}},
  {"rsp", ByteArray{std::byte{0x48}, std::byte{0x81}, std::byte{0xfc}}},
  {"rbp", ByteArray{std::byte{0x48}, std::byte{0x81}, std::byte{0xfd}}},
  {"rsi", ByteArray{std::byte{0x48}, std::byte{0x81}, std::byte{0xfe}}},
  {"rdi", ByteArray{std::byte{0x48}, std::byte{0x81}, std::byte{0xff}}},
  {"r8",  ByteArray{std::byte{0x49}, std::byte{0x81}, std::byte{0xf8}}},
  {"r9",  ByteArray{std::byte{0x49}, std::byte{0x81}, std::byte{0xf9}}},
  {"r10", ByteArray{std::byte{0x49}, std::byte{0x81}, std::byte{0xfa}}},
  {"r11", ByteArray{std::byte{0x49}, std::byte{0x81}, std::byte{0xfb}}},
  {"r12", ByteArray{std::byte{0x49}, std::byte{0x81}, std::byte{0xfc}}},
  {"r13", ByteArray{std::byte{0x49}, std::byte{0x81}, std::byte{0xfd}}},
  {"r14", ByteArray{std::byte{0x49}, std::byte{0x81}, std::byte{0xfe}}},
  {"r15", ByteArray{std::byte{0x49}, std::byte{0x81}, std::byte{0xff}}},
};

static inline std::unordered_map<std::string_view, ByteArray> CmpBytePtrMachineCode {
  {"rax", ByteArray{std::byte{0x80}, std::byte{0x38}}},
  {"rcx", ByteArray{std::byte{0x80}, std::byte{0x39}}},
  {"rdx", ByteArray{std::byte{0x80}, std::byte{0x3a}}},
  {"rbx", ByteArray{std::byte{0x80}, std::byte{0x3b}}},
  {"rsp", ByteArray{std::byte{0x80}, std::byte{0x3c}}},
  {"rbp", ByteArray{std::byte{0x80}, std::byte{0x7d}}},
  {"rsi", ByteArray{std::byte{0x80}, std::byte{0x3e}}},
  {"rdi", ByteArray{std::byte{0x80}, std::byte{0x3f}}},
  {"r8",  ByteArray{std::byte{0x41}, std::byte{0x80}, std::byte{0x38}}},
  {"r9",  ByteArray{std::byte{0x41}, std::byte{0x80}, std::byte{0x39}}},
  {"r10", ByteArray{std::byte{0x41}, std::byte{0x80}, std::byte{0x3a}}},
  {"r11", ByteArray{std::byte{0x41}, std::byte{0x80}, std::byte{0x3b}}},
  {"r12", ByteArray{std::byte{0x41}, std::byte{0x80}, std::byte{0x3c}}},
  {"r13", ByteArray{std::byte{0x41}, std::byte{0x80}, std::byte{0x7d}}},
  {"r14", ByteArray{std::byte{0x41}, std::byte{0x80}, std::byte{0x3e}}},
  {"r15", ByteArray{std::byte{0x41}, std::byte{0x80}, std::byte{0x3f}}},
};

static inline std::unordered_map<std::string_view, ByteArray> AddMachineCode {
  {"rax", ByteArray{std::byte{0x48}, std::byte{0x05}}},
  {"rcx", ByteArray{std::byte{0x48}, std::byte{0x81}, std::byte{0xc1}}},
  {"rdx", ByteArray{std::byte{0x48}, std::byte{0x81}, std::byte{0xc2}}},
  {"rbx", ByteArray{std::byte{0x48}, std::byte{0x81}, std::byte{0xc3}}},
  {"rsp", ByteArray{std::byte{0x48}, std::byte{0x81}, std::byte{0xc4}}},
  {"rbp", ByteArray{std::byte{0x48}, std::byte{0x81}, std::byte{0xc5}}},
  {"rsi", ByteArray{std::byte{0x48}, std::byte{0x81}, std::byte{0xc6}}},
  {"rdi", ByteArray{std::byte{0x48}, std::byte{0x81}, std::byte{0xc7}}},
  {"r8",  ByteArray{std::byte{0x49}, std::byte{0x81}, std::byte{0xc0}}},
  {"r9",  ByteArray{std::byte{0x49}, std::byte{0x81}, std::byte{0xc1}}},
  {"r10", ByteArray{std::byte{0x49}, std::byte{0x81}, std::byte{0xc2}}},
  {"r11", ByteArray{std::byte{0x49}, std::byte{0x81}, std::byte{0xc3}}},
  {"r12", ByteArray{std::byte{0x49}, std::byte{0x81}, std::byte{0xc4}}},
  {"r13", ByteArray{std::byte{0x49}, std::byte{0x81}, std::byte{0xc5}}},
  {"r14", ByteArray{std::byte{0x49}, std::byte{0x81}, std::byte{0xc6}}},
  {"r15", ByteArray{std::byte{0x49}, std::byte{0x81}, std::byte{0xc7}}},
  {"edx", ByteArray{std::byte{0x81}, std::byte{0xc2}}},
};

static inline std::unordered_map<std::string_view, ByteArray> SubMachineCode {
  {"rax", ByteArray{std::byte{0x48}, std::byte{0x2d}}},
  {"rcx", ByteArray{std::byte{0x48}, std::byte{0x81}, std::byte{0xe9}}},
  {"rdx", ByteArray{std::byte{0x48}, std::byte{0x81}, std::byte{0xea}}},
  {"rbx", ByteArray{std::byte{0x48}, std::byte{0x81}, std::byte{0xeb}}},
  {"rsp", ByteArray{std::byte{0x48}, std::byte{0x81}, std::byte{0xec}}},
  {"rbp", ByteArray{std::byte{0x48}, std::byte{0x81}, std::byte{0xed}}},
  {"rsi", ByteArray{std::byte{0x48}, std::byte{0x81}, std::byte{0xee}}},
  {"rdi", ByteArray{std::byte{0x48}, std::byte{0x81}, std::byte{0xef}}},
  {"r8",  ByteArray{std::byte{0x49}, std::byte{0x81}, std::byte{0xe8}}},
  {"r9",  ByteArray{std::byte{0x49}, std::byte{0x81}, std::byte{0xe9}}},
  {"r10", ByteArray{std::byte{0x49}, std::byte{0x81}, std::byte{0xea}}},
  {"r11", ByteArray{std::byte{0x49}, std::byte{0x81}, std::byte{0xeb}}},
  {"r12", ByteArray{std::byte{0x49}, std::byte{0x81}, std::byte{0xec}}},
  {"r13", ByteArray{std::byte{0x49}, std::byte{0x81}, std::byte{0xed}}},
  {"r14", ByteArray{std::byte{0x49}, std::byte{0x81}, std::byte{0xee}}},
  {"r15", ByteArray{std::byte{0x49}, std::byte{0x81}, std::byte{0xef}}},
};

static inline std::unordered_map<std::string_view, ByteArray> MulMachineCode {
  {"rax", ByteArray{std::byte{0x48}, std::byte{0x69}, std::byte{0xc0}}},
  {"rcx", ByteArray{std::byte{0x48}, std::byte{0x69}, std::byte{0xc9}}},
  {"rdx", ByteArray{std::byte{0x48}, std::byte{0x69}, std::byte{0xd2}}},
  {"rbx", ByteArray{std::byte{0x48}, std::byte{0x69}, std::byte{0xdb}}},
  {"rsp", ByteArray{std::byte{0x48}, std::byte{0x69}, std::byte{0xe4}}},
  {"rbp", ByteArray{std::byte{0x48}, std::byte{0x69}, std::byte{0xed}}},
  {"rsi", ByteArray{std::byte{0x48}, std::byte{0x69}, std::byte{0xf6}}},
  {"rdi", ByteArray{std::byte{0x48}, std::byte{0x69}, std::byte{0xff}}},
  {"r8",  ByteArray{std::byte{0x4d}, std::byte{0x69}, std::byte{0xc0}}},
  {"r9",  ByteArray{std::byte{0x4d}, std::byte{0x69}, std::byte{0xc9}}},
  {"r10", ByteArray{std::byte{0x4d}, std::byte{0x69}, std::byte{0xd2}}},
  {"r11", ByteArray{std::byte{0x4d}, std::byte{0x69}, std::byte{0xdb}}},
  {"r12", ByteArray{std::byte{0x4d}, std::byte{0x69}, std::byte{0xe4}}},
  {"r13", ByteArray{std::byte{0x4d}, std::byte{0x69}, std::byte{0xed}}},
  {"r14", ByteArray{std::byte{0x4d}, std::byte{0x69}, std::byte{0xf6}}},
  {"r15", ByteArray{std::byte{0x4d}, std::byte{0x69}, std::byte{0xff}}},
};

static inline std::unordered_map<std::string_view, ByteArray> DivMachineCode {
  {"rax", ByteArray{std::byte{0x48}, std::byte{0xf7}, std::byte{0xf0}}},
  {"rcx", ByteArray{std::byte{0x48}, std::byte{0xf7}, std::byte{0xf1}}},
  {"rdx", ByteArray{std::byte{0x48}, std::byte{0xf7}, std::byte{0xf2}}},
  {"rbx", ByteArray{std::byte{0x48}, std::byte{0xf7}, std::byte{0xf3}}},
  {"rsp", ByteArray{std::byte{0x48}, std::byte{0xf7}, std::byte{0xf4}}},
  {"rbp", ByteArray{std::byte{0x48}, std::byte{0xf7}, std::byte{0xf5}}},
  {"rsi", ByteArray{std::byte{0x48}, std::byte{0xf7}, std::byte{0xf6}}},
  {"rdi", ByteArray{std::byte{0x48}, std::byte{0xf7}, std::byte{0xf7}}},
  {"r8",  ByteArray{std::byte{0x49}, std::byte{0xf7}, std::byte{0xf0}}},
  {"r9",  ByteArray{std::byte{0x49}, std::byte{0xf7}, std::byte{0xf1}}},
  {"r10", ByteArray{std::byte{0x49}, std::byte{0xf7}, std::byte{0xf2}}},
  {"r11", ByteArray{std::byte{0x49}, std::byte{0xf7}, std::byte{0xf3}}},
  {"r12", ByteArray{std::byte{0x49}, std::byte{0xf7}, std::byte{0xf4}}},
  {"r13", ByteArray{std::byte{0x49}, std::byte{0xf7}, std::byte{0xf5}}},
  {"r14", ByteArray{std::byte{0x49}, std::byte{0xf7}, std::byte{0xf6}}},
  {"r15", ByteArray{std::byte{0x49}, std::byte{0xf7}, std::byte{0xf7}}},
};

static inline std::unordered_map<std::string_view, ByteArray> XorMachineCode {
  {"rax", ByteArray{std::byte{0x48}, std::byte{0x31}, std::byte{0xc0}}},
  {"rcx", ByteArray{std::byte{0x48}, std::byte{0x31}, std::byte{0xc9}}},
  {"rdx", ByteArray{std::byte{0x48}, std::byte{0x31}, std::byte{0xd2}}},
  {"rbx", ByteArray{std::byte{0x48}, std::byte{0x31}, std::byte{0xdb}}},
  {"rsp", ByteArray{std::byte{0x48}, std::byte{0x31}, std::byte{0xe4}}},
  {"rbp", ByteArray{std::byte{0x48}, std::byte{0x31}, std::byte{0xed}}},
  {"rsi", ByteArray{std::byte{0x48}, std::byte{0x31}, std::byte{0xf6}}},
  {"rdi", ByteArray{std::byte{0x48}, std::byte{0x31}, std::byte{0xff}}},
  {"r8",  ByteArray{std::byte{0x4d}, std::byte{0x31}, std::byte{0xc0}}},
  {"r9",  ByteArray{std::byte{0x4d}, std::byte{0x31}, std::byte{0xc9}}},
  {"r10", ByteArray{std::byte{0x4d}, std::byte{0x31}, std::byte{0xd2}}},
  {"r11", ByteArray{std::byte{0x4d}, std::byte{0x31}, std::byte{0xdb}}},
  {"r12", ByteArray{std::byte{0x4d}, std::byte{0x31}, std::byte{0xe4}}},
  {"r13", ByteArray{std::byte{0x4d}, std::byte{0x31}, std::byte{0xed}}},
  {"r14", ByteArray{std::byte{0x4d}, std::byte{0x31}, std::byte{0xf6}}},
  {"r15", ByteArray{std::byte{0x4d}, std::byte{0x31}, std::byte{0xff}}},
  {"edx", ByteArray{std::byte{0x31}, std::byte{0xd2}}},
};

static inline std::unordered_map<std::string_view, ByteArray> TestMachineCode {
  {"rax", ByteArray{std::byte{0x48}, std::byte{0x85}, std::byte{0xc0}}},
  {"rcx", ByteArray{std::byte{0x48}, std::byte{0x85}, std::byte{0xc9}}},
  {"rdx", ByteArray{std::byte{0x48}, std::byte{0x85}, std::byte{0xd2}}},
  {"rbx", ByteArray{std::byte{0x48}, std::byte{0x85}, std::byte{0xdb}}},
  {"rsp", ByteArray{std::byte{0x48}, std::byte{0x85}, std::byte{0xe4}}},
  {"rbp", ByteArray{std::byte{0x48}, std::byte{0x85}, std::byte{0xed}}},
  {"rsi", ByteArray{std::byte{0x48}, std::byte{0x85}, std::byte{0xf6}}},
  {"rdi", ByteArray{std::byte{0x48}, std::byte{0x85}, std::byte{0xff}}},
  {"r8",  ByteArray{std::byte{0x4d}, std::byte{0x85}, std::byte{0xc0}}},
  {"r9",  ByteArray{std::byte{0x4d}, std::byte{0x85}, std::byte{0xc9}}},
  {"r10", ByteArray{std::byte{0x4d}, std::byte{0x85}, std::byte{0xd2}}},
  {"r11", ByteArray{std::byte{0x4d}, std::byte{0x85}, std::byte{0xdb}}},
  {"r12", ByteArray{std::byte{0x4d}, std::byte{0x85}, std::byte{0xe4}}},
  {"r13", ByteArray{std::byte{0x4d}, std::byte{0x85}, std::byte{0xed}}},
  {"r14", ByteArray{std::byte{0x4d}, std::byte{0x85}, std::byte{0xf6}}},
  {"r15", ByteArray{std::byte{0x4d}, std::byte{0x85}, std::byte{0xff}}},
};

constexpr std::array<std::string_view, 16> kRegisters {
  "rax", "rcx", "rdx", "rbx", "rsp", "rbp", "rsi", "rdi",
  "r8",  "r9",  "r10", "r11", "r12", "r13", "r14", "r15",
};

constexpr auto kHalfRegisters{kRegisters.size() / 2};

struct Register {
  int source{-1};
  int destination{-1};
};

constexpr Register assignRegisters(std::string_view source, std::string_view destination) {
  Register assigned{};

  for (auto i = 0; i < kRegisters.size(); i++) {
    if (kRegisters[i] == source) assigned.source = i;
    if (kRegisters[i] == destination) assigned.destination = i;
    if (assigned.source > -1 && assigned.destination > -1) break;
  }

  return assigned;
}

void CodeGenerator::generateCode(const std::vector<CodeGenerator::InstructionValue> &instructions) {
  for (const auto &instruction : instructions) {
    switch (instruction->getOperation()) {
      case Operation::LEA:
        emitLea(instruction);
        break;
      case Operation::MOV:
        emitMove(instruction);
        break;
      case Operation::MOV_MEMORY:
        emitMoveMemory(instruction);
        break;
      case Operation::SYSCALL:
        emitSysCall(instruction);
        break;
      case Operation::PUSH:
        emitPush(instruction);
        break;
      case Operation::POP:
        emitPop(instruction);
        break;
      case Operation::CALL:
        emitCall(instruction);
        break;
      case Operation::LABEL:
        emitLabel(instruction);
        break;
      case Operation::CMP:
        emitCmp(instruction);
        break;
      case Operation::CMP_BYTE_PTR:
        emitCmpBytePtr(instruction);
        break;
      case Operation::JMP:
      case Operation::JE:
      case Operation::JZ:
      case Operation::JNE:
      case Operation::JNZ:
        emitJmp(instruction);
        break;
      case Operation::INC:
        emitInc(instruction);
        break;
      case Operation::DEC:
        emitDec(instruction);
        break;
      case Operation::IADD:
        emitAdd(instruction);
        break;
      case Operation::ISUB:
        emitSub(instruction);
        break;
      case Operation::IMUL:
        emitMul(instruction);
        break;
      case Operation::IDIV:
        emitDiv(instruction);
        break;
      case Operation::XOR:
        emitXor(instruction);
        break;
      case Operation::OR:
        emitOr(instruction);
        break;
      case Operation::TEST:
        emitTest(instruction);
        break;
      case Operation::RET:
        emitRet(instruction);
        break;
      case Operation::NOP:
        emitNop(instruction);
        break;
      default:
        throw CodeGenerationError{"Unknown operation to generate the code for"};
    }
  }

  // Patch data
  for (const auto &data : m_data) {
    const auto start{data.m_start};
    const auto offset{data.m_offset};

    const uint32_t newAddress{
        static_cast<uint32_t>(kVirtualStartAddress + offset + m_textSection.size() + kTextOffset)};
    const ByteArray bytes{newAddress};

    // Overwrite the instruction
    for (size_t i = 0; i < bytes.size(); i++) {
      m_textSection.insert(i + start, bytes.data()[i]);
    }
  }

  // Patch jumps
  for (const auto &jump : m_jumps) {
    const auto label = std::find_if(m_labels.begin(), m_labels.end(),
                                    [&](const auto &label) { return label.m_name == jump.m_name; });
    assert(label != m_labels.end() && "No such label to jump to");
    const auto offset{label->m_offset};
    const auto diff{jump.m_offset - offset};

    // Overwrite the instruction
    m_textSection.insert(jump.m_offset, std::byte(0xff - diff));
  }

  // Patch calls
  for (const auto &call : m_calls) {
    const auto label = std::find_if(m_labels.begin(), m_labels.end(),
                                    [&](const auto &label) { return label.m_name == call.m_name; });
    assert(label != m_labels.end() && "No such label to call");
    const auto offset{label->m_offset};

    // The offset of the position is a byte
    const uint32_t diff{static_cast<uint32_t>(call.m_offset - offset + 4)};
    const uint32_t x{0xffffffff - static_cast<uint32_t>(diff - 1)};
    const ByteArray bytes{x};

    // Overwrite the instruction
    for (size_t i = 0; i < bytes.size(); i++) {
      m_textSection.insert(i + call.m_offset, bytes.data()[i]);
    }
  }
}

void CodeGenerator::emitLea(const CodeGenerator::InstructionValue &instruction) {
  const auto &target = instruction->getTarget();
  const auto &argOne = instruction->getArg1();

  // lea reg, [rsp + number]
  if (target->getType() == TType::REGISTER && argOne->getType() == TType::LIT_INT) {
    m_textSection.putBytes(LeaMachineCode[target->getValue<std::string>()]);
    m_textSection.putU32(argOne->getValue<int>());
    return;
  }

  throw CodeGenerationError{"Unknown lea operation"};
}

void CodeGenerator::emitMove(const CodeGenerator::InstructionValue &instruction, bool label) {
  const auto &target = instruction->getTarget();
  const auto &argOne = instruction->getArg1();

  // mov reg, number
  if (target->getType() == TType::REGISTER && argOne->getType() == TType::LIT_INT) {
    m_textSection.putBytes(MovMachineCode[target->getValue<std::string>()]);
    if (label) {
      m_data.emplace_back(Data{m_textSection.size(), static_cast<size_t>(argOne->getValue<int>())});
    }
    m_textSection.putU32(argOne->getValue<int>());
    return;
  }

  // mov reg, bool
  if (target->getType() == TType::REGISTER && (argOne->getType() == TType::KW_TRUE || argOne->getType() == TType::KW_FALSE)) {
    m_textSection.putBytes(MovMachineCode[target->getValue<std::string>()]);
    m_textSection.putU32(argOne->getValue<bool>() ? 1 : 0);
    return;
  }

  // mov reg, string
  if (target->getType() == TType::REGISTER && argOne->getType() == TType::LIT_STR) {
    auto strVal = argOne->getValue<std::string>();
    for (const auto ch : strVal) {
      m_dataSection.putBytes(std::byte(ch));
    }
    argOne->setType(TType::LIT_INT);
    argOne->setValue(std::abs(static_cast<int>(m_dataSection.size() - strVal.size())));
    emitMove(instruction, true);
    return;
  }

  // mov reg1, reg2
  if (target->getType() == TType::REGISTER && argOne->getType() == TType::REGISTER) {
    const auto [src, dst] = assignRegisters(argOne->getValue<std::string>(), target->getValue<std::string>());
    assert((src > -1 && dst > -1) && "Failed to look up registers for mov operation");

    // <-        rax       ...       r15
    // rax   <byte_0000>   ...   <byte_0015>
    // ...       ...       ...       ...
    // r15   <byte_1500>   ...   <byte_1515>
    if (dst < kHalfRegisters && src < kHalfRegisters) {
      // top left
      m_textSection.putBytes(std::byte{0x48}, std::byte{0x89});
    } else if (dst < kHalfRegisters && src >= kHalfRegisters) {
      // top right
      m_textSection.putBytes(std::byte{0x4c}, std::byte{0x89});
    } else if (dst >= kHalfRegisters && src < kHalfRegisters) {
      // bottom left
      m_textSection.putBytes(std::byte{0x49}, std::byte{0x89});
    } else if (dst >= kHalfRegisters && src >= kHalfRegisters) {
      // bottom right
      m_textSection.putBytes(std::byte{0x4d}, std::byte{0x89});
    } else {
      assert(0 && "Unknown table entry for mov operation");
    }

    auto result = 0xc0 + (8 * (src % kHalfRegisters)) + (dst % kHalfRegisters);
    assert(result < 255 && "Result value is out of range");
    m_textSection.putBytes(std::byte(result));
    return;
  }

  throw CodeGenerationError{"Unknown move operation"};
}

void CodeGenerator::emitMoveMemory(const CodeGenerator::InstructionValue &instruction) {
  const auto &target = instruction->getTarget();
  const auto &argOne = instruction->getArg1();

  // mov [rsi], dl
  if (target->getType() == TType::REGISTER && argOne->getType() == TType::REGISTER &&
      target->getValue<std::string>() == "rsi" && argOne->getValue<std::string>() == "dl") {
    m_textSection.putBytes(std::byte{0x88}, std::byte{0x16});
    return;
  }

  throw CodeGenerationError{"Unknown move memory operation"};
}

void CodeGenerator::emitSysCall(const CodeGenerator::InstructionValue &instruction) {
  m_textSection.putBytes(std::byte{0x0f}, std::byte{0x05});
}

void CodeGenerator::emitPush(const CodeGenerator::InstructionValue &instruction) {
  // push reg
  if (instruction->getArg1()->getType() == TType::REGISTER) {
    m_textSection.putBytes(PushMachineCode[instruction->getArg1()->getValue<std::string>()]);
    return;
  }

  throw CodeGenerationError{"Unknown push operation"};
}

void CodeGenerator::emitPop(const CodeGenerator::InstructionValue &instruction) {
  // pop reg
  if (instruction->getArg1()->getType() == TType::REGISTER) {
    m_textSection.putBytes(PopMachineCode[instruction->getArg1()->getValue<std::string>()]);
    return;
  }

  throw CodeGenerationError{"Unknown pop operation"};
}

void CodeGenerator::emitCall(const CodeGenerator::InstructionValue &instruction) {
  m_textSection.putBytes(std::byte{0xe8});

  // call label
  const auto &label{instruction->getTarget()->getValue<std::string>()};
  const auto offset{m_textSection.size()};
  m_calls.emplace_back(Label{label, offset});

  m_textSection.putU32(0);
}

void CodeGenerator::emitLabel(const CodeGenerator::InstructionValue &instruction) {
  const auto &label = instruction->getArg1()->getValue<std::string>();
  const auto offset = m_textSection.size();
  m_labels.emplace_back(Label{label, offset});
}

void CodeGenerator::emitCmp(const CodeGenerator::InstructionValue &instruction) {
  const auto &argOne = instruction->getArg1();
  const auto &argTwo = instruction->getArg2();

  // cmp reg, number
  if (instruction->getArg1()->getType() == TType::REGISTER) {
    m_textSection.putBytes(CmpMachineCode[instruction->getArg1()->getValue<std::string>()]);
    m_textSection.putU32(argTwo->getValue<int>());
    return;
  }

  throw CodeGenerationError{"Unknown cmp operation"};
}

void CodeGenerator::emitCmpBytePtr(const CodeGenerator::InstructionValue &instruction) {
  const auto &argOne = instruction->getArg1();
  const auto &argTwo = instruction->getArg2();

  // cmp byte ptr [reg], number
  if (instruction->getArg1()->getType() == TType::REGISTER) {
    m_textSection.putBytes(CmpBytePtrMachineCode[instruction->getArg1()->getValue<std::string>()]);
    m_textSection.putBytes(std::byte(argTwo->getValue<int>()));
    return;
  }

  throw CodeGenerationError{"Unknown cmp byte ptr operation"};
}

void CodeGenerator::emitJmp(const CodeGenerator::InstructionValue &instruction) {
  const auto getOperandByte = [&]() -> std::byte {
    switch (instruction->getOperation()) {
      case Operation::JMP:
        return std::byte{0xeb};
      case Operation::JE:
      case Operation::JZ:
        return std::byte{0x74};
      case Operation::JNE:
      case Operation::JNZ:
        return std::byte{0x75};
      default: {
        assert(0 && "Unknown jump operation");
        return std::byte{0x00};
      }
    }
  };

  m_textSection.putBytes(getOperandByte());

  const auto &label = instruction->getArg1()->getValue<std::string>();
  const auto offset = m_textSection.size();
  m_jumps.emplace_back(Label{label, offset});

  // Empty displacement
  m_textSection.putBytes(std::byte{0x00});
}

void CodeGenerator::emitInc(const CodeGenerator::InstructionValue &instruction) {
  // inc reg
  if (instruction->getArg1()->getType() == TType::REGISTER) {
    m_textSection.putBytes(IncMachineCode[instruction->getArg1()->getValue<std::string>()]);
    return;
  }

  throw CodeGenerationError{"Unknown inc operation"};
}

void CodeGenerator::emitDec(const CodeGenerator::InstructionValue &instruction) {
  // dec reg
  if (instruction->getArg1()->getType() == TType::REGISTER) {
    m_textSection.putBytes(DecMachineCode[instruction->getArg1()->getValue<std::string>()]);
    return;
  }

  throw CodeGenerationError{"Unknown dec operation"};
}

void CodeGenerator::emitAdd(const CodeGenerator::InstructionValue &instruction) {
  const auto &target = instruction->getTarget();
  const auto &argOne = instruction->getArg1();

  // add reg, number
  if (target->getType() == TType::REGISTER && argOne->getType() == TType::LIT_INT) {
    m_textSection.putBytes(AddMachineCode[target->getValue<std::string>()]);
    m_textSection.putU32(argOne->getValue<int>());
    return;
  }

  // add reg1, reg2
  if (target->getType() == TType::REGISTER && argOne->getType() == TType::REGISTER) {
    const auto [src, dst] = assignRegisters(argOne->getValue<std::string>(), target->getValue<std::string>());
    assert((src > -1 && dst > -1) && "Failed to look up registers for add operation");

    //  +        rax       ...       r15
    // rax   <byte_0000>   ...   <byte_0015>
    // ...       ...       ...       ...
    // r15   <byte_1500>   ...   <byte_1515>
    if (dst < kHalfRegisters && src < kHalfRegisters) {
      // top left
      m_textSection.putBytes(std::byte{0x48}, std::byte{0x01});
    } else if (dst < kHalfRegisters && src >= kHalfRegisters) {
      // top right
      m_textSection.putBytes(std::byte{0x4c}, std::byte{0x01});
    } else if (dst >= kHalfRegisters && src < kHalfRegisters) {
      // bottom left
      m_textSection.putBytes(std::byte{0x49}, std::byte{0x01});
    } else if (dst >= kHalfRegisters && src >= kHalfRegisters) {
      // bottom right
      m_textSection.putBytes(std::byte{0x4d}, std::byte{0x01});
    } else {
      assert(0 && "Unknown table entry for add operation");
    }

    auto result = 0xc0 + (8 * (src % kHalfRegisters)) + (dst % kHalfRegisters);
    assert(result < 255 && "Result value is out of range");
    m_textSection.putBytes(std::byte(result));
    return;
  }

  throw CodeGenerationError{"Unknown add operation"};
}

void CodeGenerator::emitSub(const CodeGenerator::InstructionValue &instruction) {
  const auto &target = instruction->getTarget();
  const auto &argOne = instruction->getArg1();

  // sub reg, number
  if (target->getType() == TType::REGISTER && argOne->getType() == TType::LIT_INT) {
    m_textSection.putBytes(SubMachineCode[target->getValue<std::string>()]);
    m_textSection.putU32(argOne->getValue<int>());
    return;
  }

  // sub edx, esi
  if (target->getType() == TType::REGISTER && argOne->getType() == TType::REGISTER &&
      target->getValue<std::string>() == "edx" && argOne->getValue<std::string>() == "esi") {
    m_textSection.putBytes(std::byte{0x29}, std::byte{0xf2});
    return;
  }

  // sub reg1, reg2
  if (target->getType() == TType::REGISTER && argOne->getType() == TType::REGISTER) {
    const auto [src, dst] = assignRegisters(argOne->getValue<std::string>(), target->getValue<std::string>());
    assert((src > -1 && dst > -1) && "Failed to look up registers for sub operation");

    //  -        rax       ...       r15
    // rax   <byte_0000>   ...   <byte_0015>
    // ...       ...       ...       ...
    // r15   <byte_1500>   ...   <byte_1515>
    if (dst < kHalfRegisters && src < kHalfRegisters) {
      // top left
      m_textSection.putBytes(std::byte{0x48}, std::byte{0x29});
    } else if (dst < kHalfRegisters && src >= kHalfRegisters) {
      // top right
      m_textSection.putBytes(std::byte{0x4c}, std::byte{0x29});
    } else if (dst >= kHalfRegisters && src < kHalfRegisters) {
      // bottom left
      m_textSection.putBytes(std::byte{0x49}, std::byte{0x29});
    } else if (dst >= kHalfRegisters && src >= kHalfRegisters) {
      // bottom right
      m_textSection.putBytes(std::byte{0x4d}, std::byte{0x29});
    } else {
      assert(0 && "Unknown table entry for sub operation");
    }

    auto result = 0xc0 + (8 * (src % kHalfRegisters)) + (dst % kHalfRegisters);
    assert(result < 255 && "Result value is out of range");
    m_textSection.putBytes(std::byte(result));
    return;
  }

  throw CodeGenerationError{"Unknown sub operation"};
}

void CodeGenerator::emitMul(const CodeGenerator::InstructionValue &instruction) {
  const auto &target = instruction->getTarget();
  const auto &argOne = instruction->getArg1();

  // imul reg, number
  if (target->getType() == TType::REGISTER && argOne->getType() == TType::LIT_INT) {
    m_textSection.putBytes(MulMachineCode[target->getValue<std::string>()]);
    m_textSection.putU32(argOne->getValue<int>());
    return;
  }

  // imul reg1, reg2
  if (target->getType() == TType::REGISTER && argOne->getType() == TType::REGISTER) {
    const auto [src, dst] = assignRegisters(argOne->getValue<std::string>(), target->getValue<std::string>());
    assert((src > -1 && dst > -1) && "Failed to look up registers for mul operation");

    //  *        rax       ...       r15
    // rax   <byte_0000>   ...   <byte_0015>
    // ...       ...       ...       ...
    // r15   <byte_1500>   ...   <byte_1515>
    if (dst < kHalfRegisters && src < kHalfRegisters) {
      // top left
      m_textSection.putBytes(std::byte{0x48}, std::byte{0x0f}, std::byte{0xaf});
    } else if (dst < kHalfRegisters && src >= kHalfRegisters) {
      // top right
      m_textSection.putBytes(std::byte{0x49}, std::byte{0x0f}, std::byte{0xaf});
    } else if (dst >= kHalfRegisters && src < kHalfRegisters) {
      // bottom left
      m_textSection.putBytes(std::byte{0x4c}, std::byte{0x0f}, std::byte{0xaf});
    } else if (dst >= kHalfRegisters && src >= kHalfRegisters) {
      // bottom right
      m_textSection.putBytes(std::byte{0x4d}, std::byte{0x0f}, std::byte{0xaf});
    } else {
      assert(0 && "Unknown table entry for mul operation");
    }

    auto result = 0xc0 + (src % kHalfRegisters) + (8 * (dst % kHalfRegisters));
    assert(result < 255 && "Result value is out of range");
    m_textSection.putBytes(std::byte(result));
    return;
  }

  throw CodeGenerationError{"Unknown mul operation"};
}

void CodeGenerator::emitDiv(const CodeGenerator::InstructionValue &instruction) {
  // div reg
  if (instruction->getArg1()->getType() == TType::REGISTER) {
    m_textSection.putBytes(DivMachineCode[instruction->getArg1()->getValue<std::string>()]);
    return;
  }

  throw CodeGenerationError{"Unknown div operation"};
}

void CodeGenerator::emitXor(const CodeGenerator::InstructionValue &instruction) {
  const auto &argOne = instruction->getArg1();
  const auto &argTwo = instruction->getArg2();

  // xor reg, reg
  if (argOne->getType() == TType::REGISTER && argTwo->getType() == TType::REGISTER &&
      argOne->getValue<std::string>() == argTwo->getValue<std::string>()) {
    m_textSection.putBytes(XorMachineCode[argOne->getValue<std::string>()]);
    return;
  }

  throw CodeGenerationError{"Unknown xor operation"};
}

void CodeGenerator::emitOr(const CodeGenerator::InstructionValue &instruction) {
  const auto &argOne = instruction->getArg1();
  const auto &argTwo = instruction->getArg2();

  // or dl, 0x30
  if (argOne->getType() == TType::REGISTER && argTwo->getType() == TType::LIT_INT &&
      argOne->getValue<std::string>() == "dl") {
    m_textSection.putBytes(std::byte{0x80}, std::byte{0xca});
    m_textSection.putBytes(std::byte(argTwo->getValue<int>()));
    return;
  }

  throw CodeGenerationError{"Unknown or operation"};
}

void CodeGenerator::emitTest(const CodeGenerator::InstructionValue &instruction) {
  const auto &argOne = instruction->getArg1();
  const auto &argTwo = instruction->getArg2();

  // test reg, reg
  if (argOne->getType() == TType::REGISTER && argTwo->getType() == TType::REGISTER &&
      argOne->getValue<std::string>() == argTwo->getValue<std::string>()) {
    m_textSection.putBytes(TestMachineCode[argOne->getValue<std::string>()]);
    return;
  }

  throw CodeGenerationError{"Unknown test operation"};
}

void CodeGenerator::emitRet(const CodeGenerator::InstructionValue &instruction) {
  m_textSection.putBytes(std::byte{0xc3});
}

void CodeGenerator::emitNop(const CodeGenerator::InstructionValue &instruction) {
  m_textSection.putBytes(std::byte{0x90});
}
