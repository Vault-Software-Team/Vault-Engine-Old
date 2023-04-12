using System;
using System.Runtime.CompilerServices;
using Vault;

namespace Vault
{
    public class Rigidbody3D : Component
    {
        [MethodImplAttribute(MethodImplOptions.InternalCall)]
        extern public static void cpp_GetKey(string key, string id, out string result);

        [MethodImplAttribute(MethodImplOptions.InternalCall)]
        extern public static void cpp_AddForce(float x, float y, float z, string id);

        [MethodImplAttribute(MethodImplOptions.InternalCall)]
        extern public static void cpp_AddTorque(float x, float y, float z, string id);

        [MethodImplAttribute(MethodImplOptions.InternalCall)]
        extern public static void cpp_AddForceAtPosition(float x, float y, float z, float px, float py, float pz, string id);

        [MethodImplAttribute(MethodImplOptions.InternalCall)]
        extern public static void cpp_SetVelocity(float x, float y, float z, string id);

        [MethodImplAttribute(MethodImplOptions.InternalCall)]
        extern public static void cpp_SetAngularVelocity(float x, float y, float z, string id);

        [MethodImplAttribute(MethodImplOptions.InternalCall)]
        extern public static void cpp_SetPosition(float x, float y, float z, string id);


        public void AddForce(Vector3 force)
        {
            cpp_AddForce(force.x, force.y, force.z, Entity.ID);
        }

        public void AddTorque(Vector3 torque)
        {
            cpp_AddTorque(torque.x, torque.y, torque.z, Entity.ID);
        }

        public void AddForceAtPosition(Vector3 force, Vector3 pos)
        {
            cpp_AddForceAtPosition(force.x, force.y, force.z, pos.x, pos.y, pos.z, Entity.ID);
        }

        public void SetVelocity(Vector3 velocity)
        {
            cpp_SetVelocity(velocity.x, velocity.y, velocity.z, Entity.ID);
        }

        public Vector3 GetVelocity()
        {
            cpp_GetKey("velocity", Entity.ID, out string result);
            string[] strs = result.Split(" ");
            return new Vector3(float.Parse(strs[0]), float.Parse(strs[1]), float.Parse(strs[2]));
        }

        public void SetAngularVelocity(Vector3 AngularVelocity)
        {
            cpp_SetAngularVelocity(AngularVelocity.x, AngularVelocity.y, AngularVelocity.z, Entity.ID);
        }

        public Vector3 GetAngularVelocity()
        {
            cpp_GetKey("AngularVelocity", Entity.ID, out string result);
            string[] strs = result.Split(" ");
            return new Vector3(float.Parse(strs[0]), float.Parse(strs[1]), float.Parse(strs[2]));
        }

        public void SetPosition(Vector3 pos)
        {
            cpp_SetPosition(pos.x, pos.y, pos.z, Entity.ID);
        }
    }
}