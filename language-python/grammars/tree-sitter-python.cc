#include <tree-sitter-grammar.h>

extern "C" const TSLanguage *tree_sitter_python();

extern "C" TreeSitterGrammar *atom_language_python() {
  TreeSitterGrammar *grammar = new TreeSitterGrammar(
    "Python",
    "source.python",
    tree_sitter_python()
  );

  grammar->addFileType("py");
  grammar->addFileType("pyi");
  grammar->addFileType("pyw");
  grammar->addFileType("gyp");
  grammar->addFileType("gypi");
  grammar->addFileType("SConstruct");
  grammar->addFileType("Sconstruct");
  grammar->addFileType("sconstruct");
  grammar->addFileType("SConscript");
  grammar->addFileType("wsgi");

  grammar->addScope("module", "source.python");

  grammar->addScope("comment", "comment.line");
  grammar->addScope("string", "string.quoted");
  grammar->addScope("escape_sequence", "constant.character.escape");
  grammar->addScope("interpolation", "meta.embedded");
  grammar->addScope("interpolation > \"{\"", "punctuation.section.embedded");
  grammar->addScope("interpolation > \"}\"", "punctuation.section.embedded");

  grammar->addScope("class_definition > identifier", "entity.name.type.class");
  grammar->addScope("function_definition > identifier", "entity.name.function.definition");
  /*'call > identifier:nth-child(0)': [
    {match: '^(abs|all|any|ascii|bin|bool|breakpoint|bytearray|bytes|callable|chr|classmethod|compile|complex|delattr|dict|dir|divmod|enumerate|eval|exec|filter|float|format|frozenset|getattr|globals|hasattr|hash|help|hex|id|input|int|isinstance|issubclass|iter|len|list|locals|map|max|memoryview|min|next|object|oct|open|ord|pow|print|property|range|repr|reversed|round|set|setattr|slice|sorted|staticmethod|str|sum|super|tuple|type|vars|zip|__import__)$',
    scopes: 'support.function.call'},
    {match: '^[A-Z]', scopes: 'support.type.contructor'}
    'entity.name.function.call'
   ]*/
  grammar->addScope("call > attribute > identifier:nth-child(2)", "entity.name.function");

  /*'identifier': [
    {match:
      '^(BaseException|Exception|TypeError|StopAsyncIteration|StopIteration|ImportError|ModuleNotFoundError|OSError|ConnectionError|BrokenPipeError|ConnectionAbortedError|ConnectionRefusedError|ConnectionResetError|BlockingIOError|ChildProcessError|FileExistsError|FileNotFoundError|IsADirectoryError|NotADirectoryError|InterruptedError|PermissionError|ProcessLookupError|TimeoutError|EOFError|RuntimeError|RecursionError|NotImplementedError|NameError|UnboundLocalError|AttributeError|SyntaxError|IndentationError|TabError|LookupError|IndexError|KeyError|ValueError|UnicodeError|UnicodeEncodeError|UnicodeDecodeError|UnicodeTranslateError|AssertionError|ArithmeticError|FloatingPointError|OverflowError|ZeroDivisionError|SystemError|ReferenceError|BufferError|MemoryError|Warning|UserWarning|DeprecationWarning|PendingDeprecationWarning|SyntaxWarning|RuntimeWarning|FutureWarning|ImportWarning|UnicodeWarning|BytesWarning|ResourceWarning|GeneratorExit|SystemExit|KeyboardInterrupt)$'
    scopes: 'support.type.exception'},
    {match: '^(self)', scopes: 'entity.name.variable.self'}
  ]*/

  grammar->addScope("attribute > identifier:nth-child(2)", "variable.other.object.property");

  grammar->addScope("decorator", "entity.name.function.decorator");

  grammar->addScope("none", "constant.language");
  grammar->addScope("true", "constant.language");
  grammar->addScope("false", "constant.language");
  grammar->addScope("integer", "constant.numeric");
  grammar->addScope("float", "constant.numeric");

  grammar->addScope("type > identifier", "support.storage.type");

  grammar->addScope("class_definition > argument_list > attribute", "entity.other.inherited-class");
  grammar->addScope("class_definition > argument_list > identifier", "entity.other.inherited-class");
  grammar->addScope("class_definition > argument_list > keyword_argument > attribute", "entity.other.inherited-class");
  grammar->addScope("class_definition > argument_list > keyword_argument > identifier:nth-child(2)", "entity.other.inherited-class");

  grammar->addScope("\"class\"", "storage.type.class");
  grammar->addScope("\"def\"", "storage.type.function");
  grammar->addScope("\"lambda\"", "storage.type.function");

  grammar->addScope("\"global\"", "storage.modifier.global");
  grammar->addScope("\"nonlocal\"", "storage.modifier.nonlocal");

  grammar->addScope("parameters > identifier", "variable.parameter.function");
  grammar->addScope("parameters > list_splat > identifier", "variable.parameter.function");
  grammar->addScope("parameters > dictionary_splat > identifier", "variable.parameter.function");
  grammar->addScope("default_parameter > identifier:nth-child(0)", "variable.parameter.function");
  grammar->addScope("keyword_argument > identifier:nth-child(0)", "variable.parameter.function");
  grammar->addScope("lambda_parameters > identifier", "variable.parameter.function");
  grammar->addScope("typed_parameter > identifier", "variable.parameter.function");

  grammar->addScope("argument_list", "meta.method-call.python");

  grammar->addScope("\"if\"", "keyword.control");
  grammar->addScope("\"else\"", "keyword.control");
  grammar->addScope("\"elif\"", "keyword.control");
  grammar->addScope("\"while\"", "keyword.control");
  grammar->addScope("\"for\"", "keyword.control");
  grammar->addScope("\"return\"", "keyword.control");
  grammar->addScope("\"break\"", "keyword.control");
  grammar->addScope("\"continue\"", "keyword.control");
  grammar->addScope("\"pass\"", "keyword.control");
  grammar->addScope("\"raise\"", "keyword.control");
  grammar->addScope("\"yield\"", "keyword.control");
  grammar->addScope("\"await\"", "keyword.control");
  grammar->addScope("\"async\"", "keyword.control");
  grammar->addScope("\"try\"", "keyword.control");
  grammar->addScope("\"except\"", "keyword.control");
  grammar->addScope("\"with\"", "keyword.control");
  grammar->addScope("\"as\"", "keyword.control");
  grammar->addScope("\"finally\"", "keyword.control");
  grammar->addScope("\"import\"", "keyword.control");
  grammar->addScope("\"from\"", "keyword.control");

  grammar->addScope("\"print\"", "keyword.other");
  grammar->addScope("\"assert\"", "keyword.other");
  grammar->addScope("\"exec\"", "keyword.other");
  grammar->addScope("\"del\"", "keyword.other");

  grammar->addScope("\"+\"", "keyword.operator");
  grammar->addScope("\"-\"", "keyword.operator");
  grammar->addScope("\"*\"", "keyword.operator");
  grammar->addScope("\"/\"", "keyword.operator");
  grammar->addScope("\"%\"", "keyword.operator");
  grammar->addScope("\"**\"", "keyword.operator");
  grammar->addScope("\"//\"", "keyword.operator");
  grammar->addScope("\"==\"", "keyword.operator");
  grammar->addScope("\"!=\"", "keyword.operator");
  grammar->addScope("\"<>\"", "keyword.operator");
  grammar->addScope("\">\"", "keyword.operator");
  grammar->addScope("\"<\"", "keyword.operator");
  grammar->addScope("\">=\"", "keyword.operator");
  grammar->addScope("\"<=\"", "keyword.operator");
  grammar->addScope("\"=\"", "keyword.operator");
  grammar->addScope("\"+=\"", "keyword.operator");
  grammar->addScope("\"-=\"", "keyword.operator");
  grammar->addScope("\"*=\"", "keyword.operator");
  grammar->addScope("\"/=\"", "keyword.operator");
  grammar->addScope("\"%=\"", "keyword.operator");
  grammar->addScope("\"**=\"", "keyword.operator");
  grammar->addScope("\"//=\"", "keyword.operator");
  grammar->addScope("\"&\"", "keyword.operator");
  grammar->addScope("\"|\"", "keyword.operator");
  grammar->addScope("\"^\"", "keyword.operator");
  grammar->addScope("\"~\"", "keyword.operator");
  grammar->addScope("\"<<\"", "keyword.operator");
  grammar->addScope("\">>\"", "keyword.operator");
  grammar->addScope("binary_operator > \"@\"", "keyword.operator");
  grammar->addScope("binary_operator > \"@=\"", "keyword.operator");
  grammar->addScope("\"in\"", "keyword.operator.logical.python");
  grammar->addScope("\"and\"", "keyword.operator.logical.python");
  grammar->addScope("\"or\"", "keyword.operator.logical.python");
  grammar->addScope("\"not\"", "keyword.operator.logical.python");
  grammar->addScope("\"is\"", "keyword.operator.logical.python");
  grammar->addScope("\"->\"", "keyword.control.return");

  grammar->addScope("\"[\"", "punctuation.definition.begin.bracket.square");
  grammar->addScope("\"]\"", "punctuation.definition.end.bracket.square");
  grammar->addScope("\",\"", "punctuation.separator.delimiter");
  grammar->addScope("\"{\"", "punctuation.section.block.begin.bracket.curly");
  grammar->addScope("\"}\"", "punctuation.section.block.end.bracket.curly");
  grammar->addScope("\"(\"", "punctuation.section.parens.begin.bracket.round");
  grammar->addScope("\")\"", "punctuation.section.parens.end.bracket.round");

  return grammar;
}
