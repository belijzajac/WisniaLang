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

// Wisnia
#include "AST.hpp"
#include "Lexer.hpp"
#include "Parser.hpp"
#include "SemanticAnalysis.hpp"
#include "SemanticTestFixture.hpp"

using namespace Wisnia;
using ASTPrinterTest = SemanticTestFixture;

TEST_F(ASTPrinterTest, ASTIsCorrectlyIndented) {
  SemanticAnalysis analysis;
  std::stringstream ss;
  m_root->accept(analysis);
  m_root->print(ss);

  EXPECT_STREQ(ss.str().c_str(),
    "Root\n"
    "  ClassDef\n"
    "    Var (name=Foo, type=class)\n"
    "    FnDef\n"
    "      Var (name=simple_operations, type=void)\n"
    "      StmtBlock\n"
    "        VarDeclStmt\n"
    "          Var (name=a, type=float)\n"
    "          AddExpr (+)\n"
    "            Var (name=number, type=float)\n"
    "            FloatExpr (7.750000)\n"
    "        ForLoop\n"
    "          VarDeclStmt\n"
    "            Var (name=i, type=int)\n"
    "            IntExpr (0)\n"
    "          CompExpr (<)\n"
    "            Var (name=i, type=int)\n"
    "            IntExpr (5)\n"
    "          VarAssignStmt\n"
    "            Var (name=i, type=int)\n"
    "            AddExpr (+)\n"
    "              Var (name=i, type=int)\n"
    "              IntExpr (1)\n"
    "          StmtBlock\n"
    "            VarAssignStmt\n"
    "              Var (name=a, type=float)\n"
    "              AddExpr (+)\n"
    "                Var (name=a, type=float)\n"
    "                FloatExpr (1.450000)\n"
    "            IfStmt\n"
    "              EqExpr (==)\n"
    "                Var (name=a, type=float)\n"
    "                FloatExpr (15.000000)\n"
    "              StmtBlock\n"
    "                VarAssignStmt\n"
    "                  Var (name=is_fifteen, type=bool)\n"
    "                  BoolExpr (true)\n"
    "                BreakStmt\n"
    "        WhileLoop\n"
    "          CompExpr (<)\n"
    "            Var (name=digit, type=int)\n"
    "            IntExpr (150000)\n"
    "          StmtBlock\n"
    "            VarAssignStmt\n"
    "              Var (name=a, type=float)\n"
    "              SubExpr (-)\n"
    "                Var (name=a, type=float)\n"
    "                FloatExpr (1.330000)\n"
    "            VarAssignStmt\n"
    "              Var (name=digit, type=int)\n"
    "              MultExpr (*)\n"
    "                Var (name=digit, type=int)\n"
    "                IntExpr (2)\n"
    "            WriteStmt\n"
    "              Var (name=a, type=float)\n"
    "              Var (name=digit, type=int)\n"
    "    Field\n"
    "      Var (name=is_fifteen, type=bool)\n"
    "    Field\n"
    "      Var (name=number, type=float)\n"
    "    Field\n"
    "      Var (name=digit, type=int)\n"
    "  FnDef\n"
    "    Var (name=output_hello, type=string)\n"
    "    Param\n"
    "      Var (name=do_output, type=bool)\n"
    "    StmtBlock\n"
    "      IfStmt\n"
    "        Var (name=do_output, type=bool)\n"
    "        StmtBlock\n"
    "          ReturnStmt\n"
    "            StringExpr (\"hello\\0\")\n"
    "      ReturnStmt\n"
    "        StringExpr (\"bye\\0\")\n"
    "  FnDef\n"
    "    Var (name=main, type=int)\n"
    "    Param\n"
    "      Var (name=argc, type=int)\n"
    "    Param\n"
    "      Var (name=argv, type=string)\n"
    "    StmtBlock\n"
    "      VarDeclStmt\n"
    "        Var (name=correct, type=bool)\n"
    "        EqExpr (==)\n"
    "          Var (name=argc, type=int)\n"
    "          IntExpr (5)\n"
    "      VarDeclStmt\n"
    "        Var (name=answer, type=string)\n"
    "        FnCallExpr\n"
    "          Var (name=output_hello, type=string)\n"
    "          Var (name=correct, type=bool)\n"
    "      VarDeclStmt\n"
    "        Var (name=fooPtr, type=void)\n"
    "        ClassInitExpr\n"
    "          Var (name=Foo, type=class)\n"
    "      WhileLoop\n"
    "        BoolExpr (true)\n"
    "        StmtBlock\n"
    "          IfStmt\n"
    "            Var (name=correct, type=bool)\n"
    "            StmtBlock\n"
    "              VarDeclStmt\n"
    "                Var (name=yes, type=bool)\n"
    "                BoolExpr (false)\n"
    "              VarAssignStmt\n"
    "                Var (name=answer, type=string)\n"
    "                FnCallExpr\n"
    "                  Var (name=output_hello, type=string)\n"
    "                  Var (name=yes, type=bool)\n"
    "          ElseIfStmt\n"
    "            CompExpr (<)\n"
    "              Var (name=argc, type=int)\n"
    "              IntExpr (5)\n"
    "            StmtBlock\n"
    "              VarAssignStmt\n"
    "                Var (name=answer, type=string)\n"
    "                FnCallExpr\n"
    "                  Var (name=output_hello, type=string)\n"
    "                  BoolExpr (false)\n"
    "          ElseIfStmt\n"
    "            CompExpr (>)\n"
    "              Var (name=argc, type=int)\n"
    "              IntExpr (7)\n"
    "            StmtBlock\n"
    "              ContinueStmt\n"
    "          ElseStmt\n"
    "            StmtBlock\n"
    "              BreakStmt\n"
    "      ReturnStmt\n"
    "        IntExpr (5)\n");
}
