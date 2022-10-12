function OnStart()
    PlayAudio("assets/guns/Inst.ogg", 0.5, true)
end

function OnUpdate()
    local transform = GetComponent("Transform");
    local camera = GetComponent("CameraComponent");
    camera:MouseInput();
end