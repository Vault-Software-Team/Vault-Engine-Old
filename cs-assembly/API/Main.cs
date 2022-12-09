using System.Runtime.CompilerServices;
using System;

namespace Vault
{
    public class Terminal
    {
        public Terminal() { }

        [MethodImplAttribute(MethodImplOptions.InternalCall)]
        extern public static void Log(string text);
    }

    public class Debug
    {
        public Debug() { }

        [MethodImplAttribute(MethodImplOptions.InternalCall)]
        extern public static void Log(string text);

        [MethodImplAttribute(MethodImplOptions.InternalCall)]
        extern public static void Error(string text);

        [MethodImplAttribute(MethodImplOptions.InternalCall)]
        extern public static void Warning(string text);

        [MethodImplAttribute(MethodImplOptions.InternalCall)]
        extern public static void TestVector(ref Vector3 parameter);
    }

    public class Audio
    {
        public Audio() { }

        [MethodImplAttribute(MethodImplOptions.InternalCall)]
        extern public static void Play(string file, float volume, bool loop, int channel = -1);

        [MethodImplAttribute(MethodImplOptions.InternalCall)]
        extern public static void Music(string file, float volume, bool loop);
    }

    public class Main
    {
        public Main()
        {
            Vector3 vector = new Vector3(1, 2, 3);
        }
    }

    public class Entity
    {
        [MethodImplAttribute(MethodImplOptions.InternalCall)]
        extern public static void GetID(out string result);

        public string parentID;
        public string ID;
        // public T GetComponent<T>() where T : Component, new()
        // {

        // }

        protected virtual void OnStart() { }
        protected virtual void OnUpdate(float ts) { }

        public T GetComponent<T>() where T : Component, new()
        {
            if (typeof(T) == typeof(Transform))
            {
                Transform comp = new Transform() { Entity = this };
                return (T)(object)comp;
            }

            return null;
        }
    }
}