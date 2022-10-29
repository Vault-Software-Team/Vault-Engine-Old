camera = {}
fx_target = "Player"
fx_speed = 5

function camera:OnStart()
end

function camera:OnUpdate()
    local target = FindGameObjectByName(fx_target);
    local target_transform = target:GetComponent("Transform");

    local transform = GetComponent("Transform");
    transform.positionX = Float.Lerp(transform.positionX, target_transform.positionX, Timestep.DeltaTime * fx_speed);
    transform.positionY = Float.Lerp(transform.positionY, target_transform.positionY, Timestep.DeltaTime * fx_speed);

    UpdateComponent(transform);
end