function OnStart()
end

function OnUpdate()
    local transform = GetComponent("Transform");

    if IsKeyPressed(KEY_W) then
        transform.rotationX = transform.rotationX + 1;
    end

    if IsKeyPressed(KEY_S) then
        transform.rotationX = transform.rotationX - 1;
    end

    if IsKeyPressed(KEY_A) then
        transform.rotationY = transform.rotationY + 1;
    end

    if IsKeyPressed(KEY_D) then
        transform.rotationY = transform.rotationY - 1;
    end

    UpdateComponent(transform);
end
