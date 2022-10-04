function OnStart()
end

function OnUpdate()
    local transform = GetComponent("Transform");
    local camera = GetComponent("CameraComponent");
    camera:MouseInput();
end