//  SwiftFlatBuffers is licensed under the Apache License, Version 2.0, From FlatBuffers.
//
//  FlatBuffers         <https://github.com/google/flatbuffers>
//  SwiftFlatBuffers    <https://github.com/TonyStark106/SwiftFlatBuffers>
//
//  Created by TonyStark106 on 2017/5/9.
//

#include <string>

#include "flatbuffers/flatbuffers.h"
#include "flatbuffers/idl.h"
#include "flatbuffers/util.h"
#include "flatbuffers/code_generators.h"

#include <iostream>

namespace flatbuffers {
    namespace swift {
        struct LanguageParameters {
            const char *file_explain;
            const char *file_name_pre;
            const char *file_name_suff;
            const char *file_extension;
            const char *open_curly;
            const char *close_curly;
            const char *internal_static;
            const char *struct_decl;
            const char *struct_field_decl;
            const char *struct_field_decl_fileprivate;
            const char *struct_field_decl_private;
            const char *struct_field_decl_override;
            const char *struct_func_private;
            const char *enum_decl;
            const char *enum_field;
            const char *indent;
            const char *comment_prefix;
            const char *colon;
            const char *union_type;
        };
       
        static bool should_gen_namespace = true;
        const std::string framework_name = "SwiftFlatBuffers";
        const int swift_keywords_size = 54;
        extern const std::string swift_keywords[];
        
        static const struct LanguageParameters language_parameters = {
            "//  Automatically generated by the FlatBuffers compiler, do not modify.\
            \n//  SwiftFlatBuffers, licensed under the Apache License, Version 2.0, is from FlatBuffers.\
            \n//\
            \n//  SwiftFlatBuffers  <https://github.com/TonyStark106/SwiftFlatBuffers>\
            \n//",
            "",
            "_generated",
            "swift",
            " {\n",
            "}\n",
            "internal static ",
            "public class ",
            "public lazy var ",
            "fileprivate lazy var ",
            "private lazy var ",
            "public override var ",
            "private func _add_",
            "public enum ",
            "case ",
            "    ",
            "// MARK: ",
            ": ",
            "type"
        };
        
        static std::string GeneratedFileName(const std::string &path,
                                             const std::string &file_name);
        
        static void GenComment(const std::vector<std::string> &dc,
                               std::string *code_ptr,
                               const int indentCount = 0);
        
        static std::string GenTypeBasic(const BaseType &type);
        
        static std::string GenTypePointer(const Type &type);
        
        static std::string GenType(const Type &type);
        
        static std::string checkKeywods(std::string var_name);
        
        static void GenGetterMethod(const FieldDef &field_def,
                              std::string *code_ptr);
        
        static void GenEnum(const EnumDef &enum_,
                            std::string *code_ptr);
        
        static void GenStruct(const StructDef &st,
                              std::string *code_ptr);
        
        static size_t GenStructField(const FieldDef &field_def,
                                     const std::string struct_name,
                                     std::string *code_ptr,
                                     std::string *extension_code_ptr,
                                     std::string *union_type_name,
                                     const size_t offset);
        
        static void GenStructHardCode(const StructDef &struct_def,
                                      const size_t byteSize,
                                      std::string *code_ptr);
        
        static void upper(std::string &s);
        
        static std::string namespace_ifneeded(const std::string name, const Namespace *ns);
        
        class SwiftGenerator: public BaseGenerator {
            public:
            SwiftGenerator(const Parser &parser,
                           const std::string &path,
                           const std::string &file_name)
            : BaseGenerator(parser, path, file_name, "", "::") {};
            
            void GenEnums(std::string *code_ptr) {
                for (auto it = parser_.enums_.vec.begin(); it != parser_.enums_.vec.end();
                     ++it) {
                    GenEnum(**it, code_ptr);
                }
            }
            
            void GenStructs(std::string *code_ptr) {
                for (auto it = parser_.structs_.vec.begin(); it != parser_.structs_.vec.end();
                     ++it) {
                    GenStruct(**it, code_ptr);
                }
            }
            
