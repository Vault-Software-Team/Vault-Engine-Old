using System.Runtime.CompilerServices;
using System;

namespace Vault
{
    public class SaveFile
    {
        [MethodImplAttribute(MethodImplOptions.InternalCall)]
        extern public static string cpp_GetVariable(string save_file_path, string variable_name);

        [MethodImplAttribute(MethodImplOptions.InternalCall)]
        extern public static bool cpp_SaveVariable(string save_file_path, string variable_name, string value);

        [MethodImplAttribute(MethodImplOptions.InternalCall)]
        extern public static bool cpp_DeleteVariable(string save_file_path, string variable_name);

        public static bool SaveVariable(string save_file_path, string variable_name, string value)
        {
            return cpp_SaveVariable(save_file_path, variable_name, value);
        }

        public static bool DeleteVariable(string save_file_path, string variable_name)
        {
            return cpp_DeleteVariable(save_file_path, variable_name);
        }

        public static T? GetVariable<T>(string save_file_path, string variable_name)
        {
            string val = cpp_GetVariable(save_file_path, variable_name);
            if (val == "INVALID") return default(T);

            if (typeof(T) == typeof(int))
            {
                return (T)(object)int.Parse(val);
            }
            if (typeof(T) == typeof(float))
            {
                return (T)(object)float.Parse(val);
            }
            if (typeof(T) == typeof(double))
            {
                return (T)(object)double.Parse(val);
            }
            if (typeof(T) == typeof(string))
            {
                return (T)(object)val;
            }
            if (typeof(T) == typeof(long))
            {
                return (T)(object)long.Parse(val);
            }
            if (typeof(T) == typeof(Int64))
            {
                return (T)(object)Int64.Parse(val);
            }
            else
            {
                Debug.Error("GetVariable: Only int, float, double and string are complete types!");
                return default(T);
            }
        }
    }
}