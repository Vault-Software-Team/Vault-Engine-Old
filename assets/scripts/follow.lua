follow = {}
fx_target = "Player"

function follow:OnStart()
end

function follow:OnUpdate()
    local target = FindGameObjectByName(fx_target);
    local target_transform = target:GetComponent("Transform");

    local transform = GetComponent("Transform");
    transform.positionX = target_transform.positionX;
    transform.positionZ = target_transform.positionZ + 0.5;

    UpdateComponent(transform);
end