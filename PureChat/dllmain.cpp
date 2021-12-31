#ifndef PCH_H
#define PCH_H

// Добавьте сюда заголовочные файлы для предварительной компиляции
#include <Windows.h>
#include <codecvt>
#include <locale>
#define _SILENCE_CXX17_CODECVT_HEADER_DEPRECATION_WARNING
#include <EventAPI.h>
#include <LLAPI.h>
#include <MC/Actor.hpp>
#include <MC/Block.hpp>
#include <MC/CommandOrigin.hpp>
#include <MC/CommandOutput.hpp>
#include <MC/CommandPosition.hpp>
#include <MC/CommandRegistry.hpp>
#include <MC/ComponentItem.hpp>
#include <MC/CompoundTag.hpp>
#include <MC/HashedString.hpp>
#include <MC/Item.hpp>
#include <MC/ItemStack.hpp>
#include <MC/Level.hpp>
#include <MC/Mob.hpp>
#include <MC/MobEffect.hpp>
#include <MC/MobEffectInstance.hpp>
#include <MC/Player.hpp>
#include <MC/ServerPlayer.hpp>
#include <MC/Tag.hpp>
#include <MC/Types.hpp>
#include <RegCommandAPI.h>
#include <ServerAPI.h>
#include <iostream>
#pragma warning(push)
#pragma warning(disable : 4996)
#pragma warning(disable : 4251)
#pragma warning(disable : 4275)
#include<third-party/yaml-cpp/yaml.h>
#pragma warning(pop)
#include <algorithm>
#include <fstream>
#include <iostream>
#include <regex>

#pragma comment(lib, "bedrock_server_api.lib")
#pragma comment(lib, "bedrock_server_var.lib")
#pragma comment(lib, "LiteLoader.lib")
#pragma comment(lib, "yaml-cpp.lib")
#pragma comment(lib, "SymDBHelper.lib")

#endif //PCH_H
using namespace std;

std::wstring to_wstring(const std::string& str,
    const std::locale& loc = std::locale())
{
    std::vector<wchar_t> buf(str.size());
    std::use_facet<std::ctype<wchar_t>>(loc).widen(str.data(),
        str.data() + str.size(),
        buf.data());
    return std::wstring(buf.data(), buf.size());
}

std::string utf8_encode(const std::wstring& wstr)
{
    if (wstr.empty()) return std::string();
    int size_needed = WideCharToMultiByte(CP_UTF8, 0, &wstr[0], (int)wstr.size(), NULL, 0, NULL, NULL);
    std::string strTo(size_needed, 0);
    WideCharToMultiByte(CP_UTF8, 0, &wstr[0], (int)wstr.size(), &strTo[0], size_needed, NULL, NULL);
    return strTo;
}

BOOL APIENTRY DllMain(HMODULE hModule,
    DWORD  ul_reason_for_call,
    LPVOID lpReserved
)
{
    switch (ul_reason_for_call)
    {
    case DLL_PROCESS_ATTACH:
        LL::registerPlugin("PureChat v1.0", "Порт всеми известного PureChat под LiteLoader 2.0", LL::Version(0, 0, 0, LL::Version::Dev));
    case DLL_THREAD_ATTACH:
    case DLL_THREAD_DETACH:
    case DLL_PROCESS_DETACH:
        break;
    }
    return TRUE;
}

class _Group {
public:
    string         name; //имя группы
    string         prefix;
    vector<string> perms;       //права
    bool           is_default;  //установлен ли по умолчанию
    string         inheritance; //группу,которую будем наследовать
};

class _Groups {
public:
    vector<_Group> groups; //группы
};

class _User {
public:
    string nickname; //ник
    string prefix;
    string suffix;
    string         group;       //группа игрока
    vector<string> permissions; //права игрока
};

class Users {
public:
    vector<_User> users; //игроки
};

namespace YAML
{
    template <>
    struct convert<_Group>
    {
        static Node encode(const _Group& rhs)
        {
            Node node;
            node[rhs.name] = YAML::Node(YAML::NodeType::Map);
            node[rhs.name]["prefix"] = rhs.prefix;
            node[rhs.name]["inheritance"] = rhs.inheritance;
            node[rhs.name]["default"] = rhs.is_default;
            if (rhs.perms.size() == 0)
            {
                node[rhs.name]["permissions"] = vector<string>(0);
            }
            node[rhs.name]["permissions"] = rhs.perms;
            return node;
        }
        static bool decode(const Node& node, _Group& rhs)
        {
            using namespace std;
            string name;
            for (const auto& kv : node)
            {
                if (kv.first.as<std::string>() == "")
                {
                    continue;
                }
                name = kv.first.as<std::string>();
                break;
            }
            rhs.name = name;
            rhs.prefix = node[name]["prefix"].as<std::string>();
            rhs.inheritance = node[name]["inheritance"].as<std::string>();
            rhs.is_default = node[name]["default"].as<bool>();
            rhs.perms = node[name]["permissions"].as<std::vector<string>>();
            return true;
        }
    };
} // namespace YAML

