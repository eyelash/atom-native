#ifndef WHITESPACE_H_
#define WHITESPACE_H_

struct TextEditor;

struct Whitespace {
  Whitespace();
  ~Whitespace();

  void handleEvents(TextEditor *);
};

#endif // WHITESPACE_H_
