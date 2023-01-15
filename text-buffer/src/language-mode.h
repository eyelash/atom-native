#ifndef LANGUAGE_MODE_H_
#define LANGUAGE_MODE_H_

#include "range.h"
#include <memory>
#include <functional>

class LanguageMode {
public:
  class HighlightIterator {
  public:
    virtual ~HighlightIterator();
    virtual std::vector<int32_t> seek(Point, double);
    virtual bool moveToSuccessor();
    virtual Point getPosition();
    virtual std::vector<int32_t> getCloseTags();
    virtual std::vector<int32_t> getOpenTags();
    virtual std::vector<int32_t> getCloseScopeIds();
    virtual std::vector<int32_t> getOpenScopeIds();
  };
  virtual ~LanguageMode();
  virtual void bufferDidChange(Range, Range, const std::u16string &, const std::u16string &);
  virtual void bufferDidFinishTransaction();
  virtual std::unique_ptr<HighlightIterator> buildHighlightIterator();
  virtual void onDidChangeHighlighting(std::function<void(Range)>);
  virtual std::string classNameForScopeId(int32_t);
  virtual std::u16string getLanguageId();
};

#endif  // LANGUAGE_MODE_H_
