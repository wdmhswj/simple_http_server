#pragma once 
#include <cstdint>
#include <functional>
#include <string>
#include <memory>
#include <typeinfo>
#include <map>
#include <boost/lexical_cast.hpp>
#include "src/_log/macro.h"
#include "src/util/util.h"
#include <yaml-cpp/yaml.h>
#include <list>
#include <set>
#include <unordered_set>
#include <map>
#include <unordered_map>
#include <mutex>
#include <iostream>
#include "src/_log/log.h"

namespace shs {
// 配置变量的基类
class ConfigVarBase {
public:
    using ptr = std::shared_ptr<ConfigVarBase>;
    ConfigVarBase(const std::string& name, const std::string& description=""): m_name(name), m_description(description){
        std::transform(m_name.begin(), m_name.end(), m_name.begin(), ::tolower);
    }

    virtual ~ConfigVarBase() {}
    virtual std::string toString() = 0;
    virtual bool fromString(const std::string& val) = 0;
    // 返回配置参数的类型名称
    virtual std::string getTypeName() const = 0;
    const std::string& getName() const { return m_name; }
    const std::string& getDescription() const { return m_description; }

private:
    // 配置名称
    std::string m_name;
    // 配置描述
    std::string m_description;
};

// F: From_type, T: To_type
template<typename F, typename T>
class Lexical_cast {
public:
    T operator()(const F& f) {
        return boost::lexical_cast<T>(f);
    }
};

// Lexical_cast 偏特化
template<typename T>
class Lexical_cast<std::string, std::vector<T>> {
public:
    std::vector<T> operator()(const std::string& f) {
        // str -> yaml::node -> Lexical_cast<str, T> -> vec<T>
        auto node = YAML::Load(f);
        typename std::vector<T> res;
        std::stringstream ss;
        for(size_t i=0; i<node.size(); ++i) {
            ss.str("");
            ss << node[i];
            res.push_back(Lexical_cast<std::string, T>()(ss.str()));
        }
        return res;
    }
};

template<typename T>
class Lexical_cast<std::vector<T>, std::string> {
public:
    std::string operator()(const std::vector<T>& f) {
        // vec<T> -> Lexical_cast<T, str> -> node -> str
        YAML::Node node;
        for(const auto& i: f) {
            node.push_back(YAML::Load(Lexical_cast<T, std::string>()(i)));
        }
        std::stringstream ss;
        ss << node;
        return ss.str();
    }
};

template<typename T>
class Lexical_cast<std::string, std::list<T>> {
public:
    std::list<T> operator()(const std::string& f) {
        // str -> yaml::node -> Lexical_cast<str, T> -> vec<T>
        auto node = YAML::Load(f);
        typename std::list<T> res;
        std::stringstream ss;
        for(size_t i=0; i<node.size(); ++i) {
            ss.str("");
            ss << node[i];
            res.push_back(Lexical_cast<std::string, T>()(ss.str()));
        }
        return res;
    }
};

template<typename T>
class Lexical_cast<std::list<T>, std::string> {
public:
    std::string operator()(const std::list<T>& f) {
        // vec<T> -> Lexical_cast<T, str> -> node -> str
        YAML::Node node;
        for(const auto& i: f) {
            node.push_back(YAML::Load(Lexical_cast<T, std::string>()(i)));
        }
        std::stringstream ss;
        ss << node;
        return ss.str();
    }
};

template<typename T>
class Lexical_cast<std::string, std::set<T>> {
public:
    std::set<T> operator()(const std::string& f) {
        // str -> yaml::node -> Lexical_cast<str, T> -> vec<T>
        auto node = YAML::Load(f);
        typename std::set<T> res;
        std::stringstream ss;
        for(size_t i=0; i<node.size(); ++i) {
            ss.str("");
            ss << node[i];
            res.insert(Lexical_cast<std::string, T>()(ss.str()));
        }
        return res;
    }
};

template<typename T>
class Lexical_cast<std::set<T>, std::string> {
public:
    std::string operator()(const std::set<T>& f) {
        // vec<T> -> Lexical_cast<T, str> -> node -> str
        YAML::Node node;
        for(const auto& i: f) {
            node.push_back(YAML::Load(Lexical_cast<T, std::string>()(i)));
        }
        std::stringstream ss;
        ss << node;
        return ss.str();
    }
};

template<typename T>
class Lexical_cast<std::string, std::unordered_set<T>> {
public:
    std::unordered_set<T> operator()(const std::string& f) {
        // str -> yaml::node -> Lexical_cast<str, T> -> vec<T>
        auto node = YAML::Load(f);
        typename std::unordered_set<T> res;
        std::stringstream ss;
        for(size_t i=0; i<node.size(); ++i) {
            ss.str("");
            ss << node[i];
            res.insert(Lexical_cast<std::string, T>()(ss.str()));
        }
        return res;
    }
};

template<typename T>
class Lexical_cast<std::unordered_set<T>, std::string> {
public:
    std::string operator()(const std::unordered_set<T>& f) {
        // vec<T> -> Lexical_cast<T, str> -> node -> str
        YAML::Node node;
        for(const auto& i: f) {
            node.push_back(YAML::Load(Lexical_cast<T, std::string>()(i)));
        }
        std::stringstream ss;
        ss << node;
        return ss.str();
    }
};

template<typename T>
class Lexical_cast<std::string, std::map<std::string, T>> {
public:
    std::map<std::string, T> operator()(const std::string& f) {
        // str -> yaml::node -> Lexical_cast<str, T> -> vec<T>
        auto node = YAML::Load(f);
        typename std::map<std::string, T> res;
        std::stringstream ss;
        for(auto it=node.begin(); it!=node.end(); ++it) {
            ss.str("");
            ss << it->second;
            res.insert(std::make_pair(it->first.Scalar(), Lexical_cast<std::string, T>()(ss.str())));
        }
        return res;
    }
};

template<typename T>
class Lexical_cast<std::map<std::string, T>, std::string> {
public:
    std::string operator()(const std::map<std::string, T>& f) {
        // vec<T> -> Lexical_cast<T, str> -> node -> str
        YAML::Node node;
        for(const auto& i: f) {
            node[i.first] = YAML::Load(Lexical_cast<T, std::string>()(i.second));
        }
        std::stringstream ss;
        ss << node;
        return ss.str();
    }
};

template<typename T>
class Lexical_cast<std::string, std::unordered_map<std::string, T>> {
public:
    std::unordered_map<std::string, T> operator()(const std::string& f) {
        // str -> yaml::node -> Lexical_cast<str, T> -> vec<T>
        auto node = YAML::Load(f);
        typename std::unordered_map<std::string, T> res;
        std::stringstream ss;
        for(auto it=node.begin(); it!=node.end(); ++it) {
            ss.str("");
            ss << it->second;
            res.insert(std::make_pair(it->first.Scalar(), Lexical_cast<std::string, T>()(ss.str())));
        }
        return res;
    }
};

template<typename T>
class Lexical_cast<std::unordered_map<std::string, T>, std::string> {
public:
    std::string operator()(const std::unordered_map<std::string, T>& f) {
        // vec<T> -> Lexical_cast<T, str> -> node -> str
        YAML::Node node;
        for(const auto& i: f) {
            node[i.first] = YAML::Load(Lexical_cast<T, std::string>()(i.second));
        }
        std::stringstream ss;
        ss << node;
        return ss.str();
    }
};
template<typename T, typename FromStr = Lexical_cast<std::string, T>, typename ToStr = Lexical_cast<T, std::string>>
class ConfigVar: public ConfigVarBase {
public:
    using ptr = std::shared_ptr<ConfigVar>;
    using on_change_cb = std::function<void (const T& old_value, const T& new_value)>;

