function OnStart()
end

function OnUpdate()
    local transform = GetComponent("Transform");
    transform.positionX = transform.positionX - 0.01;
    if transform.positionX < -2.6 then
        -- generate a random float between -0.5 and -0.1
        local random = math.random() * 0.4 - 0.5;
        transform.positionY = random;
        transform.positionX = 2.3;
    end
    
    UpdateComponent(transform);
end