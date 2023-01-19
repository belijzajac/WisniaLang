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

#include <gtest/gtest.h>
// Wisnia
#include "AST.hpp"
#include "IRGenerator.hpp"
#include "Instruction.hpp"
#include "Lexer.hpp"
#include "Modules.hpp"
#include "Parser.hpp"
#include "SemanticAnalysis.hpp"

using namespace Wisnia;
using namespace Basic;
using namespace std::literals;

class IRGeneratorTestWithRegisterAllocationFixture : public testing::Test {
 protected:
  void SetUp(std::string_view program) {
    std::istringstream iss{program.data()};
    Lexer lexer{iss};
    Parser parser{lexer};
    const auto &root = parser.parse();
    root->accept(m_analysis);
    root->accept(m_generator);
  }

  void TearDown() override {
    Modules::markAllAsUnused();
  }

 protected:
  IRGenerator m_generator{};

 private:
  SemanticAnalysis m_analysis{};
};

using IRGeneratorTestWithRegisterAllocation = IRGeneratorTestWithRegisterAllocationFixture;

TEST_F(IRGeneratorTestWithRegisterAllocation, InstructionsShouldBeOptimized) {
  constexpr auto program = R"(
  fn main() {
    print((1 + 2) * 3);
  })"sv;
  SetUp(program.data());
  const auto &unoptimizedInstructions = m_generator.getInstructions();
  const auto &optimizedInstructions   = m_generator.getInstructionsAfterInstructionOptimization();

  // since we add extra instructions from `_print_number_` module
  EXPECT_LT(unoptimizedInstructions.size(), optimizedInstructions.size());

  // `rax <- rax`
  EXPECT_EQ(unoptimizedInstructions[2]->getOperation(), Operation::MOV);
  EXPECT_EQ(unoptimizedInstructions[2]->getTarget()->getType(), TType::REGISTER);
  EXPECT_EQ(unoptimizedInstructions[2]->getArg1()->getType(), TType::REGISTER);
  EXPECT_EQ(unoptimizedInstructions[2]->getTarget()->getValue<Basic::register_t>(),
            unoptimizedInstructions[2]->getArg1()->getValue<Basic::register_t>());

  // `rax <- rax` has been optimized out, so should not be present anymore

  // the following unoptimized instruction should contain IR for `rax * 3`
  EXPECT_EQ(unoptimizedInstructions[3]->getOperation(), Operation::IMUL);
  EXPECT_EQ(unoptimizedInstructions[3]->getTarget()->getType(), TType::REGISTER);
  EXPECT_EQ(unoptimizedInstructions[3]->getArg1()->getType(), TType::LIT_INT);
  EXPECT_EQ(unoptimizedInstructions[3]->getTarget()->getValue<Basic::register_t>(), Basic::register_t::RAX);
  EXPECT_EQ(unoptimizedInstructions[3]->getArg1()->getValue<int64_t>(), 3);

  // in its old place (`rax <- rax`) now should stand IR for `rax * 3`
  EXPECT_EQ(optimizedInstructions[2]->getOperation(), Operation::IMUL);
  EXPECT_EQ(optimizedInstructions[2]->getTarget()->getType(), TType::REGISTER);
  EXPECT_EQ(optimizedInstructions[2]->getArg1()->getType(), TType::LIT_INT);
  EXPECT_EQ(optimizedInstructions[2]->getTarget()->getValue<Basic::register_t>(), Basic::register_t::RAX);
  EXPECT_EQ(optimizedInstructions[2]->getArg1()->getValue<int64_t>(), 3);
}