    ConfigVar(const T& default_value, const std::string& name, const std::string& description=""): ConfigVarBase(name, description), m_val(default_value) {}

    std::string toString() override {
        try {
            // return boost::lexical_cast<std::string>(m_val);
            // return Lexical_cast<T, std::string>()(m_val);
            return ToStr()(m_val);
        } catch(std::exception& e) {    
            SHS_LOG_ERROR(SHS_LOG_ROOT()) << "ConfigVar::toString exception " << e.what() << " convert: " << typeid(m_val).name() << " to string";
        }
        return {};
    }

    bool fromString(const std::string& val) override {
        try {
            // m_val = boost::lexical_cast<T>(val);
            // m_val = Lexical_cast<std::string, T>()(val);
            // m_val = FromStr()(val);
            setValue(FromStr()(val));   // 应使用 setValue() 函数以触发回调函数，而不是直接操作 m_val
            return true;
        } catch(std::exception& e) {    
            SHS_LOG_ERROR(SHS_LOG_ROOT()) << "ConfigVar::fromString exception " << e.what() << " convert: string to" << TypeToName<T>() << " name=" << getName() << " - " << val; 
        }
        return false;
    } 
    std::string getTypeName() const override { return TypeToName<T>();}
    const T getValue() const { return m_val; }
    void setValue(const T& val) {
        // SHS_LOG_INFO(SHS_LOG_ROOT()) << "setValue()";
        if(m_val == val) 
            return;
        // SHS_LOG_INFO(SHS_LOG_ROOT()) << "m_cbs.size(): " << m_cbs.size();
        for(auto& i: m_cbs) {
            i.second(m_val, val);
        }
        m_val = val;
    }