string utf8_to_string(const char* utf8str, const locale& loc)
{
    // UTF-8 to wstring
    wstring_convert<codecvt_utf8<wchar_t>> wconv;
    wstring wstr = wconv.from_bytes(utf8str);
    // wstring to string
    vector<char> buf(wstr.size());
    use_facet<ctype<wchar_t>>(loc).narrow(wstr.data(), wstr.data() + wstr.size(), '?', buf.data());
    return string(buf.data(), buf.size());
}

namespace YAML
{
    template <>
    struct convert<_User>
    {
        static Node encode(const _User& rhs)
        {
            Node node;
            node[rhs.nickname] = YAML::Node(YAML::NodeType::Map);
            node[rhs.nickname]["group"] = (rhs.group);
            node[rhs.nickname]["prefix"] = (rhs.prefix);
            node[rhs.nickname]["suffix"] = (rhs.suffix);
            if (rhs.permissions.size() == 0)
            {
                node[rhs.nickname]["permissions"] = {};
            }
            node[rhs.nickname]["permissions"] = rhs.permissions;
            return node;
        }

        static bool decode(const Node& node, _User& rhs) {
            using namespace std;
            string name;
            for (const auto& kv : node) {
                if (kv.first.as<std::string>() == "")
                {
                    continue;
                }
                name = kv.first.as<std::string>();
                break;
            }
            rhs.nickname = name;
            rhs.group = node[name]["group"].as<string>();
            rhs.prefix = node[name]["prefix"].as<string>();
            rhs.suffix = node[name]["suffix"].as<string>();
            rhs.permissions = node[name]["permissions"].as<vector<string>>();
            return true;
        }
    };
} // namespace YAML


_Group load_group(string name) {
    YAML::Node config = YAML::LoadFile("plugins/PurePerms/groups.yml");
    _Group group;
    using namespace std;
    for (const auto& p : config["groups"]) {
        _Group g = p.as<_Group>();
        if (g.name == name)
        {
            group = g;
            break;
        }
    }
    return group;
}

_User load_user(string nick) {
    YAML::Node config = YAML::LoadFile("plugins/PurePerms/users.yml");
    using namespace std;
    _User user;
    for (const auto& p : config["users"]) {
        _User us = p.as<_User>();
        if (nick == us.nickname)
        {
            user = us;
            break;
        }
    }
    return user;
}

vector<string> split(string s, string delimiter) {
    size_t         pos_start = 0, pos_end, delim_len = delimiter.length();
    string         token;
    vector<string> res;

    while ((pos_end = s.find(delimiter, pos_start)) != string::npos) {
        token = s.substr(pos_start, pos_end - pos_start);
        pos_start = pos_end + delim_len;
        res.push_back(token);
    }
    res.push_back(s.substr(pos_start));
    return res;
}


YAML::Node  config;
_Groups    groups;
YAML::Node config1;
Users      users;