            bool generate() {
                std::string code = "";
                code += language_parameters.file_explain;
                code += "\n\n";
                code += "import " + framework_name + "\n\n";
                GenEnums(&code);
                GenStructs(&code);
                std::string file_name = GeneratedFileName(path_, file_name_);
                SaveFile(file_name.c_str(), code, false);
                return true;
            }
        };
        
        static std::string GeneratedFileName(const std::string &path,
                                             const std::string &file_name) {
            std::string _file_name = language_parameters.file_name_pre + file_name + language_parameters.file_name_suff + "." + language_parameters.file_extension;
            return _file_name;
        }
        
        static void GenComment(const std::vector<std::string> &dc,
                               std::string *code_ptr,
                               int indentCount) {
            std::string &code = *code_ptr;
            if (dc.begin() == dc.end()) {
                return;
            }
            for (int i = 0; i < indentCount; i++) {
                code += language_parameters.indent;
            }
            code += language_parameters.comment_prefix;
            for (auto it = dc.begin();
                 it != dc.end();
                 ++it) {
                code += *it + " ";
            }
            code += "\n";
        }
        
        static void GenIndent(std::string *code_ptr, int num);
        
        static size_t GenStructField(const FieldDef &field_def,
                                     const std::string struct_name,
                                     std::string *code_ptr,
                                     std::string *extension_code_ptr,
                                     std::string *union_type_name,
                                     const size_t offset) {
            std::string &code = *code_ptr;
            std::string &extension_code = *extension_code_ptr;
            std::string field_name = checkKeywods(field_def.name);
            // first line
            code += language_parameters.indent;
            
            code += field_def.value.type.base_type == BASE_TYPE_UTYPE ?
            language_parameters.struct_field_decl_fileprivate :
            language_parameters.struct_field_decl;
            
            code += field_name + language_parameters.colon;
            BaseType baseType = field_def.value.type.base_type;
            if (baseType == BASE_TYPE_UTYPE) {
                code += "FBTable.Type";
            } else if (baseType == BASE_TYPE_UNION) {
                code += "FBTable";
            } else {
                code += GenType(field_def.value.type);
            };
            auto value_type = field_def.value.type;
            if (value_type.enum_def != nullptr ||
                value_type.base_type == BASE_TYPE_VECTOR ||
                value_type.base_type == BASE_TYPE_STRUCT ||
                value_type.base_type == BASE_TYPE_STRING ||
                value_type.base_type == BASE_TYPE_UNION) {
                code += "?";
            }
            code +=  " =";
            code += language_parameters.open_curly;
            // second line
            code += language_parameters.indent;
            code += language_parameters.indent;
            
            EnumDef *enum_def = field_def.value.type.enum_def;
            if (baseType == BASE_TYPE_UTYPE) {
                if (enum_def == nullptr) {
                    assert(0);
                }
                code += "return " +
                        checkKeywods(namespace_ifneeded(enum_def->name, enum_def->defined_namespace)) +
                        "(rawValue: self.getUInt8(vOffset: " +
                        std::to_string(field_def.value.offset) +
                        "))?." +
                        language_parameters.union_type
                        + "\n";
            } else if (baseType == BASE_TYPE_UNION) {
                if (enum_def == nullptr) {
                    assert(0);
                }
                code += "return self." +
                        field_def.name +
                        "_type != nil ? self.getTable(type: self." +
                        field_def.name +
                        "_type!, vOffset: " +
                        std::to_string(field_def.value.offset) +
                        ") : nil\n";
            } else {
                GenGetterMethod(field_def, code_ptr);
            }
            code += language_parameters.indent;
            code += "}()";
            code += "\n\n";
            
            // set
            if (field_def.value.type.base_type != BASE_TYPE_UTYPE) {
                extension_code += language_parameters.indent;
                extension_code += "@objc";
                extension_code += "\n";
                extension_code += language_parameters.indent;
                extension_code += language_parameters.struct_func_private;
                extension_code += field_def.name;
                extension_code += "(i: ";
                extension_code += struct_name;
                extension_code += ")";
                extension_code += language_parameters.open_curly;
                
                if (field_def.value.type.base_type == BASE_TYPE_UNION) {
                    extension_code += language_parameters.indent;
                    extension_code += language_parameters.indent;
                    extension_code += "set(vOffset: ";
                    extension_code += NumToString(field_def.value.offset - 2);
                    extension_code += ", pOffset: ";
                    extension_code += NumToString(offset + 4 - 1);
                    extension_code += ", value: ";
                    
                    extension_code += *union_type_name;
                    extension_code += ".value(";
                    extension_code += "i.";
                    extension_code += field_name;
                    extension_code += "))\n";
                }
                
                extension_code += language_parameters.indent;
                extension_code += language_parameters.indent;
                extension_code += "set(vOffset: ";
                extension_code += NumToString(field_def.value.offset);
                extension_code += ", pOffset: ";
                extension_code += NumToString(offset + 4);
                extension_code += ", value: i.";
                extension_code += field_name;
                if (field_def.value.type.base_type != BASE_TYPE_UNION) {
                    if (field_def.value.type.enum_def != nullptr) {
                        extension_code += "?.rawValue";
                    }
                }
                extension_code += ")\n";
                
                extension_code += language_parameters.indent;
                extension_code += language_parameters.close_curly;
                extension_code += "\n";
            }
            
            return InlineSize(field_def.value.type);
        }
        