    void addListener(uint64_t key, on_change_cb cb) {
        // SHS_LOG_INFO(SHS_LOG_ROOT()) << "addListener()";
        m_cbs.insert({key, cb});    // insert 需要 key 原本不存在才会添加
    }

    void delListener(uint64_t key) {
        m_cbs.erase(key);   // 若 key 不存在，不会产生异常，会返回0
    }

    on_change_cb getListener(uint64_t key) {
        auto it = m_cbs.find(key);
        return it==m_cbs.end() ? nullptr : it->second;
    }
private:
    T m_val;
    // 变更回调函数组, uint64_t key,要求唯一，一般可以用hash
    std::map<uint64_t, on_change_cb> m_cbs;
};

class Config {
public:
    using ConfigVarMap = std::map<std::string, ConfigVarBase::ptr>;

    template<typename T>
    static typename ConfigVar<T>::ptr Lookup(const T& default_value, const std::string& name, const std::string& description="") {

        auto it = GetDatas().find(name);
        if(it != GetDatas().end()) {
            auto ptr = std::dynamic_pointer_cast<ConfigVar<T>>(it->second);
            if(ptr) {
                SHS_LOG_INFO(SHS_LOG_ROOT()) << "Looup name: " << name << " exists";
                return ptr;
            } else {
                SHS_LOG_ERROR(SHS_LOG_ROOT()) << "Lookup name=" << name << " exists but type not "
                        << TypeToName<T>() << " real_type=" << it->second->getTypeName()
                        << " " << it->second->toString();
                return nullptr;
            }
        } else {
            if(name.find_first_not_of("abcdefghijklmnopqrstuvwxyz._1234567890") != name.npos) {
                SHS_LOG_ERROR(SHS_LOG_ROOT()) << "Lookup name invalid " << name;
                throw std::invalid_argument(name);
            }

            // auto ptr = std::make_shared<ConfigVar<T>>(default_value, name, description);
            auto ptr = std::make_shared<ConfigVar<T>>(default_value, name, description);
            if (!ptr) {
                SHS_LOG_ERROR(SHS_LOG_ROOT()) << "Generated ptr is nullptr for " << name;
                return nullptr;
            }

            try {
                auto result = GetDatas().insert({name, ptr});
                if (!result.second) {
                    SHS_LOG_ERROR(SHS_LOG_ROOT()) << "Failed to insert new config var";
                    return nullptr;
                }
            } catch (const std::exception& e) {
                SHS_LOG_ERROR(SHS_LOG_ROOT()) << "Exception during insertion: " << e.what() << std::endl;
                return nullptr;
            }
            return ptr;
        }

    }

    template<typename T>
    static typename ConfigVar<T>::ptr Lookup(const std::string& name) {
        auto it = GetDatas().find(name);
        if(it == GetDatas().end()) {
            return nullptr;
        }
        return std::dynamic_pointer_cast<ConfigVar<T>>(it->second);
    }
    
    // 使用 YAML::Node 初始化配置模块
    static void LoadFromYaml(const YAML::Node& root);
    // 查询配置参数，返回配置参数的基类
    static ConfigVarBase::ptr LookupBase(const std::string& name);


    // static void debug() {
    //     std::cout << "m_datas: size=" << m_datas.size() << "\n";
    //     for(const auto& i: m_datas) {
    //         std::cout << "\t" << i.first << std::endl;
    //     }
    // }
private:
    // static std::mutex m_mutex;  // 添加互斥锁

    // static ConfigVarMap m_datas;
    static ConfigVarMap& GetDatas() {
        static ConfigVarMap s_datas;
        return s_datas;
    }
};


}