class Setsuffix : public Command
{
    CommandSelector<Actor> player;
    string suffix;
public:
    void execute(CommandOrigin const& ori, CommandOutput& output) const override
    {
        string perm = "pchat.command.setsuffix";
        _Groups    groups;
        Users      users;
        auto nodes = YAML::LoadFile("plugins/PurePerms/users.yml");
        for (const auto& p : nodes["users"])
        {
            users.users.push_back(p.as<_User>());
        }
        auto nodes1 = YAML::LoadFile("plugins/PurePerms/groups.yml");
        for (const auto& p : nodes1["groups"])
        {
            groups.groups.push_back(p.as<_Group>());
        }
        if (player.getName() == "" || suffix == "")
        {
            output.error("Invalid argument command!");
            return;
        }
        if (ori.getPermissionsLevel() == CommandPermissionLevel::Console && player.getName() != "" && suffix != "")
        {
            for (int i = 0; i < users.users.size(); ++i)
            {
                if (player.getName() == users.users[i].nickname)
                {
                    users.users[i].suffix = suffix;
                    config1.reset();
                    for (auto v : users.users)
                        config1["users"].push_back(v);
                    remove("plugins/PurePerms/users.yml");
                    ofstream fout("plugins/PurePerms/users.yml");
                    fout << config1;
                    fout.close();
                    break;
                }
            }
            output.success("Suffix for user " + player.getName() + " changed to " + suffix);
            return;
        }
        auto plain = ori.getPlayer()->getName();
        using namespace std;
        auto nick = split(plain, " ");
        string res_nick;
        for (auto n : nick)
        {
            for (auto v : users.users)
            {
                if (n == v.nickname)
                {
                    res_nick = n;
                    break;
                }
            }
        }
        auto pl = load_user(res_nick);
        auto gr = load_group(pl.group);
        for (auto v : pl.permissions)
        {
            if (v == perm)
            {
                for (int i = 0; i < users.users.size(); ++i)
                {
                    if (player.getName() == users.users[i].nickname)
                    {
                        users.users[i].suffix = suffix;
                        config1.reset();
                        for (auto v : users.users)
                            config1["users"].push_back(v);
                        remove("plugins/PurePerms/users.yml");
                        ofstream fout("plugins/PurePerms/users.yml");
                        fout << config1;
                        fout.close();
                        break;
                    }
                }
                output.success("Suffix for user " + player.getName() + " changed to " + suffix);
                return;
            }
        }
        for (auto v : gr.perms)
        {
            if (v == perm)
            {
                for (int i = 0; i < users.users.size(); ++i)
                {
                    if (player.getName() == users.users[i].nickname)
                    {
                        users.users[i].suffix = suffix;
                        config1.reset();
                        for (auto v : users.users)
                            config1["users"].push_back(v);
                        remove("plugins/PurePerms/users.yml");
                        ofstream fout("plugins/PurePerms/users.yml");
                        fout << config1;
                        fout.close();
                        break;
                    }
                }
                output.success("Suffix for user " + player.getName() + " changed to " + suffix);
                return;
            }
        }
        output.error("You do not have permission to execute this command!");
        return;
    }
    static void setup(CommandRegistry* registry) {

        registry->registerCommand(
            "setsuffix", "Set suffix for user.", CommandPermissionLevel::Any, { (CommandFlagValue)0 },
            { (CommandFlagValue)0x80 });
        registry->registerOverload<Setsuffix>("setsuffix", RegisterCommandHelper::makeMandatory(&Setsuffix::player, "player"), RegisterCommandHelper::makeMandatory(&Setsuffix::suffix, "suffix"));
    }
};

class Setprefix : public Command
{
    CommandSelector<Actor> player;
    string prefix;
public:
    void execute(CommandOrigin const& ori, CommandOutput& output) const override
    {
        string perm = "pchat.command.setprefix";
        _Groups    groups;
        Users      users;
        auto nodes = YAML::LoadFile("plugins/PurePerms/users.yml");
        for (const auto& p : nodes["users"])
        {
            users.users.push_back(p.as<_User>());
        }
        auto nodes1 = YAML::LoadFile("plugins/PurePerms/groups.yml");
        for (const auto& p : nodes1["groups"])
        {
            groups.groups.push_back(p.as<_Group>());
        }
        if (player.getName() == "" || prefix == "")
        {
            output.error("Invalid argument command!");
            return;
        }
        if (ori.getPermissionsLevel() == CommandPermissionLevel::Console && player.getName() != "" && prefix != "")
        {
            for (int i = 0; i < users.users.size(); ++i)
            {
                if (player.getName() == users.users[i].nickname)
                {
                    users.users[i].prefix = prefix;
                    config1.reset();
                    for (auto v : users.users)
                        config1["users"].push_back(v);
                    remove("plugins/PurePerms/users.yml");
                    ofstream fout("plugins/PurePerms/users.yml");
                    fout << config1;
                    fout.close();
                    break;
                }
            }
            output.success("Prefix for user " + player.getName() + " changed to " + prefix);
            return;
        }
        auto plain = ori.getPlayer()->getName();
        using namespace std;
        auto nick = split(plain, " ");
        string res_nick;
        for (auto n : nick)
        {
            for (auto v : users.users)
            {
                if (n == v.nickname)
                {
                    res_nick = n;
                    break;
                }
            }
        }
        auto pl = load_user(res_nick);
        auto gr = load_group(pl.group);
        for (auto v : pl.permissions)
        {
            if (v == perm)
            {
                for (int i = 0; i < users.users.size(); ++i)
                {
                    if (player.getName() == users.users[i].nickname)
                    {
                        users.users[i].prefix = prefix;
                        config1.reset();
                        for (auto v : users.users)
                            config1["users"].push_back(v);
                        remove("plugins/PurePerms/users.yml");
                        ofstream fout("plugins/PurePerms/users.yml");
                        fout << config1;
                        fout.close();
                        break;
                    }
                }
                output.success("Prefix for user " + player.getName() + " changed to " + prefix);
                return;
            }
        }
        for (auto v : gr.perms)
        {
            if (v == perm)
            {
                for (int i = 0; i < users.users.size(); ++i)
                    for (int i = 0; i < users.users.size(); ++i)
                    {
                        if (player.getName() == users.users[i].nickname)
                        {
                            users.users[i].prefix = prefix;
                            config1.reset();
                            for (auto v : users.users)
                                config1["users"].push_back(v);
                            remove("plugins/PurePerms/users.yml");
                            ofstream fout("plugins/PurePerms/users.yml");
                            fout << config1;
                            fout.close();
                            break;
                        }
                    }
                output.success("Prefix for user " + player.getName() + " changed to " + prefix);
                return;
            }
        }
        output.error("You do not have permission to execute this command!");
        return;
    }
    static void setup(CommandRegistry* registry) {

        registry->registerCommand(
            "setprefix", "Set prefix for user.", CommandPermissionLevel::Any, { (CommandFlagValue)0 },
            { (CommandFlagValue)0x80 });
        registry->registerOverload<Setprefix>("setprefix", RegisterCommandHelper::makeMandatory(&Setprefix::player, "player"), RegisterCommandHelper::makeMandatory(&Setprefix::prefix, "prefix"));
    }
};

