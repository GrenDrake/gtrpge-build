/* **************************************************************************
 * Core parsing functions
 *
 * Deals with parsing previously tokenized data into meaningful game objects
 * and properties.
 *
 * Part of GTRPE by Gren Drake
 * **************************************************************************/
#include <algorithm>
#include <cctype>
#include <iostream>
#include <map>
#include <sstream>
#include <string>
#include <vector>

#include "parsestate.h"
#include "builderror.h"
#include "gamedata.h"
#include "build.h"
#include "origin.h"
#include "token.h"

#include "bytestream.h"


struct OpcodeDef {
    enum CodeOpcode {
        Return              = 0,
        Push0               = 1,
        Push1               = 2,
        PushNeg1            = 3,
        Push8               = 4,
        Push16              = 5,
        Push32              = 6,
        Store               = 7,
        SayUCFirst          = 9,
        Say                 = 10,
        SayUnsigned         = 11,
        SayChar             = 12,
        StackPop            = 13, // remove the top item from the stack
        StackDup            = 14, // duplicate the top item on the stack
        StackPeek           = 15, // peek at the stack item X items from the top
        StackSize           = 16, // get the current size of the stack
        Call                = 17, // call a value as a function
        CallMethod          = 18, // call an object property as a function
        Self                = 19, // get object the current function is a property of
        GetProp             = 20,
        HasProp             = 21, // check if property is set on object
        SetProp             = 22, // set object property to value
        GetItem             = 23, // get item from list (index) or map (key)
        HasItem             = 24, // check if index (for list) or key (for map) exists
        GetSize             = 25, // get size of list or map
        SetItem             = 26, // set item in list (by index) of map (by key)
        TypeOf              = 27, // get value type
        DelItem             = 28, // remove an item from a list or a key from a map
        AddItem             = 29, // add an item to a list (use set-item for maps)
        AsType              = 30, // type conversion
        Compare             = 31, // compare two values and push the result
        Jump                = 32, // unconditional jump
        JumpZero            = 33, // jump if top of stack == 0
        JumpNotZero         = 34, // jump if top of stack != 0
        JumpLessThan        = 35, // jump if top of stack < 0
        JumpLessThanEqual   = 36, // jump if top of stack <= 0
        JumpGreaterThan     = 37, // jump if top of stack > 0
        JumpGreaterThanEqual= 38, // jump if top of stack >= 0
        Not                 = 39,
        Add                 = 40,
        Sub                 = 41,
        Mult                = 42,
        Div                 = 43,
        Mod                 = 44,
        Pow                 = 45,
        BitLeft             = 46,
        BitRight            = 47,
        BitAnd              = 48,
        BitOr               = 49,
        BitXor              = 50,
        BitNot              = 51,
        Random              = 52,
        Inc                 = 53,
        Dec                 = 54,
        GetRandom           = 55,
        GetKeys             = 56,
        /* Unused opcodes: 57 - 59 */
        GetKey              = 60,
        GetOption           = 61,
        GetLine             = 62,
        AddOption           = 63,
        AddOptionExtra      = 64,
        StringCopy          = 65,
        StringAppend        = 66,
        StringLength        = 67,
        StringCompare       = 68,
        /* Unused opcode: 69 */
        SetInfo             = 70,
        AddPage             = 71,
        DelPage             = 72,
        EndPage             = 73,
        New                 = 74,
        /* Unused opcode: 75 */
        IsStatic            = 76,
    };

    std::string name;
    int code;
    int arg_size;
};


