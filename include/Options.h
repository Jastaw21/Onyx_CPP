//
// Created by jacks on 06/03/2026.
//

#ifndef ONYXCPP_OPTIONS_H
#define ONYXCPP_OPTIONS_H
#include <iostream>
#include <ostream>
#include <string>
#include <variant>
#include <vector>
#include "types.h"


struct Option {
    std::string name;
    OptionValue value;
    int min;
    int max;
    OptionValue defaultValue;
    bool isValid = true;

    std::string toOutput() const{
        std::string typeString;
        std::string defaultString;
        bool isInt = std::holds_alternative<int>(value);
        if (isInt) {
            typeString = "spin";
            defaultString = std::to_string(std::get<int>(value));
        } else if (std::holds_alternative<bool>(value)) {
            typeString = "check";
            bool val = std::get<int>(value);
            defaultString = val ? "true" : "false";
        } else if (std::holds_alternative<std::string>(value)) {
            typeString = "string";
            defaultString = std::get<std::string>(value);
        }

        std::string outputString = "option name " + name + " type " + typeString + " default " + defaultString;
        if (isInt) {
            outputString += " min " + std::to_string(min);
            outputString += " max " + std::to_string(max);
        }
        return outputString;
    }
};

class Options {
public:

    Options(){
        options_.push_back(
            {
                "Null Option", "Null option", 0, 0, "Null Option", false
            }
        );
    }

    Option& operator[](const std::string& name){
        for (Option& opt: options_) {
            if (!opt.isValid) continue;
            if (opt.name == name)
                return opt;
        }
        return options_.at(0);
    }

    void write() const{
        for (const auto& opt: options_) {
            if (!opt.isValid) continue;
            std::cout << opt.toOutput() << std::endl;
        }
    }

    void addOption(const Option& option){ if (!contains(option.name)) { options_.push_back(option); } }

    void setValue(const std::string& optionName, const OptionValue& value){
        Option& opt = operator[](optionName);
        if (!opt.isValid) return;

        if (std::holds_alternative<int>(value)) {
            int asInt = std::get<int>(value);
            if (asInt >= opt.min && asInt <= opt.max)
                opt.value = asInt;
            else
                std::cerr << "Out of range\n";
        }
        else {
            opt.value = value;
        }
    }

    Option* begin() {return options_.data();}
    Option* end() {return options_.data() + options_.size();}
    const Option* begin() const {return options_.data();}
    const Option* end() const {return options_.data() + options_.size();}

private:

    bool contains(const std::string& name) const{
        for (const auto& opt: options_) {
            if (opt.name == name)
                return true;
        }
        return false;
    }

    std::vector<Option> options_{};
};

#endif //ONYXCPP_OPTIONS_H