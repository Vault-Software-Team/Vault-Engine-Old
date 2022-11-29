player = {}
fx_speed = 5
fx_health = 100
fx_cycleSpeed = 5;

local defaultScale = 1;
function player:OnStart()
end

time = 3;
isDay = true;
local calledOnce = false;
function player:OnUpdate()
    local rigidbody = GetComponent("Rigidbody3D");
    local transform = GetComponent("Transform");

    if calledOnce == false then
        defaultScale = transform.scaleX;
        calledOnce = true;
    end

    if isDay then
        Vault.Ambient = Vault.Ambient - Timestep.DeltaTime * fx_cycleSpeed;
    else
        Vault.Ambient = Vault.Ambient + Timestep.DeltaTime * fx_cycleSpeed;
    end

    if Vault.Ambient <= 0 then
        isDay = false;
    elseif Vault.Ambient >= 1 then
        isDay = true;
    end

    local Xaxis = 0;
    local Zaxis = 0;

    if Input.IsKeyPressed(KEY_W) then
        rigidbody:SetVelocity(Xaxis, 0, -fx_speed * Timestep.DeltaTime);
        Zaxis = -fx_speed * Timestep.DeltaTime;
    elseif Input.IsKeyPressed(KEY_S) then
        rigidbody:SetVelocity(Xaxis, 0, fx_speed * Timestep.DeltaTime);
        Zaxis = fx_speed * Timestep.DeltaTime;
    else
        Zaxis = 0;
    end

    if Input.IsKeyPressed(KEY_A) then
        rigidbody:SetVelocity(-fx_speed * Timestep.DeltaTime, 0, Zaxis);
        Xaxis = -fx_speed * Timestep.DeltaTime;
        transform.scaleX = Float.Lerp(transform.scaleX, -defaultScale, Timestep.DeltaTime * 8);
        UpdateComponent(transform);
    elseif Input.IsKeyPressed(KEY_D) then
        rigidbody:SetVelocity(fx_speed * Timestep.DeltaTime, 0, Zaxis * Timestep.DeltaTime);
        Xaxis = fx_speed * Timestep.DeltaTime;
        transform.scaleX = Float.Lerp(transform.scaleX, defaultScale, Timestep.DeltaTime * 8);
        UpdateComponent(transform);
    else
        Xaxis = 0;
    end
end