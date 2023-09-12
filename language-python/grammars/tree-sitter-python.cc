#include <tree-sitter-grammar.h>

extern "C" const TSLanguage *tree_sitter_python();

using namespace TreeSitterGrammarDSL;

extern "C" TreeSitterGrammar *atom_language_python() {
  TreeSitterGrammar *grammar = new TreeSitterGrammar(
    "Python",
    "source.python",
    tree_sitter_python()
  );

  grammar->setFileTypes(
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

  grammar->setScopes(
    scope("module", "source.python"),

    scope("comment", "comment.line"),
    scope("string", "string.quoted"),
    scope("escape_sequence", "constant.character.escape"),
    scope("interpolation", "meta.embedded"),
    scope("interpolation > \"{\"", "punctuation.section.embedded"),
    scope("interpolation > \"}\"", "punctuation.section.embedded"),

    scope("class_definition > identifier", "entity.name.type.class"),
    scope("function_definition > identifier", "entity.name.function.definition"),
    scope("call > identifier:nth-child(0)", array(
      match(
        u"^(abs|all|any|ascii|bin|bool|breakpoint|bytearray|bytes|callable|chr|classmethod|compile|complex|delattr|dict|dir|divmod|enumerate|eval|exec|filter|float|format|frozenset|getattr|globals|hasattr|hash|help|hex|id|input|int|isinstance|issubclass|iter|len|list|locals|map|max|memoryview|min|next|object|oct|open|ord|pow|print|property|range|repr|reversed|round|set|setattr|slice|sorted|staticmethod|str|sum|super|tuple|type|vars|zip|__import__)$",
        "support.function.call"
      ),
      match(u"^[A-Z]", "support.type.contructor"),
      "entity.name.function.call"
    )),
    scope("call > attribute > identifier:nth-child(2)", "entity.name.function"),

    scope("identifier", array(
      match(
        u"^(BaseException|Exception|TypeError|StopAsyncIteration|StopIteration|ImportError|ModuleNotFoundError|OSError|ConnectionError|BrokenPipeError|ConnectionAbortedError|ConnectionRefusedError|ConnectionResetError|BlockingIOError|ChildProcessError|FileExistsError|FileNotFoundError|IsADirectoryError|NotADirectoryError|InterruptedError|PermissionError|ProcessLookupError|TimeoutError|EOFError|RuntimeError|RecursionError|NotImplementedError|NameError|UnboundLocalError|AttributeError|SyntaxError|IndentationError|TabError|LookupError|IndexError|KeyError|ValueError|UnicodeError|UnicodeEncodeError|UnicodeDecodeError|UnicodeTranslateError|AssertionError|ArithmeticError|FloatingPointError|OverflowError|ZeroDivisionError|SystemError|ReferenceError|BufferError|MemoryError|Warning|UserWarning|DeprecationWarning|PendingDeprecationWarning|SyntaxWarning|RuntimeWarning|FutureWarning|ImportWarning|UnicodeWarning|BytesWarning|ResourceWarning|GeneratorExit|SystemExit|KeyboardInterrupt)$",
        "support.type.exception"
      ),
      match(u"^(self)", "entity.name.variable.self")
    )),

    scope("attribute > identifier:nth-child(2)", "variable.other.object.property"),

    scope("decorator", "entity.name.function.decorator"),

    scope("none", "constant.language"),
    scope("true", "constant.language"),
    scope("false", "constant.language"),
    scope("integer", "constant.numeric"),
    scope("float", "constant.numeric"),

    scope("type > identifier", "support.storage.type"),

    scope("class_definition > argument_list > attribute", "entity.other.inherited-class"),
    scope("class_definition > argument_list > identifier", "entity.other.inherited-class"),
    scope("class_definition > argument_list > keyword_argument > attribute", "entity.other.inherited-class"),
    scope("class_definition > argument_list > keyword_argument > identifier:nth-child(2)", "entity.other.inherited-class"),

    scope("\"class\"", "storage.type.class"),
    scope("\"def\"", "storage.type.function"),
    scope("\"lambda\"", "storage.type.function"),

    scope("\"global\"", "storage.modifier.global"),
    scope("\"nonlocal\"", "storage.modifier.nonlocal"),

    scope("parameters > identifier", "variable.parameter.function"),
    scope("parameters > list_splat > identifier", "variable.parameter.function"),
    scope("parameters > dictionary_splat > identifier", "variable.parameter.function"),
    scope("default_parameter > identifier:nth-child(0)", "variable.parameter.function"),
    scope("keyword_argument > identifier:nth-child(0)", "variable.parameter.function"),
    scope("lambda_parameters > identifier", "variable.parameter.function"),
    scope("typed_parameter > identifier", "variable.parameter.function"),

    scope("argument_list", "meta.method-call.python"),

    scope("\"if\"", "keyword.control"),
    scope("\"else\"", "keyword.control"),
    scope("\"elif\"", "keyword.control"),
    scope("\"while\"", "keyword.control"),
    scope("\"for\"", "keyword.control"),
    scope("\"return\"", "keyword.control"),
    scope("\"break\"", "keyword.control"),
    scope("\"continue\"", "keyword.control"),
    scope("\"pass\"", "keyword.control"),
    scope("\"raise\"", "keyword.control"),
    scope("\"yield\"", "keyword.control"),
    scope("\"await\"", "keyword.control"),
    scope("\"async\"", "keyword.control"),
    scope("\"try\"", "keyword.control"),
    scope("\"except\"", "keyword.control"),
    scope("\"with\"", "keyword.control"),
    scope("\"as\"", "keyword.control"),
    scope("\"finally\"", "keyword.control"),
    scope("\"import\"", "keyword.control"),
    scope("\"from\"", "keyword.control"),

    scope("\"print\"", "keyword.other"),
    scope("\"assert\"", "keyword.other"),
    scope("\"exec\"", "keyword.other"),
    scope("\"del\"", "keyword.other"),

    scope("\"+\"", "keyword.operator"),
    scope("\"-\"", "keyword.operator"),
    scope("\"*\"", "keyword.operator"),
    scope("\"/\"", "keyword.operator"),
    scope("\"%\"", "keyword.operator"),
    scope("\"**\"", "keyword.operator"),
    scope("\"//\"", "keyword.operator"),
    scope("\"==\"", "keyword.operator"),
    scope("\"!=\"", "keyword.operator"),
    scope("\"<>\"", "keyword.operator"),
    scope("\">\"", "keyword.operator"),
    scope("\"<\"", "keyword.operator"),
    scope("\">=\"", "keyword.operator"),
    scope("\"<=\"", "keyword.operator"),
    scope("\"=\"", "keyword.operator"),
    scope("\"+=\"", "keyword.operator"),
    scope("\"-=\"", "keyword.operator"),
    scope("\"*=\"", "keyword.operator"),
    scope("\"/=\"", "keyword.operator"),
    scope("\"%=\"", "keyword.operator"),
    scope("\"**=\"", "keyword.operator"),
    scope("\"//=\"", "keyword.operator"),
    scope("\"&\"", "keyword.operator"),
    scope("\"|\"", "keyword.operator"),
    scope("\"^\"", "keyword.operator"),
    scope("\"~\"", "keyword.operator"),
    scope("\"<<\"", "keyword.operator"),
    scope("\">>\"", "keyword.operator"),
    scope("binary_operator > \"@\"", "keyword.operator"),
    scope("binary_operator > \"@=\"", "keyword.operator"),
    scope("\"in\"", "keyword.operator.logical.python"),
    scope("\"and\"", "keyword.operator.logical.python"),
    scope("\"or\"", "keyword.operator.logical.python"),
    scope("\"not\"", "keyword.operator.logical.python"),
    scope("\"is\"", "keyword.operator.logical.python"),
    scope("\"->\"", "keyword.control.return"),

    scope("\"[\"", "punctuation.definition.begin.bracket.square"),
    scope("\"]\"", "punctuation.definition.end.bracket.square"),
    scope("\",\"", "punctuation.separator.delimiter"),
    scope("\"{\"", "punctuation.section.block.begin.bracket.curly"),
    scope("\"}\"", "punctuation.section.block.end.bracket.curly"),
    scope("\"(\"", "punctuation.section.parens.begin.bracket.round"),
    scope("\")\"", "punctuation.section.parens.end.bracket.round")
  );

  return grammar;
}
