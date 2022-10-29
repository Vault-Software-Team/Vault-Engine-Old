player = {}
fx_speed = 5
fx_health = 100
fx_cycleSpeed = 5;
 
function player:OnStart()
--     local gameObject = InstantiatePrefab("assets/Object.prefab")
--     local transform = gameObject:GetComponent("Transform");
--     local randomX = math.random(-44, 44);
--     local randomY = math.random(-21, 21);
--
--     transform.positionX = randomX;
--     transform.positionY = randomY;
--     gameObject:UpdateComponent(transform);
--     -- for loop 10 timesr
end

time = 3;
isDay = true;
function player:OnUpdate()
    local rigidbody = GetComponent("Rigidbody2D");
    local transform = GetComponent("Transform");
    local velocity = rigidbody:GetVelocity();

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

    if Input.IsKeyPressed(KEY_A) then
        rigidbody:SetVelocity(-fx_speed, velocity.y);
        transform.scaleX = Float.Lerp(transform.scaleX, 2, Timestep.DeltaTime * 15);
        UpdateComponent(transform);
    elseif Input.IsKeyPressed(KEY_D) then
        rigidbody:SetVelocity(fx_speed, velocity.y);
        transform.scaleX = Float.Lerp(transform.scaleX, -2, Timestep.DeltaTime * 15);
        UpdateComponent(transform);
    else
        rigidbody:SetVelocity(0, velocity.y);
    end

    local velocity2 = rigidbody:GetVelocity();

    if Input.IsKeyPressed(KEY_W) then
        rigidbody:SetVelocity(velocity2.x, fx_speed);
    elseif Input.IsKeyPressed(KEY_S) then
        rigidbody:SetVelocity(velocity2.x, -fx_speed);
    else
        rigidbody:SetVelocity(velocity2.x, 0);
    end
end

function player:OnCollision2D()
	if collidedWith.tag == "Enemy" then
        Log("Player collided with enemy");
    end
end























