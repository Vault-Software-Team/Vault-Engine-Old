enemy = {}
fx_target = "Player"
fx_speed = 0.8;

function enemy:OnStart()
end

function enemy:OnUpdate()
    local target = FindGameObjectByName(fx_target);
    local target_transform = target:GetComponent("Transform");

    local transform = GetComponent("Transform");
    transform.positionX = Float.Lerp(transform.positionX, target_transform.positionX, Timestep.DeltaTime * fx_speed);
    transform.positionY = Float.Lerp(transform.positionY, target_transform.positionY, Timestep.DeltaTime * fx_speed);
    UpdateComponent(transform);
end