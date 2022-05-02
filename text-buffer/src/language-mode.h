#ifndef LANGUAGE_MODE_H_
#define LANGUAGE_MODE_H_

#include "point.h"
#include <memory>

class LanguageMode {
public:
  class HighlightIterator {
  public:
    virtual std::vector<int32_t> seek(Point, double);
    virtual bool moveToSuccessor();
    virtual Point getPosition();
    virtual std::vector<int32_t> getCloseTags();
    virtual std::vector<int32_t> getOpenTags();
    virtual std::vector<int32_t> getCloseScopeIds();
    virtual std::vector<int32_t> getOpenScopeIds();
  };
  virtual void bufferDidChange();
  virtual void bufferDidFinishTransaction();
  virtual std::unique_ptr<HighlightIterator> buildHighlightIterator();
  virtual std::u16string classNameForScopeId(int32_t);
  virtual std::u16string getLanguageId();
};

#endif  // LANGUAGE_MODE_H_
