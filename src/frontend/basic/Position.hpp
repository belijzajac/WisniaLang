// Copyright (C) 2019-2024 Tautvydas Povilaitis (belijzajac)
// SPDX-License-Identifier: GPL-3.0

#ifndef WISNIALANG_POSITION_HPP
#define WISNIALANG_POSITION_HPP

#include <utility>

namespace Wisnia::Basic {

class Position {
 public:
  Position(std::string file, const size_t line)
      : m_fileName{std::move(file)}
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