OpcodeDef opcodes[] = {
    {   "return",       OpcodeDef::Return       },
    {   "push_0",       OpcodeDef::Push0        },
    {   "push_1",       OpcodeDef::Push1        },
    {   "push_n1",      OpcodeDef::PushNeg1     },
    {   "push_8",       OpcodeDef::Push8        },
    {   "push_16",      OpcodeDef::Push16       },
    {   "push_32",      OpcodeDef::Push32       },
    {   "store",        OpcodeDef::Store        },
    {   "say_uf",       OpcodeDef::SayUCFirst   },
    {   "say",          OpcodeDef::Say          },
    {   "say_unsigned", OpcodeDef::SayUnsigned  },
    {   "say_char",     OpcodeDef::SayChar      },
    {   "pop",          OpcodeDef::StackPop     },
    {   "stack_dup",    OpcodeDef::StackDup     },
    {   "stack_peek",   OpcodeDef::StackPeek    },
    {   "stack_size",   OpcodeDef::StackSize    },
    {   "call",         OpcodeDef::Call         },
    {   "call_method",  OpcodeDef::CallMethod   },
    {   "self",         OpcodeDef::Self         },
    {   "get_prop",     OpcodeDef::GetProp      },
    {   "has_prop",     OpcodeDef::HasProp      },
    {   "set_prop",     OpcodeDef::SetProp      },
    {   "get_item",     OpcodeDef::GetItem      },
    {   "has_item",     OpcodeDef::HasItem      },
    {   "get_size",     OpcodeDef::GetSize      },
    {   "set_item",     OpcodeDef::SetItem      },
    {   "del_item",     OpcodeDef::DelItem      },
    {   "add_item",     OpcodeDef::AddItem      },
    {   "typeof",       OpcodeDef::TypeOf       },
    {   "astype",       OpcodeDef::AsType       },
    {   "cmp",          OpcodeDef::Compare              },
    {   "jmp",          OpcodeDef::Jump                 },
    {   "jeq",          OpcodeDef::JumpZero             },
    {   "jz",           OpcodeDef::JumpZero             },
    {   "jneq",         OpcodeDef::JumpNotZero          },
    {   "jnz",          OpcodeDef::JumpNotZero          },
    {   "jlt",          OpcodeDef::JumpLessThan         },
    {   "jlte",         OpcodeDef::JumpLessThanEqual    },
    {   "jgt",          OpcodeDef::JumpGreaterThan      },
    {   "jgte",         OpcodeDef::JumpGreaterThanEqual },
    {   "not",          OpcodeDef::Not                  },
    {   "add",          OpcodeDef::Add          },
    {   "sub",          OpcodeDef::Sub          },
    {   "mult",         OpcodeDef::Mult         },
    {   "div",          OpcodeDef::Div          },
    {   "mod",          OpcodeDef::Mod},
    {   "pow",          OpcodeDef::Pow},
    {   "left_shift",   OpcodeDef::BitLeft      },
    {   "right_shift",  OpcodeDef::BitRight     },
    {   "bit_and",      OpcodeDef::BitAnd       },
    {   "bit_or",       OpcodeDef::BitOr        },
    {   "bit_xor",      OpcodeDef::BitXor       },
    {   "bit_not",      OpcodeDef::BitNot       },
    {   "random",       OpcodeDef::Random       },
    {   "inc",          OpcodeDef::Inc          },
    {   "dec",          OpcodeDef::Dec          },
    {   "get_random",   OpcodeDef::GetRandom    },
    {   "get_keys",     OpcodeDef::GetKeys      },
    {   "get_key",      OpcodeDef::GetKey       },
    {   "get_option",   OpcodeDef::GetOption    },
    {   "get_line",     OpcodeDef::GetLine      },
    {   "add_option",   OpcodeDef::AddOption    },
    {   "add_option_x", OpcodeDef::AddOptionExtra },
    {   "strcpy",       OpcodeDef::StringCopy   },
    {   "strcat",       OpcodeDef::StringAppend },
    {   "strlen",       OpcodeDef::StringLength },
    {   "strcmp",       OpcodeDef::StringCompare },
    {   "set_info",     OpcodeDef::SetInfo      },
    {   "add_page",     OpcodeDef::AddPage      },
    {   "del_page",     OpcodeDef::DelPage      },
    {   "end_page",     OpcodeDef::EndPage      },
    {   "new",          OpcodeDef::New          },
    {   "is_static",    OpcodeDef::IsStatic     },
    {   ""                                      }
};

static void parse_asm_function(GameData &gamedata, FunctionDef *function, ParseState &state);
static void bytecode_push_value(ByteStream &bytecode, Value::Type type, int32_t value);

const OpcodeDef* getOpcode(const std::string &name) {
    for (const OpcodeDef &code : opcodes) {
        if (code.name == name) return &code;
    }
    return nullptr;
}

void bytecode_push_value(ByteStream &bytecode, Value::Type type, int32_t value) {
    if (value == 0) {
        bytecode.add_8(OpcodeDef::Push0);
        bytecode.add_8(type);
    } else if (value == 1) {
        bytecode.add_8(OpcodeDef::Push1);
        bytecode.add_8(type);
    } else if (value == -1) {
        bytecode.add_8(OpcodeDef::PushNeg1);
        bytecode.add_8(type);
    } else if (value >= INT8_MIN && value <= INT8_MAX) {
        bytecode.add_8(OpcodeDef::Push8);
        bytecode.add_8(type);
        bytecode.add_8(value);
    } else if (value >= INT16_MIN && value <= INT16_MAX) {
        bytecode.add_8(OpcodeDef::Push16);
        bytecode.add_8(type);
        bytecode.add_16(value);
    } else {
        bytecode.add_8(OpcodeDef::Push32);
        bytecode.add_8(type);
        bytecode.add_32(value);
    }
}

