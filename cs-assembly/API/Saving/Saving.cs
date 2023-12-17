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

        public static bool SaveVariable<T>(string save_file_path, string variable_name, T value)
        {
            if (typeof(T) == typeof(int))
            {
                return cpp_SaveVariable(save_file_path, variable_name, ((int)(object)value).ToString());
            }
            else if (typeof(T) == typeof(float))
            {
                return cpp_SaveVariable(save_file_path, variable_name, Format.ToString((float)(object)value));
            }
            else if (typeof(T) == typeof(double))
            {
                return cpp_SaveVariable(save_file_path, variable_name, Format.ToString((double)(object)value));
            }
            else if (typeof(T) == typeof(string))
            {
                return cpp_SaveVariable(save_file_path, variable_name, (string)(object)value);

            }
            else if (typeof(T) == typeof(long))
            {
                return cpp_SaveVariable(save_file_path, variable_name, ((long)(object)value).ToString());
            }
            else if (typeof(T) == typeof(Int64))
            {
                return cpp_SaveVariable(save_file_path, variable_name, ((Int64)(object)value).ToString());
            }
            else if (typeof(T) == typeof(Vector2))
            {
                Vector2 val = (Vector2)(object)value;
                string to_save = Format.ToString(val.x) + "," + Format.ToString(val.y);
                return cpp_SaveVariable(save_file_path, variable_name, to_save);
            }
            else if (typeof(T) == typeof(Vector3))
            {
                Vector3 val = (Vector3)(object)value;
                string to_save = Format.ToString(val.x) + "," + Format.ToString(val.y) + "," + Format.ToString(val.z);
                return cpp_SaveVariable(save_file_path, variable_name, to_save);
            }
            else if (typeof(T) == typeof(Vector4))
            {
                Vector4 val = (Vector4)(object)value;
                string to_save = Format.ToString(val.x) + "," + Format.ToString(val.y) + "," + Format.ToString(val.z) + "," + Format.ToString(val.w);
                return cpp_SaveVariable(save_file_path, variable_name, to_save);
            }
            else
            {
                Debug.Error("SaveVariable: Invalid type to save!");
            }
            return false;
        }

        public static T? GetVariable<T>(string save_file_path, string variable_name)
        {
            string val = cpp_GetVariable(save_file_path, variable_name);
            if (val == "INVALID") return default(T);

            if (typeof(T) == typeof(int))
            {
                return (T)(object)int.Parse(val);
            }
            else if (typeof(T) == typeof(float))
            {
                return (T)(object)float.Parse(val);
            }
            else if (typeof(T) == typeof(double))
            {
                return (T)(object)double.Parse(val);
            }
            else if (typeof(T) == typeof(string))
            {
                return (T)(object)val;
            }
            else if (typeof(T) == typeof(long))
            {
                return (T)(object)long.Parse(val);
            }
            else if (typeof(T) == typeof(Int64))
            {
                return (T)(object)Int64.Parse(val);
            }
            else if (typeof(T) == typeof(Vector2))
            {
                string[] ss = val.Split(",");
                return (T)(object)new Vector2(float.Parse(ss[0]), float.Parse(ss[1]));
            }
            else if (typeof(T) == typeof(Vector3))
            {
                string[] ss = val.Split(",");
                return (T)(object)new Vector3(float.Parse(ss[0]), float.Parse(ss[1]), float.Parse(ss[2]));
            }
            else if (typeof(T) == typeof(Vector4))
            {
                string[] ss = val.Split(",");
                return (T)(object)new Vector4(float.Parse(ss[0]), float.Parse(ss[1]), float.Parse(ss[2]), float.Parse(ss[3]));
            }
            else
            {
                Debug.Error("GetVariable: Invalid type to get!");
                return default(T);
            }
        }
    }
}