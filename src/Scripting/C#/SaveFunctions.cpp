#include "SaveFunctions.hpp"
#include "csharp.hpp"
#include "mono/metadata/object.h"
#include "scene.hpp"

namespace HyperAPI::CsharpScriptEngine::Functions {
    // Probably really bad for optimization stuff but i really do not care lmfaooo
    MonoString *Save_GetVariable(MonoString *ms_save_file, MonoString *ms_variable_name) {
        const std::string save_file = mono_string_to_utf8(ms_save_file);
        const std::string variable_name = mono_string_to_utf8(ms_variable_name);

        std::ifstream file(save_file);
        std::string line, var_name = "\0", var_value = "\0";

        while (std::getline(file, line)) {
            if (line.starts_with(variable_name)) {
                std::stringstream ss(line);
                int i = 0;
                std::string m_line;
                while (getline(ss, m_line, ':')) {
                    if (i == 0) {
                        var_name = m_line;
                        i++;
                    } else {
                        var_value = m_line;
                    }
                }
                break;
            }
        }
        file.close();
        if (var_value == "\0" || var_name == "\0")
            return mono_string_new(CsharpVariables::appDomain, "INVALID");

        return mono_string_new(CsharpVariables::appDomain, var_value.c_str());
    }

    bool Save_SaveVariable(MonoString *ms_save_file, MonoString *ms_variable_name, MonoString *ms_value) {
        const std::string save_file = mono_string_to_utf8(ms_save_file);
        const std::string variable_name = mono_string_to_utf8(ms_variable_name);
        const std::string value = mono_string_to_utf8(ms_value);

        if (fs::exists(save_file)) {
            std::vector<std::string> to_write;
            std::ifstream ifile(save_file);
            std::string line;

            while (std::getline(ifile, line)) {
                if (line.starts_with(variable_name)) {
                    continue;
                }

                to_write.push_back(line);
            }
            to_write.push_back(variable_name + ":" + value);
            ifile.close();
            std::ofstream ofile(save_file);

            for (auto &str : to_write) {
                ofile << str;
                ofile << "\n";
            }
            ofile.close();
        } else {
            std::fstream file(save_file, std::ios_base::out);
            file << variable_name + ":" + value;
            file << "\n";
            file.close();
        }
        return true;
    }

    bool Save_DeleteVariable(MonoString *ms_save_file, MonoString *ms_variable_name) {
        const std::string save_file = mono_string_to_utf8(ms_save_file);
        const std::string variable_name = mono_string_to_utf8(ms_variable_name);

        std::fstream file(save_file, std::ios_base::in | std::ios_base::out);
        std::vector<std::string> to_write;
        std::string line;

        while (std::getline(file, line)) {
            if (line.starts_with(variable_name))
                continue;

            to_write.push_back(line);
        }

        for (auto &str : to_write) {
            file << str;
            file << "\n";
        }
        file.close();
        return true;
    }
} // namespace HyperAPI::CsharpScriptEngine::Functions