bool nameInUse(GameData &gamedata, FunctionDef *function, const std::string &name, unsigned localId) {
    if (getOpcode(name)) return true;
    if (gamedata.symbols.get(name)) return true;
    for (const auto &labelIter : function->labels) {
        if (labelIter.first == name) {
            return true;
        }
    }
    for (unsigned i = 0; i < function->local_names.size(); ++i) {
        if (i == localId) continue;
        if (name == function->local_names[i]) {
            return true;
        }
    }
    return false;
}

struct Backpatch {
    unsigned position;
    std::string name;
};
void parse_asm_function(GameData &gamedata, FunctionDef *function, ParseState &state) {
    const OpcodeDef *opcode;
    const SymbolDef *symbol;
    std::vector<Backpatch> patches;

    while (!state.at_end()) {
        int ident;
        switch(state.here()->type) {
            case Token::String:
                ident = gamedata.getStringId(state.here()->text);
                bytecode_push_value(function->code, Value::String, ident);
                break;
            case Token::Identifier: {
                // is label
                if (state.peek() && state.peek()->type == Token::Colon) {
                    if (nameInUse(gamedata, function, state.here()->text, -1)) {
                        std::stringstream ss;
                        ss << "Symbol \"" << state.here()->text << "\" already defined.";
                        throw BuildError(state.here()->origin, ss.str());
                    }
                    function->labels.insert(std::make_pair(state.here()->text, function->code.size()));
                    state.next();
                    break;
                }
                // is opcode name
                opcode = getOpcode(state.here()->text);
                if (opcode) {
                    function->code.add_8(opcode->code);
                    break;
                }
                // is global symbol
                symbol = gamedata.symbols.get(state.here()->text);
                if (symbol) {
                    bytecode_push_value(function->code, symbol->value.type, symbol->value.value);
                    break;
                }
                // is local name
                auto localIter = std::find(function->local_names.begin(), function->local_names.end(), state.here()->text);
                if (localIter != function->local_names.end()) {
                    int localNumber = std::distance(function->local_names.begin(), localIter);
                    bytecode_push_value(function->code, Value::LocalVar, localNumber);
                    break;
                }
                // presume its a label
                auto labelIter = function->labels.find(state.here()->text);
                if (labelIter != function->labels.end()) {
                    bytecode_push_value(function->code, Value::JumpTarget, labelIter->second);
                } else {
                    function->code.add_8(OpcodeDef::Push32);
                    function->code.add_8(Value::JumpTarget);
                    unsigned labelPos = function->code.size();
                    patches.push_back(Backpatch{labelPos, state.here()->text});
                    function->code.add_32(0xFFFFFFFF);
                }
                break;
            }
            case Token::Integer:
                bytecode_push_value(function->code, Value::Integer, state.here()->value);
                break;
            case Token::Property:
                bytecode_push_value(function->code, Value::Property, state.here()->value);
                break;
            default:
                throw BuildError(state.here()->origin, "Unexpected token in function.");
        }
        state.next();
    }

    for (const Backpatch &patch : patches) {
        auto labelIter = function->labels.find(patch.name);
        if (labelIter != function->labels.end()) {
            function->code.overwrite_32(patch.position, labelIter->second);
        } else {
            std::stringstream ss;
            ss << "Unknown symbol " << patch.name << " in function " << function->name << '.';
            throw BuildError(ss.str());
        }
    }
}

int parse_functions(GameData &gamedata) {
    for (FunctionDef *function : gamedata.functions) {
        if (function == nullptr) continue;
        ParseState state = {
            gamedata,
            function->tokens,
            function->tokens.cbegin()
        };

        for (unsigned i = 0; i < function->local_names.size(); ++i) {
            const std::string &name = function->local_names[i];
            if (nameInUse(gamedata, function, name, i)) {
                std::stringstream ss;
                ss << "Local name \"" << name << "\" already in use.";
                throw BuildError(function->origin, ss.str());
            }
        }

        function->codePosition = gamedata.bytecode.size();
        if (state.at_end()) {
            // empty function
        } else if (state.matches("asm")) {
            state.next();
            parse_asm_function(gamedata, function, state);
        } else {
            throw BuildError(function->origin, "Unknown function type.");
        }
        function->code.add_8(OpcodeDef::Return);
        function->code.padTo(4);
        gamedata.bytecode.append(function->code);
    }

    return 1;
}