        static std::string GenTypeBasic(const BaseType &type) {
            std::string code = "";
            switch (type) {
                case BASE_TYPE_BOOL:
                    code += "Bool";
                    break;
                case BASE_TYPE_UCHAR:
                    code += "UInt8";
                    break;
                case BASE_TYPE_CHAR:
                    code += "Int8";
                    break;
                case BASE_TYPE_SHORT:
                    code += "Int16";
                    break;
                case BASE_TYPE_USHORT:
                    code += "UInt16";
                    break;
                case BASE_TYPE_INT:
                    code += "Int32";
                    break;
                case BASE_TYPE_UINT:
                    code += "UInt32";
                    break;
                case BASE_TYPE_LONG:
                    code += "Int64";
                    break;
                case BASE_TYPE_ULONG:
                    code += "UInt64";
                    break;
                case BASE_TYPE_FLOAT:
                    code += "Float32";
                    break;
                case BASE_TYPE_DOUBLE:
                    code += "Float64";
                    break;
                case BASE_TYPE_UTYPE:
                    code += "UInt8";
                    break;
                default:
                    break;
            }
            return code;
        }
        
        static std::string GenTypePointer(const Type &type) {
            std::string code = "";
            switch (type.base_type) {
                case BASE_TYPE_STRING:
                    code += "String";
                    break;
                case BASE_TYPE_VECTOR:
                {
                    auto element_type = type.VectorType();
                    code += "[";
                    code += GenType(element_type);
                    code += "]";
                    break;
                }
                case BASE_TYPE_STRUCT:
                {
                    code += checkKeywods(namespace_ifneeded(type.struct_def->name, type.struct_def->defined_namespace));
                    break;
                }
                case BASE_TYPE_UNION:
                default:
                    assert(0);
            }
            return code;
        }
        
        static std::string GenType(const Type &type) {
            if (type.enum_def != nullptr) {
                return checkKeywods(namespace_ifneeded(type.enum_def->name, type.enum_def->defined_namespace));
            }
            return IsScalar(type.base_type) ? GenTypeBasic(type.base_type) : GenTypePointer(type);
        }
        
