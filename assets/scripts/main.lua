function OnStart()
end

SpacePressed = false
function OnUpdate()
    local rigidbody = GetComponent("Rigidbody2D");
    local transform = GetComponent("Transform");
    
    if IsKeyPressed(KEY_SPACE) and SpacePressed == false and transform.positionY < 1 then
        rigidbody:Force(0, 2500);
        SpacePressed = true;
    elseif IsKeyPressed(KEY_SPACE) == false then
        SpacePressed = false;
    end
    
    UpdateComponent(rigidbody);
end

function Collision2D(other)
    Log("Collision with " .. other.name);
end

function CollisionExit2D(other)
    Log("Collision exited with " .. other.name);
end