#include <tree-sitter-grammar.h>

extern "C" const TSLanguage *tree_sitter_python();

extern "C" TreeSitterGrammar *atom_language_python() {
  TreeSitterGrammar *grammar = new TreeSitterGrammar(
    "Python",
    "source.python",
    tree_sitter_python()
  );

  grammar->addFileTypes(
    "py",
    "pyi",
    "pyw",
    "gyp",
    "gypi",
    "SConstruct",
    "Sconstruct",
    "sconstruct",
    "SConscript",
    "wsgi"
  );

  grammar->setIncreaseIndentPattern(u"^\\s*(class|def|elif|else|except|finally|for|if|try|with|while|async\\s+(def|for|with))\\b.*:\\s*$");
  grammar->setDecreaseIndentPattern(u"^\\s*(elif|else|except|finally)\\b.*:\\s*$");

  grammar->addScopes("module", "source.python");

  grammar->addScopes("comment", "comment.line");
  grammar->addScopes("string", "string.quoted");
  grammar->addScopes("escape_sequence", "constant.character.escape");
  grammar->addScopes("interpolation", "meta.embedded");
  grammar->addScopes("interpolation > \"{\"", "punctuation.section.embedded");
  grammar->addScopes("interpolation > \"}\"", "punctuation.section.embedded");

  grammar->addScopes("class_definition > identifier", "entity.name.type.class");
  grammar->addScopes("function_definition > identifier", "entity.name.function.definition");
  grammar->addScopes("call > identifier:nth-child(0)",
    TreeSitterGrammar::Match{
      u"^(abs|all|any|ascii|bin|bool|breakpoint|bytearray|bytes|callable|chr|classmethod|compile|complex|delattr|dict|dir|divmod|enumerate|eval|exec|filter|float|format|frozenset|getattr|globals|hasattr|hash|help|hex|id|input|int|isinstance|issubclass|iter|len|list|locals|map|max|memoryview|min|next|object|oct|open|ord|pow|print|property|range|repr|reversed|round|set|setattr|slice|sorted|staticmethod|str|sum|super|tuple|type|vars|zip|__import__)$",
      "support.function.call"
    },
    TreeSitterGrammar::Match{u"^[A-Z]", "support.type.contructor"},
    "entity.name.function.call"
  );
  grammar->addScopes("call > attribute > identifier:nth-child(2)", "entity.name.function");

  grammar->addScopes("identifier",
    TreeSitterGrammar::Match{
      u"^(BaseException|Exception|TypeError|StopAsyncIteration|StopIteration|ImportError|ModuleNotFoundError|OSError|ConnectionError|BrokenPipeError|ConnectionAbortedError|ConnectionRefusedError|ConnectionResetError|BlockingIOError|ChildProcessError|FileExistsError|FileNotFoundError|IsADirectoryError|NotADirectoryError|InterruptedError|PermissionError|ProcessLookupError|TimeoutError|EOFError|RuntimeError|RecursionError|NotImplementedError|NameError|UnboundLocalError|AttributeError|SyntaxError|IndentationError|TabError|LookupError|IndexError|KeyError|ValueError|UnicodeError|UnicodeEncodeError|UnicodeDecodeError|UnicodeTranslateError|AssertionError|ArithmeticError|FloatingPointError|OverflowError|ZeroDivisionError|SystemError|ReferenceError|BufferError|MemoryError|Warning|UserWarning|DeprecationWarning|PendingDeprecationWarning|SyntaxWarning|RuntimeWarning|FutureWarning|ImportWarning|UnicodeWarning|BytesWarning|ResourceWarning|GeneratorExit|SystemExit|KeyboardInterrupt)$",
      "support.type.exception"
    },
    TreeSitterGrammar::Match{u"^(self)", "entity.name.variable.self"}
  );

  grammar->addScopes("attribute > identifier:nth-child(2)", "variable.other.object.property");

  grammar->addScopes("decorator", "entity.name.function.decorator");

  grammar->addScopes("none", "constant.language");
  grammar->addScopes("true", "constant.language");
  grammar->addScopes("false", "constant.language");
  grammar->addScopes("integer", "constant.numeric");
  grammar->addScopes("float", "constant.numeric");

  grammar->addScopes("type > identifier", "support.storage.type");

  grammar->addScopes("class_definition > argument_list > attribute", "entity.other.inherited-class");
  grammar->addScopes("class_definition > argument_list > identifier", "entity.other.inherited-class");
  grammar->addScopes("class_definition > argument_list > keyword_argument > attribute", "entity.other.inherited-class");
  grammar->addScopes("class_definition > argument_list > keyword_argument > identifier:nth-child(2)", "entity.other.inherited-class");

  grammar->addScopes("\"class\"", "storage.type.class");
  grammar->addScopes("\"def\"", "storage.type.function");
  grammar->addScopes("\"lambda\"", "storage.type.function");

  grammar->addScopes("\"global\"", "storage.modifier.global");
  grammar->addScopes("\"nonlocal\"", "storage.modifier.nonlocal");

  grammar->addScopes("parameters > identifier", "variable.parameter.function");
  grammar->addScopes("parameters > list_splat > identifier", "variable.parameter.function");
  grammar->addScopes("parameters > dictionary_splat > identifier", "variable.parameter.function");
  grammar->addScopes("default_parameter > identifier:nth-child(0)", "variable.parameter.function");
  grammar->addScopes("keyword_argument > identifier:nth-child(0)", "variable.parameter.function");
  grammar->addScopes("lambda_parameters > identifier", "variable.parameter.function");
  grammar->addScopes("typed_parameter > identifier", "variable.parameter.function");

  grammar->addScopes("argument_list", "meta.method-call.python");

  grammar->addScopes("\"if\"", "keyword.control");
  grammar->addScopes("\"else\"", "keyword.control");
  grammar->addScopes("\"elif\"", "keyword.control");
  grammar->addScopes("\"while\"", "keyword.control");
  grammar->addScopes("\"for\"", "keyword.control");
  grammar->addScopes("\"return\"", "keyword.control");
  grammar->addScopes("\"break\"", "keyword.control");
  grammar->addScopes("\"continue\"", "keyword.control");
  grammar->addScopes("\"pass\"", "keyword.control");
  grammar->addScopes("\"raise\"", "keyword.control");
  grammar->addScopes("\"yield\"", "keyword.control");
  grammar->addScopes("\"await\"", "keyword.control");
  grammar->addScopes("\"async\"", "keyword.control");
  grammar->addScopes("\"try\"", "keyword.control");
  grammar->addScopes("\"except\"", "keyword.control");
  grammar->addScopes("\"with\"", "keyword.control");
  grammar->addScopes("\"as\"", "keyword.control");
  grammar->addScopes("\"finally\"", "keyword.control");
  grammar->addScopes("\"import\"", "keyword.control");
  grammar->addScopes("\"from\"", "keyword.control");

  grammar->addScopes("\"print\"", "keyword.other");
  grammar->addScopes("\"assert\"", "keyword.other");
  grammar->addScopes("\"exec\"", "keyword.other");
  grammar->addScopes("\"del\"", "keyword.other");

  grammar->addScopes("\"+\"", "keyword.operator");
  grammar->addScopes("\"-\"", "keyword.operator");
  grammar->addScopes("\"*\"", "keyword.operator");
  grammar->addScopes("\"/\"", "keyword.operator");
  grammar->addScopes("\"%\"", "keyword.operator");
  grammar->addScopes("\"**\"", "keyword.operator");
  grammar->addScopes("\"//\"", "keyword.operator");
  grammar->addScopes("\"==\"", "keyword.operator");
  grammar->addScopes("\"!=\"", "keyword.operator");
  grammar->addScopes("\"<>\"", "keyword.operator");
  grammar->addScopes("\">\"", "keyword.operator");
  grammar->addScopes("\"<\"", "keyword.operator");
  grammar->addScopes("\">=\"", "keyword.operator");
  grammar->addScopes("\"<=\"", "keyword.operator");
  grammar->addScopes("\"=\"", "keyword.operator");
  grammar->addScopes("\"+=\"", "keyword.operator");
  grammar->addScopes("\"-=\"", "keyword.operator");
  grammar->addScopes("\"*=\"", "keyword.operator");
  grammar->addScopes("\"/=\"", "keyword.operator");
  grammar->addScopes("\"%=\"", "keyword.operator");
  grammar->addScopes("\"**=\"", "keyword.operator");
  grammar->addScopes("\"//=\"", "keyword.operator");
  grammar->addScopes("\"&\"", "keyword.operator");
  grammar->addScopes("\"|\"", "keyword.operator");
  grammar->addScopes("\"^\"", "keyword.operator");
  grammar->addScopes("\"~\"", "keyword.operator");
  grammar->addScopes("\"<<\"", "keyword.operator");
  grammar->addScopes("\">>\"", "keyword.operator");
  grammar->addScopes("binary_operator > \"@\"", "keyword.operator");
  grammar->addScopes("binary_operator > \"@=\"", "keyword.operator");
  grammar->addScopes("\"in\"", "keyword.operator.logical.python");
  grammar->addScopes("\"and\"", "keyword.operator.logical.python");
  grammar->addScopes("\"or\"", "keyword.operator.logical.python");
  grammar->addScopes("\"not\"", "keyword.operator.logical.python");
  grammar->addScopes("\"is\"", "keyword.operator.logical.python");
  grammar->addScopes("\"->\"", "keyword.control.return");

  grammar->addScopes("\"[\"", "punctuation.definition.begin.bracket.square");
  grammar->addScopes("\"]\"", "punctuation.definition.end.bracket.square");
  grammar->addScopes("\",\"", "punctuation.separator.delimiter");
  grammar->addScopes("\"{\"", "punctuation.section.block.begin.bracket.curly");
  grammar->addScopes("\"}\"", "punctuation.section.block.end.bracket.curly");
  grammar->addScopes("\"(\"", "punctuation.section.parens.begin.bracket.round");
  grammar->addScopes("\")\"", "punctuation.section.parens.end.bracket.round");

  return grammar;
}