        static void GenGetterMethod(const FieldDef &field_def,
                                    std::string *code_ptr) {
            std::string &code = *code_ptr;
            std::string temp;
            std::string local_var_name = "m_" + field_def.name;
            EnumDef *enum_def = field_def.value.type.enum_def;
            std::string enum_name;
            if (enum_def != nullptr) {
                enum_name = checkKeywods(namespace_ifneeded(field_def.value.type.enum_def->name, field_def.value.type.enum_def->defined_namespace));
                temp += enum_name + "(rawValue: self.get";
                temp += GenTypeBasic(field_def.value.type.base_type);
            } else {
                temp += "self.get";
                if (field_def.value.type.base_type == BASE_TYPE_VECTOR) {
                    Type vectorType = field_def.value.type.VectorType();
                    if (vectorType.base_type == BASE_TYPE_STRING) {
                        temp += "Strings";
                    } else if (vectorType.base_type == BASE_TYPE_STRUCT) {
                        temp += vectorType.struct_def->fixed ? "Structs" : "Tables";
                    } else {
                        temp += "Numbers";
                    }
                } else {
                    if (field_def.value.type.base_type == BASE_TYPE_STRUCT) {
                        temp += field_def.value.type.struct_def->fixed ? "Struct" : "Table";
                    } else if (field_def.value.type.base_type == BASE_TYPE_STRING) {
                        temp += "String";
                    } else {
                        temp += GenType(field_def.value.type);
                    }
                }
            }
            temp += "(";
            if (field_def.value.type.base_type == BASE_TYPE_VECTOR) {
                Type vectorType = field_def.value.type.VectorType();
                if (vectorType.base_type == BASE_TYPE_STRUCT) {
                    temp += "type: " + checkKeywods(namespace_ifneeded(vectorType.struct_def->name, vectorType.struct_def->defined_namespace)) + ".self, ";
                }
            }
            if (field_def.value.type.base_type == BASE_TYPE_STRUCT) {
                temp += "type: " + checkKeywods(namespace_ifneeded(field_def.value.type.struct_def->name, field_def.value.type.struct_def->defined_namespace) ) + ".self, ";
            }
            temp += "vOffset: ";
            temp += std::to_string(field_def.value.offset);
            if (field_def.value.type.base_type == BASE_TYPE_VECTOR) {
                Type vectorType = field_def.value.type.VectorType();
                if (vectorType.base_type == BASE_TYPE_STRUCT) {
                    if (vectorType.struct_def->fixed) {
                        temp += ", byteSize: " + NumToString(InlineSize(vectorType));
                    }
                }
            }
            temp += ")";
            if (enum_def != nullptr) {
                temp += ")";
            }
            bool has_constant = false;
            if (IsScalar(field_def.value.type.base_type)) {
                if (field_def.value.constant != "0" && field_def.value.constant != "0.0") {
                    has_constant = true;
                }
            }
            if (has_constant && field_def.value.type.base_type != BASE_TYPE_BOOL) {
                code += "let " + local_var_name + " = " + temp + "\n";
                code += language_parameters.indent;
                code += language_parameters.indent;
                if (enum_def != nullptr) {
                    std::string constant = checkKeywods(namespace_ifneeded(enum_def->name, enum_def->defined_namespace)) + "(rawValue: " + field_def.value.constant + ")!";
                    code += "return " + local_var_name + " ?? " + constant;
                } else {
                    code += "return " + local_var_name + " != 0 ? " + local_var_name + " : " + field_def.value.constant;
                }
            } else {
                code += "return " + temp;
            }
            code += "\n";
        }
        
        static std::string namespace_ifneeded(std::string name, const Namespace *ns) {
            if (should_gen_namespace == false) {
                return name;
            }
            if (ns == NULL) {
                return name;
            }
            
            std::string str = ns->GetFullyQualifiedName(name);
            while(true) {
                unsigned long pos = str.find(".", 0);
                if(pos < LONG_MAX) {
                    str.replace(pos, 1, "_");
                } else break;
            }
            return str;
        }
        
        static std::string checkKeywods(std::string var_name) {
            bool flag = false;
            for (int i = 0; i < swift_keywords_size; i++) {
                std::string keyword = swift_keywords[i];
                if (var_name == keyword) {
                    flag = true;
                    break;
                }
            }
            return flag ? "`" + var_name + "`" : var_name ;
        }
        
