#ifndef WHITESPACE_H_
#define WHITESPACE_H_

struct TextEditor;

struct Whitespace {
  Whitespace();
  ~Whitespace();

  void handleEvents(TextEditor *);
  void removeTrailingWhitespace(TextEditor *);
  void ensureSingleTrailingNewline(TextEditor *);
};

#endif // WHITESPACE_H_
