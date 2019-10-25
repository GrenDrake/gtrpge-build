#ifndef GAMEDATA_H
#define GAMEDATA_H

#include <array>
#include <string>
#include <map>
#include <vector>
#include "bytestream.h"
#include "gameerror.h"
#include "stack.h"
#include "value.h"

const int FILETYPE_ID = 0x47505254;
const int HEADER_SIZE = 64;
const int ORIGIN_DYNAMIC = -2;
const int GARBAGE_FREQUENCY = 100;

const int INFO_TITLE  = 0;
const int INFO_LEFT   = 1;
const int INFO_RIGHT  = 2;
const int INFO_BOTTOM = 3;
const int INFO_COUNT  = 4;

const int SETTING_SAVE_ALLOWED   = 0;
const int SETTING_INFOBAR_LEFT   = 1;
const int SETTING_INFOBAR_RIGHT  = 2;
const int SETTING_INFOBAR_FOOTER = 3;
const int SETTING_INFOBAR_TITLE  = 4;

const int PROP_INTERNAL_NAME     = 1;
const int PROP_IDENT             = 2;
const int PROP_PARENT            = 3;

struct GameData;

struct DataItem {
    unsigned ident;
    int srcFile, srcLine, srcName;
    bool gcMark;
};

struct StringDef : public DataItem {
    std::string text;
};

struct ListDef : public DataItem {
    std::vector<Value> items;

    Value get(int key) const;
    bool has(int key) const;
    void set(int key, const Value &value);
    void del(int key);
};
struct MapDef : public DataItem  {
    struct Row {
        Value key, value;
    };
    std::vector<Row> rows;

    Value get(const Value &key) const;
    bool has(const Value &key) const;
    void set(const Value &key, const Value &value);
    void del(const Value &key);
};
struct ObjectDef : public DataItem  {
    std::map<unsigned, Value> properties;

    Value get(GameData &gamedata, unsigned propId, bool checkParent = true) const;
    bool has(unsigned propId) const;
    void set(unsigned propId, const Value &value);
};
struct FunctionDef : public DataItem  {
    int arg_count;
    int local_count;
    unsigned position;
};

enum class OptionType {
    None, Choice, Key, Line, EndOfProgram
};

struct GameOption {
    int strId;
    Value value;
    Value extra;
    int hotkey;
};

struct GameData {
    GameData() : showDebug(0), instructionCount(0), gameLoaded(false),
                 mCallCount(0) { }
    ~GameData();
    void load(const std::string &filename);
    void dump() const;

    const StringDef& getString(int index) const;
    StringDef& getString(int index);
    const ListDef& getList(int index) const;
    ListDef& getList(int index);
    const MapDef& getMap(int index) const;
    MapDef& getMap(int index);
    const ObjectDef& getObject(int index) const;
    ObjectDef& getObject(int index);
    const FunctionDef& getFunction(int index) const;
    FunctionDef& getFunction(int index);

    int collectGarbage();
    void mark(ObjectDef &object);
    void mark(ListDef   &list);
    void mark(MapDef    &map);
    void mark(StringDef &str);
    void mark(const Value &value);

    std::string getSource(const Value &value);
    Value resume(bool pushValue, const Value &inValue);
    void setExtra(const Value &newValue);
    void say(const std::string &what);
    void say(const Value &what);
    Value makeNew(Value::Type type);
    bool isStatic(const Value &what) const;
    bool isValid(const Value &what) const;
    void stringAppend(const Value &stringId, const Value &toAppend, bool upperFirst = false);
    std::string asString(const Value &value);
    void sortList(const Value &listId);

    bool showDebug;
    long instructionCount;
    OptionType optionType;
    std::vector<GameOption> options;
    int extraValue;
    std::string textBuffer;

    bool gameLoaded;
    int mainFunction;
    std::vector<StringDef*> strings;
    std::vector<ListDef*> lists;
    std::vector<MapDef*> maps;
    std::vector<ObjectDef*> objects;
    std::vector<FunctionDef> functions;
    ByteStream bytecode;
    unsigned staticStrings;
    unsigned staticLists;
    unsigned staticMaps;
    unsigned staticObjects;
    Value noneValue;

    std::array<std::string, INFO_COUNT> infoText;
    gtCallStack callStack;
private:
    unsigned mCallCount;
};

void gameloop(GameData &gamedata, bool doSilent);

#endif
