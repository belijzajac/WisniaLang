// Copyright (C) 2019-2024 Tautvydas Povilaitis (belijzajac)
// SPDX-License-Identifier: GPL-3.0

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
