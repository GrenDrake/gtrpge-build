#include "opcode.h"

OpcodeDef opcodes[] = {
    {   "return",       OpcodeDef::Return,                  1, 0 },
    {   "push_0",       OpcodeDef::Push0,                   0, 1, FORBID_ALWAYS },
    {   "push_1",       OpcodeDef::Push1,                   0, 1, FORBID_ALWAYS },
    {   "push_none",    OpcodeDef::PushNone,                0, 1, FORBID_ALWAYS },
    {   "push_8",       OpcodeDef::Push8,                   0, 1, FORBID_ALWAYS },
    {   "push_16",      OpcodeDef::Push16,                  0, 1, FORBID_ALWAYS },
    {   "push_32",      OpcodeDef::Push32,                  0, 1, FORBID_ALWAYS },
    {   "set",          OpcodeDef::Store,                   2, 0 },
    {   "say_uf",       OpcodeDef::SayUCFirst,              1, 0 },
    {   "say",          OpcodeDef::Say,                     1, 0, FORBID_EXPRESSION },
    {   "say_unsigned", OpcodeDef::SayUnsigned,             1, 0 },
    {   "say_char",     OpcodeDef::SayChar,                 1, 0 },
    {   "pop",          OpcodeDef::StackPop,                1, 0, FORBID_EXPRESSION },
    {   "stack_dup",    OpcodeDef::StackDup,                1, 2, FORBID_EXPRESSION },
    {   "stack_peek",   OpcodeDef::StackPeek,               1, 0, FORBID_EXPRESSION },
    {   "stack_size",   OpcodeDef::StackSize,               0, 1, FORBID_EXPRESSION },
    {   "call",         OpcodeDef::Call,                    2, 1, FORBID_EXPRESSION },
    {   "list_push",    OpcodeDef::ListPush,                2, 0 },
    {   "list_pop",     OpcodeDef::ListPop,                 1, 1 },
    {   "sort",         OpcodeDef::Sort,                    1, 0 },
    {   "get",          OpcodeDef::GetItem,                 2, 1 },
    {   "has",          OpcodeDef::HasItem,                 2, 1 },
    {   "setp",         OpcodeDef::SetItem,                 3, 0 },
    {   "size",         OpcodeDef::GetSize,                 1, 1 },
    {   "del",          OpcodeDef::DelItem,                 2, 0 },
    {   "ins",          OpcodeDef::InsItem,                 3, 0 },
    {   "typeof",       OpcodeDef::TypeOf,                  1, 1 },
    {   "astype",       OpcodeDef::AsType,                  2, 1 },
    {   "eq",           OpcodeDef::Equal,                   2, 1 },
    {   "neq",          OpcodeDef::NotEqual,                2, 1 },
    {   "jmp",          OpcodeDef::Jump,                    1, 0, FORBID_EXPRESSION },
    {   "jz",           OpcodeDef::JumpZero,                2, 0, FORBID_EXPRESSION },
    {   "jnz",          OpcodeDef::JumpNotZero,             2, 0, FORBID_EXPRESSION },
    {   "lt",           OpcodeDef::LessThan,                2, 1 },
    {   "lte",          OpcodeDef::LessThanEqual,           2, 1 },
    {   "gt",           OpcodeDef::GreaterThan,             2, 1 },
    {   "gte",          OpcodeDef::GreaterThanEqual,        2, 1 },
    {   "not",          OpcodeDef::Not,                     1, 1 },
    {   "add",          OpcodeDef::Add,                     2, 1 },
    {   "sub",          OpcodeDef::Sub,                     2, 1 },
    {   "mult",         OpcodeDef::Mult,                    2, 1 },
    {   "div",          OpcodeDef::Div,                     2, 1 },
    {   "mod",          OpcodeDef::Mod,                     2, 1 },
    {   "pow",          OpcodeDef::Pow,                     2, 1 },
    {   "left_shift",   OpcodeDef::BitLeft,                 2, 1 },
    {   "right_shift",  OpcodeDef::BitRight,                2, 1 },
    {   "bit_and",      OpcodeDef::BitAnd,                  2, 1 },
    {   "bit_or",       OpcodeDef::BitOr,                   2, 1 },
    {   "bit_xor",      OpcodeDef::BitXor,                  2, 1 },
    {   "bit_not",      OpcodeDef::BitNot,                  1, 1 },
    {   "random",       OpcodeDef::Random,                  2, 1 },
    {   "get_random",   OpcodeDef::GetRandom,               1, 1 },
    {   "get_keys",     OpcodeDef::GetKeys,                 1, 1 },
    {   "stack_swap",   OpcodeDef::StackSwap,               2, 0 },
    {   "get_setting",  OpcodeDef::GetSetting,              1, 1 },
    {   "set_setting",  OpcodeDef::SetSetting,              2, 0 },
    {   "get_key",      OpcodeDef::GetKey,                  1, 1 },
    {   "get_option",   OpcodeDef::GetOption,               1, 1 },
    {   "get_line",     OpcodeDef::GetLine,                 1, 1 },
    {   "add_option",   OpcodeDef::AddOption,               4, 0, FORBID_EXPRESSION },
    {   "str_clear",    OpcodeDef::StringClear,             1, 0 },
    {   "str_append",   OpcodeDef::StringAppend,            2, 0 },
    {   "str_append_uf",OpcodeDef::StringAppendUF,          2, 0 },
    {   "str_length",   OpcodeDef::StringLength,            1, 1 },
    {   "str_compare",  OpcodeDef::StringCompare,           2, 1 },
    {   "error",        OpcodeDef::Error,                   1, 0 },
    {   "origin",       OpcodeDef::Origin,                  1, 1 },
    {   "add_page",     OpcodeDef::AddPage,                 3, 0 },
    {   "del_page",     OpcodeDef::DelPage,                 1, 0 },
    {   "end_page",     OpcodeDef::EndPage,                 0, 0 },
    {   "new",          OpcodeDef::New,                     1, 1 },
    {   "is_static",    OpcodeDef::IsStatic,                1, 1 },
    {   ""                                                       }
};

const OpcodeDef* getOpcode(const std::string &name) {
    for (const OpcodeDef &code : opcodes) {
        if (code.name == name) return &code;
    }
    return nullptr;
}

OpcodeDef* getOpcodeByCode(int codeNumber) {
    for (OpcodeDef &code : opcodes) {
        if (code.code == codeNumber) return &code;
    }
    return nullptr;
}
