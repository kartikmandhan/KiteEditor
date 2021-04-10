#ifndef SYNTAXHL_H
#define SYNTAXHL_H
#include "editor.h"
char *c_extensions[] = {".c", ".h", ".cpp", NULL};
char *java_extensions[] = {".java", NULL};
char *python_extensions[] = {".py", NULL};
char *javascript_extensions[] = {".js", ".jsx", NULL};

char *c_keywords[] = {
    "switch", "if", "while", "for", "break", "continue", "return", "else",
    "struct", "union", "typedef", "static", "enum", "class", "case", "#include",
    "volatile", "register", "sizeof", "typedef", "union", "goto", "const", "auto",
    "#define", "#if", "#endif", "#error", "#ifdef", "#ifndef", "#undef",
    "int", "long", "double", "float", "char", "unsigned", "signed", "malloc", "calloc", "void",
    NULL};
char *java_keywords[] = {
    "switch", "if", "while", "for", "break", "continue", "return", "else",
    "in", "public", "private", "protected", "static", "final", "abstract",
    "enum", "class", "case", "try", "catch", "do", "extends", "implements",
    "finally", "import", "instanceof", "interface", "new", "package", "super",
    "native", "strictfp", "synchronized", "this", "throw", "throws", "transient", "volatile",
    "byte", "char", "double", "float", "int", "long", "short",
    "boolean", NULL};
char *python_keywords[] = {
    "and", "as", "assert", "break", "class", "continue", "def", "del", "elif",
    "else", "except", "exec", "finally", "for", "from", "global", "if", "import",
    "in", "is", "lambda", "not", "or", "pass", "print", "raise", "return", "try",
    "while", "with", "yield",

    "buffer", "bytearray", "complex", "False", "float", "frozenset", "int",
    "list", "long", "None", "set", "str", "tuple", "True", "type",
    "unicode", "xrange", NULL};
char *javascript_keywords[] = {
    "break", "case", "catch", "class", "const", "continue", "debugger", "default",
    "delete", "do", "else", "enum", "export", "extends", "finally", "for", "function",
    "if", "implements", "import", "in", "instanceof", "interface", "let", "new",
    "package", "private", "protected", "public", "return", "static", "super", "switch",
    "this", "throw", "try", "typeof", "var", "while", "with", "yield", "true",
    "false", "null", "NaN", "global", "window", "prototype", "constructor", "document",
    "isNaN", "arguments", "undefined", "Infinity", "Array", "Object", "Number", "String", "Boolean", "Function",
    "ArrayBuffer", "DataView", "Float32Array", "Float64Array", "Int8Array",
    "Int16Array", "Int32Array", "Uint8Array", "Uint8ClampedArray", "Uint32Array",
    "Date", "Error", "Map", "RegExp", "Symbol", "WeakMap", "WeakSet", "Set", NULL};
// highlighting database
editorSyntax syntaxDB[] = {
    // filetype
    {"c/cpp",
     // filematch
     c_extensions,
     //  singleline comment starter
     "//",
     //  list of keywords
     c_keywords,
     // flags
     HIGHLIGHT_NUMBERS | HIGHLIGHT_STRINGS},
    {"java", java_extensions, "//", java_keywords, HIGHLIGHT_NUMBERS | HIGHLIGHT_STRINGS},
    {"python", python_extensions, "#", python_keywords, HIGHLIGHT_NUMBERS | HIGHLIGHT_STRINGS},
    {"js", javascript_extensions, "//", javascript_keywords, HIGHLIGHT_NUMBERS | HIGHLIGHT_STRINGS},
};
#endif // !SYNTAXHL_H