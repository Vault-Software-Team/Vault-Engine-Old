function OnStart()
end

jumped = false
function OnUpdate()
    local rigidbody = GetComponent("Rigidbody3D");
    local transform = GetComponent("Transform");

    -- move forward
    if IsKeyPressed(KEY_W) then
        rigidbody:SetVelocity(0, 0, 15);
    elseif IsKeyPressed(KEY_S) then
        rigidbody:SetVelocity(0, 0, -15);
    elseif IsKeyPressed(KEY_A) then
        rigidbody:SetVelocity(15, 0, 0);
    elseif IsKeyPressed(KEY_D) then
        rigidbody:SetVelocity(-15, 0, 0);
    elseif IsKeyPressed(KEY_SPACE) and jumped == false then
        rigidbody:SetVelocity(0, 15, 0);
        jumped = true;
    end
end

function Collision3D(collider)
    jumped = false;
end