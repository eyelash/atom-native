#ifndef LANGUAGE_MODE_H_
#define LANGUAGE_MODE_H_

#include "range.h"
#include <optional.h>
#include <memory>
#include <functional>

struct Grammar;

struct LanguageMode {
  struct HighlightIterator {
    virtual ~HighlightIterator();
    virtual std::vector<int32_t> seek(const Point &, double);
    virtual void moveToSuccessor();
    virtual Point getPosition();
    virtual std::vector<int32_t> getCloseScopeIds();
    virtual std::vector<int32_t> getOpenScopeIds();
  };
  virtual ~LanguageMode();
  virtual void bufferDidChange(const Range &, const Range &, const std::u16string &, const std::u16string &);
  virtual void bufferDidFinishTransaction();
  virtual std::unique_ptr<HighlightIterator> buildHighlightIterator();
  virtual void onDidChangeHighlighting(std::function<void(const Range &)>);
  virtual std::string classNameForScopeId(int32_t);
  virtual bool isRowCommented(double);
  virtual std::u16string getLanguageId();
  virtual double suggestedIndentForLineAtBufferRow(double, const std::u16string &, double);
  virtual double suggestedIndentForBufferRow(double, double, bool);
  virtual optional<double> suggestedIndentForEditedBufferRow(double, double);
  virtual Grammar *getGrammar();
};

#endif // LANGUAGE_MODE_H_
