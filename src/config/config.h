#pragma once 
#include <string>
#include <memory>
#include <typeinfo>
#include <map>
#include <boost/lexical_cast.hpp>
#include "../_log/macro.h"
#include "../util/util.h"

namespace shs {
// 配置变量的基类
class ConfigVarBase {
public:
    using ptr = std::shared_ptr<ConfigVarBase>;
    ConfigVarBase(const std::string& name, const std::string& description=""): m_name(name), m_description(description){
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


template<typename T>
class ConfigVar: public ConfigVarBase {
public:
    using ptr = std::shared_ptr<ConfigVar>;

    ConfigVar(const T& default_value, const std::string& name, const std::string& description=""): ConfigVarBase(name, description), m_val(default_value) {}

    std::string toString() override {
        try {
            return boost::lexical_cast<std::string>(m_val);
        } catch(std::exception& e) {    
            SHS_LOG_ERROR(SHS_LOG_ROOT()) << "ConfigVar::toString exception " << e.what() << " convert: " << typeid(m_val).name() << " to string";
        }
        return {};
    }

    bool fromString(const std::string& val) override {
        try {
            m_val = boost::lexical_cast<T>(val);
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

        if(name.find_first_not_of("abcdefghijklmnopqrstuvwxyzABCDEFGHIJKLMNOPQRSTUVWXYZ._1234567890") != name.npos) {
            SHS_LOG_ERROR(SHS_LOG_ROOT()) << "Lookup name invalid " << name;
            throw std::invalid_argument(name);
        }

        return std::make_shared<ConfigVar<T>>(default_value, name, description);
    }

    template<typename T>
    static typename ConfigVar<T>::ptr Lookup(const std::string& name) {
        auto it = m_datas.find(name);
        if(it == m_datas.end()) {
            return nullptr;
        }
        return std::dynamic_pointer_cast<ConfigVar<T>>(it->second);
    }
    
private:
    static ConfigVarMap m_datas;
};


}