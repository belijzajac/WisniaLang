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

#ifndef WISNIALANG_POSITION_HPP
#define WISNIALANG_POSITION_HPP

namespace Wisnia::Basic {

class Position {
 public:
  Position(const std::string &file, size_t line)
      : m_fileName{file}
      , m_lineNo{line}
  {}
  std::string getFileName() const { return m_fileName; }
  size_t getLineNo() const { return m_lineNo; }

 private:
  std::string m_fileName;
  size_t m_lineNo;
};

}  // namespace Wisnia::Basic

#endif  // WISNIALANG_POSITION_HPP
