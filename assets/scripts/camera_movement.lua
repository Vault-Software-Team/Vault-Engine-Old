camera_movement = {}
fx_sensitivity = 5

camera_movement.OnStart = function()
end

camera_movement.OnUpdate = function()
    local transform = GetComponent("Transform");
    local camera = GetComponent("CameraComponent");

    camera:ControllerInput(fx_sensitivity);

--     if Input.Controller.GetLeftAnalogY() < -0.1 then
--         transform.positionX = transform.positionX + -(Input.Controller.GetLeftAnalogY() * fx_sensitivity * transform.rotationX);
--         transform.positionY = transform.positionY + -(Input.Controller.GetLeftAnalogY() * fx_sensitivity * transform.rotationY);
--         transform.positionZ = transform.positionZ + -(Input.Controller.GetLeftAnalogY() * fx_sensitivity * transform.rotationZ);
--
--         UpdateComponent(transform)
--     end
--
--     if Input.Controller.GetLeftAnalogY() > 0.1 then
--         transform.positionX = transform.positionX + -(Input.Controller.GetLeftAnalogY() * fx_sensitivity * transform.rotationX);
--         transform.positionY = transform.positionY + -(Input.Controller.GetLeftAnalogY() * fx_sensitivity * transform.rotationY);
--         transform.positionZ = transform.positionZ + -(Input.Controller.GetLeftAnalogY() * fx_sensitivity * transform.rotationZ);
--
--         UpdateComponent(transform)
--     end
end
