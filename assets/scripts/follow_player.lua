function OnStart()
end

function OnUpdate()
    local player = FindGameObjectByName("Player");
    local playerTransform = player:GetComponent("Transform");
    local transform = GetComponent("Transform");

    transform.positionX = playerTransform.positionX;
    transform.positionY = playerTransform.positionY;

    UpdateComponent(transform);
end