class Setnametag : public Command
{
    CommandSelector<Actor> player;
    string tag;
public:
    void execute(CommandOrigin const& ori, CommandOutput& output) const override
    {
        string perm = "pchat.command.setnametag";
        _Groups    groups;
        Users      users;
        auto nodes = YAML::LoadFile("plugins/PurePerms/users.yml");
        for (const auto& p : nodes["users"])
        {
            users.users.push_back(p.as<_User>());
        }
        auto nodes1 = YAML::LoadFile("plugins/PurePerms/groups.yml");
        for (const auto& p : nodes1["groups"])
        {
            groups.groups.push_back(p.as<_Group>());
        }
        if (player.getName() == "" || tag == "")
        {
            output.error("Invalid argument command!");
            return;
        }
        if (ori.getPermissionsLevel() == CommandPermissionLevel::Console && player.getName() != "" && tag != "")
        {
            auto db = Level::getAllPlayers();
            for (auto v : db)
            {
                if (v->getName() == player.getName())
                {
                    v->setNameTag(tag);
                    break;
                }
            }
            output.success("Nametag for user " + player.getName() + " changed to " + tag);
            return;
        }
        auto plain = ori.getPlayer()->getName();
        using namespace std;
        auto nick = split(plain, " ");
        string res_nick;
        for (auto n : nick)
        {
            for (auto v : users.users)
            {
                if (n == v.nickname)
                {
                    res_nick = n;
                    break;
                }
            }
        }
        auto pl = load_user(res_nick);
        auto gr = load_group(pl.group);
        for (auto v : pl.permissions)
        {
            if (v == perm)
            {
                auto db = Level::getAllPlayers();
                for (auto v : db)
                {
                    if (v->getName() == player.getName())
                    {
                        v->setNameTag(tag);
                        break;
                    }
                }
                output.success("Nametag for user " + player.getName() + " changed to " + tag);
                return;
            }
        }
        for (auto v : gr.perms)
        {
            if (v == perm)
            {
                auto db = Level::getAllPlayers();
                for (auto v : db)
                {
                    if (v->getName() == player.getName())
                    {
                        v->setNameTag(tag);
                        break;
                    }
                }
                output.success("Nametag for user " + player.getName() + " changed to " + tag);
                return;
            }
        }
        output.error("You do not have permission to execute this command!");
        return;
    }
    static void setup(CommandRegistry* registry) {

        registry->registerCommand(
            "setnametag", "Set nametag for user.", CommandPermissionLevel::Any, { (CommandFlagValue)0 },
            { (CommandFlagValue)0x80 });
        registry->registerOverload<Setnametag>("setnametag", RegisterCommandHelper::makeMandatory(&Setnametag::player, "player"), RegisterCommandHelper::makeMandatory(&Setnametag::tag, "nametag"));
    }
};

void entry();

extern "C" {
    _declspec(dllexport) void onPostInit() {
        std::ios::sync_with_stdio(false);
        entry();
    }
}

bool servRun = false;

