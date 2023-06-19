#ifndef LANGUAGE_MODE_H_
#define LANGUAGE_MODE_H_

#include "range.h"
#include <optional.h>
#include <memory>
#include <functional>

struct LanguageMode {
  struct HighlightIterator {
    virtual ~HighlightIterator();
    virtual std::vector<int32_t> seek(Point, double);
    virtual void moveToSuccessor();
    virtual Point getPosition();
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
  virtual double suggestedIndentForLineAtBufferRow(double, const std::u16string &, double);
  virtual double suggestedIndentForBufferRow(double, double, bool);
  virtual optional<double> suggestedIndentForEditedBufferRow(double, double);
  virtual optional<Range> getRangeForSyntaxNodeContainingRange(Range);
};

#endif // LANGUAGE_MODE_H_