        static void GenEnum(const EnumDef &enum_def,
                            std::string *code_ptr) {            
            if (enum_def.generated) return;
            std::string &code = *code_ptr;
            GenComment(enum_def.doc_comment, code_ptr);
            std::string enum_name = checkKeywods(namespace_ifneeded(enum_def.name, enum_def.defined_namespace));
            code += language_parameters.enum_decl +
                    enum_name +
                    language_parameters.colon;
            code += GenTypeBasic(enum_def.underlying_type.base_type);
            code += language_parameters.open_curly;
            code += "\n";
            for (auto it = enum_def.vals.vec.begin();
                 it != enum_def.vals.vec.end();
                 ++it) {
                auto &ev = **it;
                GenComment(ev.doc_comment, code_ptr, 1);
                std::string &code = *code_ptr;
                code += language_parameters.indent;
                std::string ev_name = checkKeywods(ev.name);
                if (enum_def.underlying_type.base_type == BASE_TYPE_UTYPE) {
                    upper(ev_name);
                }
                code += language_parameters.enum_field + ev_name;
                code += " = ";
                code += NumToString(ev.value) + "\n\n";
            }
            if (enum_def.underlying_type.base_type == BASE_TYPE_UTYPE) {
                code += language_parameters.indent;
                code += "var type: FBTable.Type?";
                code += language_parameters.open_curly;
                code += language_parameters.indent;
                code += language_parameters.indent;
                code += "switch self";
                code += language_parameters.open_curly;
                std::vector<std::string> ev_name_upper_check;
                for (auto it = enum_def.vals.vec.begin();
                     it != enum_def.vals.vec.end();
                     ++it) {
                    auto &ev = **it;
                    std::string ev_name = checkKeywods(ev.name);
                    code += language_parameters.indent;
                    code += language_parameters.indent;
                    code += "case .";
                    upper(ev_name);
                    ev_name_upper_check.push_back(ev_name);
                    code += ev_name;
                    code += ":\n";
                    code += language_parameters.indent;
                    code += language_parameters.indent;
                    code += language_parameters.indent;
                    code += "return ";
                    code += ev.union_type.struct_def != NULL ? namespace_ifneeded(ev.union_type.struct_def->name, ev.union_type.struct_def->defined_namespace) + ".self" : "nil" ;
                    code += "\n";
                }
                code += language_parameters.indent;
                code += language_parameters.indent;
                code += language_parameters.close_curly;
                code += language_parameters.indent;
                code += language_parameters.close_curly;
                code += "\n";
                
                code += language_parameters.indent;
                code += language_parameters.internal_static;
                code += "let";
                code += " allValues = [";
                for (int i = 0; i < ev_name_upper_check.size(); i++) {
                    if (i != 0) {
                        code += ", ";
                    }
                    code += ev_name_upper_check.at(i);
                }
                code += "]";
                code += "\n\n";                
                code += language_parameters.indent;
                code += language_parameters.internal_static;
                code += "func value(_ ins: FBTable?) -> ";
                code += enum_name;
                code += language_parameters.open_curly;
                
                GenIndent(code_ptr, 2);
                code += "for value in allValues";
                code += language_parameters.open_curly;
                
                GenIndent(code_ptr, 3);
                code += "if ins?.classForCoder == value.type";
                code += language_parameters.open_curly;
                GenIndent(code_ptr, 4);
                code += "return value\n";
                GenIndent(code_ptr, 3);
                code += language_parameters.close_curly;
                GenIndent(code_ptr, 2);
                code += language_parameters.close_curly;
                GenIndent(code_ptr, 2);
                code += "fatalError(\"Can't not serialize \\(String(describing: ins)) to ";
                code += enum_name;
                code += "\")\n";
                
                code += language_parameters.indent;
                code += language_parameters.close_curly;
                code += "\n";
            }
            code += language_parameters.close_curly;
            code += "\n";
        }
        
        static void GenIndent(std::string *code_ptr, int num) {
            std::string &code = *code_ptr;
            for (int i = 0; i < num; i++) {
                code += language_parameters.indent;
            }
        }
        