void entry()
{
    Event::ServerStartedEvent::subscribe([](const Event::ServerStartedEvent& ev)
        {
            try
            {

                using namespace std;
                config = YAML::LoadFile("plugins/PurePerms/groups.yml");
                config1 = YAML::LoadFile("plugins/PurePerms/users.yml");
                for (const auto& p : config["groups"]) {
                    groups.groups.push_back(p.as<_Group>());
                }
                users.users.clear();
                for (const auto& p : config1["users"]) {
                    users.users.push_back(p.as<_User>());
                }

            }
            catch (std::exception& e)
            {
                std::cerr << e.what() << std::endl;
            }
            return 1;
        });
    Event::RegCmdEvent::subscribe([](const Event::RegCmdEvent& ev)
        {
            try
            {
                Setsuffix::setup(ev.mCommandRegistry);
                Setprefix::setup(ev.mCommandRegistry);
                Setnametag::setup(ev.mCommandRegistry);
            }
            catch (std::exception s) {
                std::cerr << s.what() << std::endl;
            }
            return true;
        });
    Event::PlayerChatEvent::subscribe([](const Event::PlayerChatEvent& ev)
        {
            auto nick = ev.mPlayer->getName();
            char sym = ev.mMessage[0];
            using namespace std;
            auto nick1 = split(ev.mPlayer->getName(), " ");
            string res_nick;
            for (auto n : nick1)
            {
                for (auto v : users.users)
                {
                    if (n == v.nickname)
                    {
                        res_nick = n;
                        break;
                    }
                }
            }
            auto pl = load_user(res_nick);
            wstring res;
            auto players = Level::getAllPlayers();
            if (sym != '!')
            {
                for (auto p : players)
                {
                    double x1 = ev.mPlayer->getPos().x, y1 = ev.mPlayer->getPos().y, z1 = ev.mPlayer->getPos().z, x2 = p->getPos().x, y2 = p->getPos().y, z2 = p->getPos().z;
                    double distance = sqrt(pow(x2 - x1, 2) + pow(y2 - y1, 2) + pow(z2 - z1, 2));
                    if (distance <= 100 && pl.prefix != "" && pl.suffix == "")
                    {
                        (ServerPlayer*)p->sendText(utf8_encode(to_wstring("[§eL§r] ")) + pl.prefix + " " + pl.nickname + ": " + ev.mMessage);
                    }
                    else if (distance <= 100 && pl.prefix == "" && pl.suffix != "")
                    {
                        (ServerPlayer*)p->sendText(utf8_encode(to_wstring("[§eL§r] ")) + pl.nickname + " " + pl.suffix + ": " + ev.mMessage);
                    }
                    else  if (distance <= 100 && pl.prefix != "" && pl.suffix != "")
                    {
                        (ServerPlayer*)p->sendText(utf8_encode(to_wstring("[§eL§r] ")) + pl.prefix + " " + pl.nickname + " " + pl.suffix + ": " + ev.mMessage);
                    }
                }
            }
            else if (sym == '!')
            {
                for (auto p : players)
                {
                    std::wstring_convert<std::codecvt_utf8_utf16<wchar_t>> converter1;
                    std::wstring prefix = to_wstring(pl.prefix);
                    std::wstring suffix = to_wstring(pl.suffix);
                    res = L"[§aG§r] " + prefix + L" " + wstring(pl.nickname.begin(), pl.nickname.end());
                    auto res1 = res + L": ";
                    auto res2 = utf8_encode(res1);
                    if (pl.prefix != "" && pl.suffix == "")
                    {
                        (ServerPlayer*)p->sendText(utf8_encode(to_wstring("[§aG§r] ")) + pl.prefix + " " + pl.nickname + ": " + string(ev.mMessage.begin() + 1, ev.mMessage.end()));
                    }
                    else if (pl.prefix == "" && pl.suffix != "")
                    {
                        (ServerPlayer*)p->sendText(utf8_encode(to_wstring("[§aG§r] ")) + pl.prefix + " " + pl.suffix + ": " + string(ev.mMessage.begin() + 1, ev.mMessage.end()));
                    }
                    else  if (pl.prefix != "" && pl.suffix != "")
                    {
                        (ServerPlayer*)p->sendText(utf8_encode(to_wstring("[§aG§r] ")) + pl.prefix + " " + pl.nickname + " " + pl.suffix + ": " + string(ev.mMessage.begin() + 1, ev.mMessage.end()));
                    }
                }
            }
            return 0;
        });
    Event::PlayerJoinEvent::subscribe([](const Event::PlayerJoinEvent& ev)
    {
      auto u = load_user(ev.mPlayer->getName());
      ev.mPlayer->setNameTag(u.prefix + utf8_encode(to_wstring(" ")) + utf8_encode(to_wstring(ev.mPlayer->getName())));
      return 1;
           
    });
}