TEST_F(IRGeneratorTestWithRegisterAllocation, PrintNumberLiteralShouldNotInsertModules) {
  constexpr auto program = R"(
  fn main() {
    print(12345);
  })"sv;
  SetUp(program.data());
  const auto &instructions = m_generator.getInstructionsAfterInstructionOptimization();

  EXPECT_EQ(instructions.size(), 22);
  // push rdx
  EXPECT_EQ(instructions[0]->getOperation(), Operation::PUSH);
  EXPECT_EQ(instructions[0]->getArg1()->getType(), TType::REGISTER);
  EXPECT_EQ(instructions[0]->getArg1()->getValue<Basic::register_t>(), Basic::register_t::RDX);
  // push rsi
  EXPECT_EQ(instructions[1]->getOperation(), Operation::PUSH);
  EXPECT_EQ(instructions[1]->getArg1()->getType(), TType::REGISTER);
  EXPECT_EQ(instructions[1]->getArg1()->getValue<Basic::register_t>(), Basic::register_t::RSI);
  // mov rdx, 5
  EXPECT_EQ(instructions[2]->getOperation(), Operation::MOV);
  EXPECT_EQ(instructions[2]->getTarget()->getType(), TType::REGISTER);
  EXPECT_EQ(instructions[2]->getTarget()->getValue<Basic::register_t>(), Basic::register_t::RDX);
  EXPECT_EQ(instructions[2]->getArg1()->getType(), TType::LIT_INT);
  EXPECT_EQ(instructions[2]->getArg1()->getValue<int64_t>(), 5);
  // mov rsi, "12345"
  EXPECT_EQ(instructions[3]->getOperation(), Operation::MOV);
  EXPECT_EQ(instructions[3]->getTarget()->getType(), TType::REGISTER);
  EXPECT_EQ(instructions[3]->getTarget()->getValue<Basic::register_t>(), Basic::register_t::RSI);
  EXPECT_EQ(instructions[3]->getArg1()->getType(), TType::LIT_STR);
  EXPECT_STREQ(instructions[3]->getArg1()->getValue<std::string>().c_str(), "12345");
  // push rax
  EXPECT_EQ(instructions[4]->getOperation(), Operation::PUSH);
  EXPECT_EQ(instructions[4]->getArg1()->getType(), TType::REGISTER);
  EXPECT_EQ(instructions[4]->getArg1()->getValue<Basic::register_t>(), Basic::register_t::RAX);
  // push rcx
  EXPECT_EQ(instructions[5]->getOperation(), Operation::PUSH);
  EXPECT_EQ(instructions[5]->getArg1()->getType(), TType::REGISTER);
  EXPECT_EQ(instructions[5]->getArg1()->getValue<Basic::register_t>(), Basic::register_t::RCX);
  // push r11
  EXPECT_EQ(instructions[6]->getOperation(), Operation::PUSH);
  EXPECT_EQ(instructions[6]->getArg1()->getType(), TType::REGISTER);
  EXPECT_EQ(instructions[6]->getArg1()->getValue<Basic::register_t>(), Basic::register_t::R11);
  // push rdi
  EXPECT_EQ(instructions[7]->getOperation(), Operation::PUSH);
  EXPECT_EQ(instructions[7]->getArg1()->getType(), TType::REGISTER);
  EXPECT_EQ(instructions[7]->getArg1()->getValue<Basic::register_t>(), Basic::register_t::RDI);
  // mov rax, 1
  EXPECT_EQ(instructions[8]->getOperation(), Operation::MOV);
  EXPECT_EQ(instructions[8]->getTarget()->getType(), TType::REGISTER);
  EXPECT_EQ(instructions[8]->getTarget()->getValue<Basic::register_t>(), Basic::register_t::RAX);
  EXPECT_EQ(instructions[8]->getArg1()->getType(), TType::LIT_INT);
  EXPECT_EQ(instructions[8]->getArg1()->getValue<int64_t>(), 1);
  // mov rdi, 1
  EXPECT_EQ(instructions[9]->getOperation(), Operation::MOV);
  EXPECT_EQ(instructions[9]->getTarget()->getType(), TType::REGISTER);
  EXPECT_EQ(instructions[9]->getTarget()->getValue<Basic::register_t>(), Basic::register_t::RDI);
  EXPECT_EQ(instructions[9]->getArg1()->getType(), TType::LIT_INT);
  EXPECT_EQ(instructions[9]->getArg1()->getValue<int64_t>(), 1);
  // syscall
  EXPECT_EQ(instructions[10]->getOperation(), Operation::SYSCALL);
  // pop rdi
  EXPECT_EQ(instructions[11]->getOperation(), Operation::POP);
  EXPECT_EQ(instructions[11]->getArg1()->getType(), TType::REGISTER);
  EXPECT_EQ(instructions[11]->getArg1()->getValue<Basic::register_t>(), Basic::register_t::RDI);
  // pop r11
  EXPECT_EQ(instructions[12]->getOperation(), Operation::POP);
  EXPECT_EQ(instructions[12]->getArg1()->getType(), TType::REGISTER);
  EXPECT_EQ(instructions[12]->getArg1()->getValue<Basic::register_t>(), Basic::register_t::R11);
  // pop rcx
  EXPECT_EQ(instructions[13]->getOperation(), Operation::POP);
  EXPECT_EQ(instructions[13]->getArg1()->getType(), TType::REGISTER);
  EXPECT_EQ(instructions[13]->getArg1()->getValue<Basic::register_t>(), Basic::register_t::RCX);
  // pop rax
  EXPECT_EQ(instructions[14]->getOperation(), Operation::POP);
  EXPECT_EQ(instructions[14]->getArg1()->getType(), TType::REGISTER);
  EXPECT_EQ(instructions[14]->getArg1()->getValue<Basic::register_t>(), Basic::register_t::RAX);
  // pop rsi
  EXPECT_EQ(instructions[15]->getOperation(), Operation::POP);
  EXPECT_EQ(instructions[15]->getArg1()->getType(), TType::REGISTER);
  EXPECT_EQ(instructions[15]->getArg1()->getValue<Basic::register_t>(), Basic::register_t::RSI);
  // pop rdx
  EXPECT_EQ(instructions[16]->getOperation(), Operation::POP);
  EXPECT_EQ(instructions[16]->getArg1()->getType(), TType::REGISTER);
  EXPECT_EQ(instructions[16]->getArg1()->getValue<Basic::register_t>(), Basic::register_t::RDX);
  // call _exit_
  EXPECT_EQ(instructions[17]->getOperation(), Operation::CALL);
  EXPECT_EQ(instructions[17]->getTarget()->getType(), TType::IDENT_VOID);
  EXPECT_STREQ(instructions[17]->getTarget()->getValue<std::string>().c_str(), "_exit_");
  // label _exit_
  EXPECT_EQ(instructions[18]->getOperation(), Operation::LABEL);
  EXPECT_EQ(instructions[18]->getArg1()->getType(), TType::IDENT_VOID);
  EXPECT_STREQ(instructions[18]->getArg1()->getValue<std::string>().c_str(), "_exit_");
  // xor rdi, rdi
  EXPECT_EQ(instructions[19]->getOperation(), Operation::XOR);
  EXPECT_EQ(instructions[19]->getArg1()->getType(), TType::REGISTER);
  EXPECT_EQ(instructions[19]->getArg1()->getValue<Basic::register_t>(), Basic::register_t::RDI);
  EXPECT_EQ(instructions[19]->getArg2()->getType(), TType::REGISTER);
  EXPECT_EQ(instructions[19]->getArg2()->getValue<Basic::register_t>(), Basic::register_t::RDI);
  // mov rax, 60
  EXPECT_EQ(instructions[20]->getOperation(), Operation::MOV);
  EXPECT_EQ(instructions[20]->getTarget()->getType(), TType::REGISTER);
  EXPECT_EQ(instructions[20]->getTarget()->getValue<Basic::register_t>(), Basic::register_t::RAX);
  EXPECT_EQ(instructions[20]->getArg1()->getType(), TType::LIT_INT);
  EXPECT_EQ(instructions[20]->getArg1()->getValue<int64_t>(), 60);
  // syscall
  EXPECT_EQ(instructions[21]->getOperation(), Operation::SYSCALL);
}

