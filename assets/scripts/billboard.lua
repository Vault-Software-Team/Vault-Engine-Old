function OnStart()
end

function OnUpdate()
    local transform = GetComponent("Transform");
    local camera = FindGameObjectByName("Camera");
    local camTransform = camera:GetComponent("Transform");

    Log("Updated before!");
    transform:LookAt(camTransform.positionX, camTransform.positionY, camTransform.positionZ);
    Log("Updated!");
end