#ifndef POSITIONINFILE_H
#define POSITIONINFILE_H

namespace Wisnia::Basic {
class PositionInFile {
 public:
  PositionInFile(const std::string &file, int line)
      : fileName_{file}, lineNo_{line} {}

  // Getters for file information
  std::string getFileName() const { return fileName_; }

  int getLineNo() const { return lineNo_; }

 private:
  std::string fileName_{};
  int lineNo_;
};

}  // namespace Wisnia::Basic

#endif  // POSITIONINFILE_H