TEST_F(IRGeneratorTestWithRegisterAllocation, PrintStringLiteralShouldNotInsertModules) {
  constexpr auto program = R"(
  fn main() {
    print("haiii");
  })"sv;
  SetUp(program.data());
  const auto &instructions = m_generator.getInstructionsAfterInstructionOptimization();

  EXPECT_EQ(instructions.size(), 22);
  // push rdx
  EXPECT_EQ(instructions[0]->getOperation(), Operation::PUSH);
  EXPECT_EQ(instructions[0]->getArg1()->getType(), TType::REGISTER);
  EXPECT_EQ(instructions[0]->getArg1()->getValue<Basic::register_t>(), Basic::register_t::RDX);
  // push rsi
  EXPECT_EQ(instructions[1]->getOperation(), Operation::PUSH);
  EXPECT_EQ(instructions[1]->getArg1()->getType(), TType::REGISTER);
  EXPECT_EQ(instructions[1]->getArg1()->getValue<Basic::register_t>(), Basic::register_t::RSI);
  // mov rdx, 5
  EXPECT_EQ(instructions[2]->getOperation(), Operation::MOV);
  EXPECT_EQ(instructions[2]->getTarget()->getType(), TType::REGISTER);
  EXPECT_EQ(instructions[2]->getTarget()->getValue<Basic::register_t>(), Basic::register_t::RDX);
  EXPECT_EQ(instructions[2]->getArg1()->getType(), TType::LIT_INT);
  EXPECT_EQ(instructions[2]->getArg1()->getValue<int64_t>(), 5);
  // mov rsi, "12345"
  EXPECT_EQ(instructions[3]->getOperation(), Operation::MOV);
  EXPECT_EQ(instructions[3]->getTarget()->getType(), TType::REGISTER);
  EXPECT_EQ(instructions[3]->getTarget()->getValue<Basic::register_t>(), Basic::register_t::RSI);
  EXPECT_EQ(instructions[3]->getArg1()->getType(), TType::LIT_STR);
  EXPECT_STREQ(instructions[3]->getArg1()->getValue<std::string>().c_str(), "haiii");
  // push rax
  EXPECT_EQ(instructions[4]->getOperation(), Operation::PUSH);
  EXPECT_EQ(instructions[4]->getArg1()->getType(), TType::REGISTER);
  EXPECT_EQ(instructions[4]->getArg1()->getValue<Basic::register_t>(), Basic::register_t::RAX);
  // push rcx
  EXPECT_EQ(instructions[5]->getOperation(), Operation::PUSH);
  EXPECT_EQ(instructions[5]->getArg1()->getType(), TType::REGISTER);
  EXPECT_EQ(instructions[5]->getArg1()->getValue<Basic::register_t>(), Basic::register_t::RCX);
  // push r11
  EXPECT_EQ(instructions[6]->getOperation(), Operation::PUSH);
  EXPECT_EQ(instructions[6]->getArg1()->getType(), TType::REGISTER);
  EXPECT_EQ(instructions[6]->getArg1()->getValue<Basic::register_t>(), Basic::register_t::R11);
  // push rdi
  EXPECT_EQ(instructions[7]->getOperation(), Operation::PUSH);
  EXPECT_EQ(instructions[7]->getArg1()->getType(), TType::REGISTER);
  EXPECT_EQ(instructions[7]->getArg1()->getValue<Basic::register_t>(), Basic::register_t::RDI);
  // mov rax, 1
  EXPECT_EQ(instructions[8]->getOperation(), Operation::MOV);
  EXPECT_EQ(instructions[8]->getTarget()->getType(), TType::REGISTER);
  EXPECT_EQ(instructions[8]->getTarget()->getValue<Basic::register_t>(), Basic::register_t::RAX);
  EXPECT_EQ(instructions[8]->getArg1()->getType(), TType::LIT_INT);
  EXPECT_EQ(instructions[8]->getArg1()->getValue<int64_t>(), 1);
  // mov rdi, 1
  EXPECT_EQ(instructions[9]->getOperation(), Operation::MOV);
  EXPECT_EQ(instructions[9]->getTarget()->getType(), TType::REGISTER);
  EXPECT_EQ(instructions[9]->getTarget()->getValue<Basic::register_t>(), Basic::register_t::RDI);
  EXPECT_EQ(instructions[9]->getArg1()->getType(), TType::LIT_INT);
  EXPECT_EQ(instructions[9]->getArg1()->getValue<int64_t>(), 1);
  // syscall
  EXPECT_EQ(instructions[10]->getOperation(), Operation::SYSCALL);
  // pop rdi
  EXPECT_EQ(instructions[11]->getOperation(), Operation::POP);
  EXPECT_EQ(instructions[11]->getArg1()->getType(), TType::REGISTER);
  EXPECT_EQ(instructions[11]->getArg1()->getValue<Basic::register_t>(), Basic::register_t::RDI);
  // pop r11
  EXPECT_EQ(instructions[12]->getOperation(), Operation::POP);
  EXPECT_EQ(instructions[12]->getArg1()->getType(), TType::REGISTER);
  EXPECT_EQ(instructions[12]->getArg1()->getValue<Basic::register_t>(), Basic::register_t::R11);
  // pop rcx
  EXPECT_EQ(instructions[13]->getOperation(), Operation::POP);
  EXPECT_EQ(instructions[13]->getArg1()->getType(), TType::REGISTER);
  EXPECT_EQ(instructions[13]->getArg1()->getValue<Basic::register_t>(), Basic::register_t::RCX);
  // pop rax
  EXPECT_EQ(instructions[14]->getOperation(), Operation::POP);
  EXPECT_EQ(instructions[14]->getArg1()->getType(), TType::REGISTER);
  EXPECT_EQ(instructions[14]->getArg1()->getValue<Basic::register_t>(), Basic::register_t::RAX);
  // pop rsi
  EXPECT_EQ(instructions[15]->getOperation(), Operation::POP);
  EXPECT_EQ(instructions[15]->getArg1()->getType(), TType::REGISTER);
  EXPECT_EQ(instructions[15]->getArg1()->getValue<Basic::register_t>(), Basic::register_t::RSI);
  // pop rdx
  EXPECT_EQ(instructions[16]->getOperation(), Operation::POP);
  EXPECT_EQ(instructions[16]->getArg1()->getType(), TType::REGISTER);
  EXPECT_EQ(instructions[16]->getArg1()->getValue<Basic::register_t>(), Basic::register_t::RDX);
  // call _exit_
  EXPECT_EQ(instructions[17]->getOperation(), Operation::CALL);
  EXPECT_EQ(instructions[17]->getTarget()->getType(), TType::IDENT_VOID);
  EXPECT_STREQ(instructions[17]->getTarget()->getValue<std::string>().c_str(), "_exit_");
  // label _exit_
  EXPECT_EQ(instructions[18]->getOperation(), Operation::LABEL);
  EXPECT_EQ(instructions[18]->getArg1()->getType(), TType::IDENT_VOID);
  EXPECT_STREQ(instructions[18]->getArg1()->getValue<std::string>().c_str(), "_exit_");
  // xor rdi, rdi
  EXPECT_EQ(instructions[19]->getOperation(), Operation::XOR);
  EXPECT_EQ(instructions[19]->getArg1()->getType(), TType::REGISTER);
  EXPECT_EQ(instructions[19]->getArg1()->getValue<Basic::register_t>(), Basic::register_t::RDI);
  EXPECT_EQ(instructions[19]->getArg2()->getType(), TType::REGISTER);
  EXPECT_EQ(instructions[19]->getArg2()->getValue<Basic::register_t>(), Basic::register_t::RDI);
  // mov rax, 60
  EXPECT_EQ(instructions[20]->getOperation(), Operation::MOV);
  EXPECT_EQ(instructions[20]->getTarget()->getType(), TType::REGISTER);
  EXPECT_EQ(instructions[20]->getTarget()->getValue<Basic::register_t>(), Basic::register_t::RAX);
  EXPECT_EQ(instructions[20]->getArg1()->getType(), TType::LIT_INT);
  EXPECT_EQ(instructions[20]->getArg1()->getValue<int64_t>(), 60);
  // syscall
  EXPECT_EQ(instructions[21]->getOperation(), Operation::SYSCALL);
}

