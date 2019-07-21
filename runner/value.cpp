/* **************************************************************************
 * General Class Definitions
 *
 * This file contains definitions and close utilities for several small utility
 * classes used by the compiler.
 *
 * Part of GTRPE by Gren Drake
 * **************************************************************************/
#include <fstream>
#include <sstream>
#include <string>

#include "gamedata.h"
#include "value.h"

std::ostream& operator<<(std::ostream &out, const Value::Type &type) {
    switch(type) {
        case Value::None:
            out << "None";
            break;
        case Value::Integer:
            out << "Integer";
            break;
        case Value::String:
            out << "String";
            break;
        case Value::Object:
            out << "Object";
            break;
        case Value::List:
            out << "List";
            break;
        case Value::Map:
            out << "Map";
            break;
        case Value::Function:
            out << "Function";
            break;
        case Value::Property:
            out << "Property";
            break;
        case Value::VarRef:
            out << "VarRef";
            break;
        case Value::JumpTarget:
            out << "Jump Target";
            break;
        case Value::LocalVar:
            out << "LocalVar";
            break;
        default:
            out << "(unhandled type " << static_cast<int>(type) << ")";
    }
    return out;
}

std::ostream& operator<<(std::ostream &out, const Value &value) {
    std::stringstream ss;
    ss << '<' << value.type;
    if (value.type != Value::None) {
        ss << ' ' << value.value;
    }
    ss << '>';
    out << ss.str();
    return out;
}

void Value::requireType(Value::Type theType) {
    if (type != theType) {
        std::stringstream ss;
        ss << "Expected value of type " << theType << ", but found value of type";
        ss << type << '.';
    }
}

void Value::requireType(Value::Type typeOne, Value::Type typeTwo) {
    if (type != typeOne && type != typeTwo) {
        std::stringstream ss;
        ss << "Expected value of type " << typeOne << " or ";
        ss << typeTwo << ", but found value of type";
        ss << type << '.';
    }
}

void Value::forbidType(Value::Type theType) {
    if (type == theType) {
        std::stringstream ss;
        ss << "Found value of forbidden type " << theType << ".";
    }
}

bool Value::isTrue() const {
    return type != None && value != 0;
}

int Value::compare(const Value &rhs) const {
    if (type != rhs.type) return 1;
    switch(type) {
        case None:
            return 0;
        case Integer:
            return value - rhs.value;
        default:
            return value == rhs.value ? 0 : 1;
    }
}
