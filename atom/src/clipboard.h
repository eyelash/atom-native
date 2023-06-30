#ifndef CLIPBOARD_H_
#define CLIPBOARD_H_

#include <string>
#include <vector>

struct Clipboard {
  struct Selection {
    std::u16string text;
    double indentBasis;
    bool fullLine;
  };
  struct Metadata {
    double indentBasis;
    bool fullLine;
    std::vector<Selection> selections;
    Metadata(double, bool);
    Metadata();
  };

  Metadata *metadata;
  std::u16string signatureForMetadata;
  std::u16string systemText;

  Clipboard();
  ~Clipboard();

  std::u16string md5(const std::u16string &);
  void write(const std::u16string &, Metadata *);
  std::u16string read();
  std::pair<std::u16string, Metadata *> readWithMetadata();
};

#endif // CLIPBOARD_H_