TEST_F(IRGeneratorTestWithRegisterAllocation, PrintBooleanLiteralShouldNotInsertModules) {
  constexpr auto program = R"(
  fn main() {
    print(true, false);
  })"sv;
  SetUp(program.data());
  const auto &instructions = m_generator.getInstructionsAfterInstructionOptimization();

  EXPECT_EQ(instructions.size(), 39);
  // push rdx
  EXPECT_EQ(instructions[0]->getOperation(), Operation::PUSH);
  EXPECT_EQ(instructions[0]->getArg1()->getType(), TType::REGISTER);
  EXPECT_EQ(instructions[0]->getArg1()->getValue<Basic::register_t>(), Basic::register_t::RDX);
  // push rsi
  EXPECT_EQ(instructions[1]->getOperation(), Operation::PUSH);
  EXPECT_EQ(instructions[1]->getArg1()->getType(), TType::REGISTER);
  EXPECT_EQ(instructions[1]->getArg1()->getValue<Basic::register_t>(), Basic::register_t::RSI);
  // mov rdx, 4
  EXPECT_EQ(instructions[2]->getOperation(), Operation::MOV);
  EXPECT_EQ(instructions[2]->getTarget()->getType(), TType::REGISTER);
  EXPECT_EQ(instructions[2]->getTarget()->getValue<Basic::register_t>(), Basic::register_t::RDX);
  EXPECT_EQ(instructions[2]->getArg1()->getType(), TType::LIT_INT);
  EXPECT_EQ(instructions[2]->getArg1()->getValue<int64_t>(), 4);
  // mov rsi, "true"
  EXPECT_EQ(instructions[3]->getOperation(), Operation::MOV);
  EXPECT_EQ(instructions[3]->getTarget()->getType(), TType::REGISTER);
  EXPECT_EQ(instructions[3]->getTarget()->getValue<Basic::register_t>(), Basic::register_t::RSI);
  EXPECT_EQ(instructions[3]->getArg1()->getType(), TType::LIT_STR);
  EXPECT_STREQ(instructions[3]->getArg1()->getValue<std::string>().c_str(), "true");
  // push rax
  EXPECT_EQ(instructions[4]->getOperation(), Operation::PUSH);
  EXPECT_EQ(instructions[4]->getArg1()->getType(), TType::REGISTER);
  EXPECT_EQ(instructions[4]->getArg1()->getValue<Basic::register_t>(), Basic::register_t::RAX);
  // push rcx
  EXPECT_EQ(instructions[5]->getOperation(), Operation::PUSH);
  EXPECT_EQ(instructions[5]->getArg1()->getType(), TType::REGISTER);
  EXPECT_EQ(instructions[5]->getArg1()->getValue<Basic::register_t>(), Basic::register_t::RCX);
  // push r11
  EXPECT_EQ(instructions[6]->getOperation(), Operation::PUSH);
  EXPECT_EQ(instructions[6]->getArg1()->getType(), TType::REGISTER);
  EXPECT_EQ(instructions[6]->getArg1()->getValue<Basic::register_t>(), Basic::register_t::R11);
  // push rdi
  EXPECT_EQ(instructions[7]->getOperation(), Operation::PUSH);
  EXPECT_EQ(instructions[7]->getArg1()->getType(), TType::REGISTER);
  EXPECT_EQ(instructions[7]->getArg1()->getValue<Basic::register_t>(), Basic::register_t::RDI);
  // mov rax, 1
  EXPECT_EQ(instructions[8]->getOperation(), Operation::MOV);
  EXPECT_EQ(instructions[8]->getTarget()->getType(), TType::REGISTER);
  EXPECT_EQ(instructions[8]->getTarget()->getValue<Basic::register_t>(), Basic::register_t::RAX);
  EXPECT_EQ(instructions[8]->getArg1()->getType(), TType::LIT_INT);
  EXPECT_EQ(instructions[8]->getArg1()->getValue<int64_t>(), 1);
  // mov rdi, 1
  EXPECT_EQ(instructions[9]->getOperation(), Operation::MOV);
  EXPECT_EQ(instructions[9]->getTarget()->getType(), TType::REGISTER);
  EXPECT_EQ(instructions[9]->getTarget()->getValue<Basic::register_t>(), Basic::register_t::RDI);
  EXPECT_EQ(instructions[9]->getArg1()->getType(), TType::LIT_INT);
  EXPECT_EQ(instructions[9]->getArg1()->getValue<int64_t>(), 1);
  // syscall
  EXPECT_EQ(instructions[10]->getOperation(), Operation::SYSCALL);
  // pop rdi
  EXPECT_EQ(instructions[11]->getOperation(), Operation::POP);
  EXPECT_EQ(instructions[11]->getArg1()->getType(), TType::REGISTER);
  EXPECT_EQ(instructions[11]->getArg1()->getValue<Basic::register_t>(), Basic::register_t::RDI);
  // pop r11
  EXPECT_EQ(instructions[12]->getOperation(), Operation::POP);
  EXPECT_EQ(instructions[12]->getArg1()->getType(), TType::REGISTER);
  EXPECT_EQ(instructions[12]->getArg1()->getValue<Basic::register_t>(), Basic::register_t::R11);
  // pop rcx
  EXPECT_EQ(instructions[13]->getOperation(), Operation::POP);
  EXPECT_EQ(instructions[13]->getArg1()->getType(), TType::REGISTER);
  EXPECT_EQ(instructions[13]->getArg1()->getValue<Basic::register_t>(), Basic::register_t::RCX);
  // pop rax
  EXPECT_EQ(instructions[14]->getOperation(), Operation::POP);
  EXPECT_EQ(instructions[14]->getArg1()->getType(), TType::REGISTER);
  EXPECT_EQ(instructions[14]->getArg1()->getValue<Basic::register_t>(), Basic::register_t::RAX);
  // pop rsi
  EXPECT_EQ(instructions[15]->getOperation(), Operation::POP);
  EXPECT_EQ(instructions[15]->getArg1()->getType(), TType::REGISTER);
  EXPECT_EQ(instructions[15]->getArg1()->getValue<Basic::register_t>(), Basic::register_t::RSI);
  // pop rdx
  EXPECT_EQ(instructions[16]->getOperation(), Operation::POP);
  EXPECT_EQ(instructions[16]->getArg1()->getType(), TType::REGISTER);
  EXPECT_EQ(instructions[16]->getArg1()->getValue<Basic::register_t>(), Basic::register_t::RDX);
  // push rdx
  EXPECT_EQ(instructions[17]->getOperation(), Operation::PUSH);
  EXPECT_EQ(instructions[17]->getArg1()->getType(), TType::REGISTER);
  EXPECT_EQ(instructions[17]->getArg1()->getValue<Basic::register_t>(), Basic::register_t::RDX);
  // push rsi
  EXPECT_EQ(instructions[18]->getOperation(), Operation::PUSH);
  EXPECT_EQ(instructions[18]->getArg1()->getType(), TType::REGISTER);
  EXPECT_EQ(instructions[18]->getArg1()->getValue<Basic::register_t>(), Basic::register_t::RSI);
  // mov rdx, 5
  EXPECT_EQ(instructions[19]->getOperation(), Operation::MOV);
  EXPECT_EQ(instructions[19]->getTarget()->getType(), TType::REGISTER);
  EXPECT_EQ(instructions[19]->getTarget()->getValue<Basic::register_t>(), Basic::register_t::RDX);
  EXPECT_EQ(instructions[19]->getArg1()->getType(), TType::LIT_INT);
  EXPECT_EQ(instructions[19]->getArg1()->getValue<int64_t>(), 5);
  // mov rsi, "false"
  EXPECT_EQ(instructions[20]->getOperation(), Operation::MOV);
  EXPECT_EQ(instructions[20]->getTarget()->getType(), TType::REGISTER);
  EXPECT_EQ(instructions[20]->getTarget()->getValue<Basic::register_t>(), Basic::register_t::RSI);
  EXPECT_EQ(instructions[20]->getArg1()->getType(), TType::LIT_STR);
  EXPECT_STREQ(instructions[20]->getArg1()->getValue<std::string>().c_str(), "false");
  // push rax
  EXPECT_EQ(instructions[21]->getOperation(), Operation::PUSH);
  EXPECT_EQ(instructions[21]->getArg1()->getType(), TType::REGISTER);
  EXPECT_EQ(instructions[21]->getArg1()->getValue<Basic::register_t>(), Basic::register_t::RAX);
  // push rcx
  EXPECT_EQ(instructions[22]->getOperation(), Operation::PUSH);
  EXPECT_EQ(instructions[22]->getArg1()->getType(), TType::REGISTER);
  EXPECT_EQ(instructions[22]->getArg1()->getValue<Basic::register_t>(), Basic::register_t::RCX);
  // push r11
  EXPECT_EQ(instructions[23]->getOperation(), Operation::PUSH);
  EXPECT_EQ(instructions[23]->getArg1()->getType(), TType::REGISTER);
  EXPECT_EQ(instructions[23]->getArg1()->getValue<Basic::register_t>(), Basic::register_t::R11);
  // push rdi
  EXPECT_EQ(instructions[24]->getOperation(), Operation::PUSH);
  EXPECT_EQ(instructions[24]->getArg1()->getType(), TType::REGISTER);
  EXPECT_EQ(instructions[24]->getArg1()->getValue<Basic::register_t>(), Basic::register_t::RDI);
  // mov rax, 1
  EXPECT_EQ(instructions[25]->getOperation(), Operation::MOV);
  EXPECT_EQ(instructions[25]->getTarget()->getType(), TType::REGISTER);
  EXPECT_EQ(instructions[25]->getTarget()->getValue<Basic::register_t>(), Basic::register_t::RAX);
  EXPECT_EQ(instructions[25]->getArg1()->getType(), TType::LIT_INT);
  EXPECT_EQ(instructions[25]->getArg1()->getValue<int64_t>(), 1);
  // mov rdi, 1
  EXPECT_EQ(instructions[26]->getOperation(), Operation::MOV);
  EXPECT_EQ(instructions[26]->getTarget()->getType(), TType::REGISTER);
  EXPECT_EQ(instructions[26]->getTarget()->getValue<Basic::register_t>(), Basic::register_t::RDI);
  EXPECT_EQ(instructions[26]->getArg1()->getType(), TType::LIT_INT);
  EXPECT_EQ(instructions[26]->getArg1()->getValue<int64_t>(), 1);
  // syscall
  EXPECT_EQ(instructions[27]->getOperation(), Operation::SYSCALL);
  // pop rdi
  EXPECT_EQ(instructions[28]->getOperation(), Operation::POP);
  EXPECT_EQ(instructions[28]->getArg1()->getType(), TType::REGISTER);
  EXPECT_EQ(instructions[28]->getArg1()->getValue<Basic::register_t>(), Basic::register_t::RDI);
  // pop r11
  EXPECT_EQ(instructions[29]->getOperation(), Operation::POP);
  EXPECT_EQ(instructions[29]->getArg1()->getType(), TType::REGISTER);
  EXPECT_EQ(instructions[29]->getArg1()->getValue<Basic::register_t>(), Basic::register_t::R11);
  // pop rcx
  EXPECT_EQ(instructions[30]->getOperation(), Operation::POP);
  EXPECT_EQ(instructions[30]->getArg1()->getType(), TType::REGISTER);
  EXPECT_EQ(instructions[30]->getArg1()->getValue<Basic::register_t>(), Basic::register_t::RCX);
  // pop rax
  EXPECT_EQ(instructions[31]->getOperation(), Operation::POP);
  EXPECT_EQ(instructions[31]->getArg1()->getType(), TType::REGISTER);
  EXPECT_EQ(instructions[31]->getArg1()->getValue<Basic::register_t>(), Basic::register_t::RAX);
  // pop rsi
  EXPECT_EQ(instructions[32]->getOperation(), Operation::POP);
  EXPECT_EQ(instructions[32]->getArg1()->getType(), TType::REGISTER);
  EXPECT_EQ(instructions[32]->getArg1()->getValue<Basic::register_t>(), Basic::register_t::RSI);
  // pop rdx
  EXPECT_EQ(instructions[33]->getOperation(), Operation::POP);
  EXPECT_EQ(instructions[33]->getArg1()->getType(), TType::REGISTER);
  EXPECT_EQ(instructions[33]->getArg1()->getValue<Basic::register_t>(), Basic::register_t::RDX);
  // call _exit_
  EXPECT_EQ(instructions[34]->getOperation(), Operation::CALL);
  EXPECT_EQ(instructions[34]->getTarget()->getType(), TType::IDENT_VOID);
  EXPECT_STREQ(instructions[34]->getTarget()->getValue<std::string>().c_str(), "_exit_");
  // label _exit_
  EXPECT_EQ(instructions[35]->getOperation(), Operation::LABEL);
  EXPECT_EQ(instructions[35]->getArg1()->getType(), TType::IDENT_VOID);
  EXPECT_STREQ(instructions[35]->getArg1()->getValue<std::string>().c_str(), "_exit_");
  // xor rdi, rdi
  EXPECT_EQ(instructions[36]->getOperation(), Operation::XOR);
  EXPECT_EQ(instructions[36]->getArg1()->getType(), TType::REGISTER);
  EXPECT_EQ(instructions[36]->getArg1()->getValue<Basic::register_t>(), Basic::register_t::RDI);
  EXPECT_EQ(instructions[36]->getArg2()->getType(), TType::REGISTER);
  EXPECT_EQ(instructions[36]->getArg2()->getValue<Basic::register_t>(), Basic::register_t::RDI);
  // mov rax, 60
  EXPECT_EQ(instructions[37]->getOperation(), Operation::MOV);
  EXPECT_EQ(instructions[37]->getTarget()->getType(), TType::REGISTER);
  EXPECT_EQ(instructions[37]->getTarget()->getValue<Basic::register_t>(), Basic::register_t::RAX);
  EXPECT_EQ(instructions[37]->getArg1()->getType(), TType::LIT_INT);
  EXPECT_EQ(instructions[37]->getArg1()->getValue<int64_t>(), 60);
  // syscall
  EXPECT_EQ(instructions[38]->getOperation(), Operation::SYSCALL);
}
