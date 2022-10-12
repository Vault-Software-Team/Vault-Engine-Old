function OnStart()
end

jumped = false
function OnUpdate()
    local rigidbody = GetComponent("Rigidbody3D");
    local transform = GetComponent("Transform");

    -- move forward, transform.forwardX, transform.forwardY, transform.forwardZ
    if Input.IsKeyPressed(KEY_W) then
        rigidbody:AddForce(transform.forwardX * 10, 0, transform.forwardZ * 10);
    end

    -- move backward, transform.forwardX, transform.forwardY, transform.forwardZ
    if Input.IsKeyPressed(KEY_S) then
        rigidbody:ApplyForce(transform.forwardX * -10, 0, transform.forwardZ * -10);
    end

    -- move left, transform.rightX, transform.rightY, transform.rightZ
    if Input.IsKeyPressed(KEY_A) then
        rigidbody:ApplyForce(transform.rightX * -10, 0, transform.rightZ * -10);
    end

    -- move right, transform.rightX, transform.rightY, transform.rightZ
    if Input.IsKeyPressed(KEY_D) then
        rigidbody:ApplyForce(transform.rightX * 10, 0, transform.rightZ * 10);
    end

    -- jump
    if Input.IsKeyPressed(KEY_SPACE) and not jumped then
        rigidbody:ApplyForce(0, 10, 0);
        jumped = true;
    end

    if Input.IsKeyReleased(KEY_SPACE) then
        jumped = false;
    end


end

function Collision3D(collider)
    jumped = false;
end