        static void GenStruct(const StructDef &struct_def,
                              std::string *code_ptr) {
            if (struct_def.generated) return;
            std::string &code = *code_ptr;
            GenComment(struct_def.doc_comment, code_ptr);
            code += language_parameters.struct_decl +
                    checkKeywods(namespace_ifneeded(struct_def.name, struct_def.defined_namespace)) +
                    language_parameters.colon +
                    "FBTable" +
                    language_parameters.open_curly + "\n";
            // field
            size_t byteSize = 0;
            std::string extension_code_ptr = "";
            std::string union_type_name = "";
            for (auto it = struct_def.fields.vec.begin();
                 it != struct_def.fields.vec.end();
                 ++it) {
                auto &field = **it;
                if (field.deprecated) continue;
                GenComment(field.doc_comment, code_ptr, 1);
                
                byteSize += GenStructField(field,
                                           checkKeywods(namespace_ifneeded(struct_def.name, struct_def.defined_namespace)),
                                           code_ptr,
                                           &extension_code_ptr,
                                           &union_type_name,
                                           byteSize);
                
                if (field.value.type.base_type == BASE_TYPE_UTYPE) {
                    union_type_name = checkKeywods(namespace_ifneeded(field.value.type.enum_def->name, field.value.type.enum_def->defined_namespace));
                } else {
                    union_type_name = "";
                }
            }
            GenStructHardCode(struct_def, byteSize, code_ptr);
            code += language_parameters.close_curly;
            code += "\n";
            
            // Extension
            code += "extension ";
            code += checkKeywods(namespace_ifneeded(struct_def.name, struct_def.defined_namespace));
            code += language_parameters.open_curly;
            code += "\n";
            code += extension_code_ptr;
            code += language_parameters.close_curly;
            code += "\n";
        }
        
        static void GenStructHardCode(const StructDef &struct_def,
                                      const size_t byteSize,
                                      std::string *code_ptr) {
            std::string &code = *code_ptr;
            size_t pos = struct_def.fixed ? 0 : struct_def.fields.vec.size() * 2 + 10;
            size_t hardSize = struct_def.fixed ? struct_def.bytesize :  byteSize + pos + 4;
            code += language_parameters.indent;
            code += language_parameters.struct_field_decl_override;
            code += "hardPos: FBOffset";
            code += language_parameters.open_curly;
            code += language_parameters.indent;
            code += language_parameters.indent;
            code += "return ";
            code += NumToString(pos);
            code += "\n";
            code += language_parameters.indent;
            code += language_parameters.close_curly;
            code += "\n";
            
            code += language_parameters.indent;
            code += language_parameters.struct_field_decl_override;
            code += "hardSize: UInt";
            code += language_parameters.open_curly;
            code += language_parameters.indent;
            code += language_parameters.indent;
            code += "return ";
            code += NumToString(hardSize);
            code += "\n";
            code += language_parameters.indent;
            code += language_parameters.close_curly;
            code += "\n";
        }
        
        static void upper(std::string &s) {
            std::transform(s.begin(), s.end(), s.begin(), ::toupper);
        }
        
        const std::string swift_keywords[] = {
            "class",
            "operator",
            "deinit",
            "enum",
            "extension",
            "func",
            "import",
            "init",
            "let",
            "protocol",
            "static",
            "struct",
            "subscript",
            "typealias",
            "var",
            "break",
            "case",
            "continue",
            "default",
            "do",
            "else",
            "fallthrough",
            "if",
            "in",
            "for",
            "return",
            "switch",
            "where",
            "while",
            "as",
            "is",
            "new",
            "super",
            "self",
            "Self",
            "Type",
            "associativity",
            "didSet",
            "get",
            "infix",
            "inout",
            "mutating",
            "nonmutating",
            "operator",
            "override",
            "postfix",
            "precedence",
            "prefix",
            "set",
            "unowned",
            "weak",
            "Any",
            "AnyObject",
            "class",
        };
    }
    
    bool GenerateSwift(const Parser &parser,
                       const std::string &path,
                       const std::string &file_name) {
        swift::should_gen_namespace = !parser.opts.swift_without_namespace;
        swift::SwiftGenerator generator(parser, path, file_name);
        return generator.generate();
    }
}