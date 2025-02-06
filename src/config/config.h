#pragma once 
#include <string>
#include <memory>
#include <typeinfo>
#include <map>
#include <boost/lexical_cast.hpp>
#include "../_log/macro.h"
#include "../util/util.h"
#include <yaml-cpp/yaml.h>
// #include <iostream>

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
class ConfigVar: public ConfigVarBase {
public:
    using ptr = std::shared_ptr<ConfigVar>;

    ConfigVar(const T& default_value, const std::string& name, const std::string& description=""): ConfigVarBase(name, description), m_val(default_value) {}

    std::string toString() override {
        try {
            // return boost::lexical_cast<std::string>(m_val);
            return Lexical_cast<T, std::string>()(m_val);
        } catch(std::exception& e) {    
            SHS_LOG_ERROR(SHS_LOG_ROOT()) << "ConfigVar::toString exception " << e.what() << " convert: " << typeid(m_val).name() << " to string";
        }
        return {};
    }

    bool fromString(const std::string& val) override {
        try {
            // m_val = boost::lexical_cast<T>(val);
            m_val = Lexical_cast<std::string, T>()(val);
            return true;
        } catch(std::exception& e) {    
            SHS_LOG_ERROR(SHS_LOG_ROOT()) << "ConfigVar::fromString exception " << e.what() << " convert: string to" << typeid(m_val).name();
        }
        return false;
    } 
    std::string getTypeName() const override { return TypeToName<T>();}
    const T getValue() const { return m_val; }
    void setValue(const T& val) { m_val = val; }

private:
    T m_val;
};

class Config {
public:
    using ConfigVarMap = std::map<std::string, ConfigVarBase::ptr>;

    template<typename T>
    static typename ConfigVar<T>::ptr Lookup(const T& default_value, const std::string& name, const std::string& description="") {
        auto tmp = Lookup<T>(name);
        if(tmp) {
            SHS_LOG_INFO(SHS_LOG_ROOT()) << "Looup name: " << name << " exists";
            return tmp;
        }

        if(name.find_first_not_of("abcdefghijklmnopqrstuvwxyz._1234567890") != name.npos) {
            SHS_LOG_ERROR(SHS_LOG_ROOT()) << "Lookup name invalid " << name;
            throw std::invalid_argument(name);
        }

        auto ptr = std::make_shared<ConfigVar<T>>(default_value, name, description);
        m_datas[name] = ptr;
        return ptr;
    }

    template<typename T>
    static typename ConfigVar<T>::ptr Lookup(const std::string& name) {
        auto it = m_datas.find(name);
        if(it == m_datas.end()) {
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
    static ConfigVarMap m_datas;
};


}