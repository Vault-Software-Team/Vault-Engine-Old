camera = {}
fx_target = "Player"
fx_speed = 5
fx_offset = 5

function camera:OnStart()
end

local offsetX = 0;
local offsetY = 0;
local rotate = false;

local offsetTo = {
    x = 0,
    y = 0,
    xRot = 0,
    yRot = 0,
    yObjRot = 0,
}

function camera:OnUpdate()
    fx_offset = tonumber(fx_offset);
    local target = FindGameObjectByName(fx_target);
    local target_transform = target:GetComponent("Transform");

    local transform = GetComponent("Transform");
    transform.positionX = Float.Lerp(transform.positionX, target_transform.positionX - offsetX, Timestep.DeltaTime * fx_speed);
    transform.positionZ = Float.Lerp(transform.positionZ, target_transform.positionZ - offsetY - fx_offset, Timestep.DeltaTime * fx_speed);

    -- loop through objects
    
    if Input.IsKeyPressed(KEY_E) and rotate == false then
        rotate = true;
        offsetTo.x = offsetTo.x - 5;
        offsetTo.y = offsetTo.y + 5;
        offsetTo.xRot = offsetTo.xRot + ToRadians(-90);
        offsetTo.yRot = offsetTo.yRot + ToRadians(-24.30);
        offsetTo.yObjRot = offsetTo.yObjRot + ToRadians(90);
        
    elseif Input.IsKeyPressed(KEY_E) == false and rotate == true then
        rotate = false;
    end
    
    if rotate == true then
        local objects = FindGameObjectsByTag("Enviroment");
        for _, object in ipairs(objects) do
            local object_transform = object:GetComponent("Transform");
            object_transform.rotationY = Float.Lerp(object_transform.rotationY, offsetTo.yObjRot, Timestep.DeltaTime * fx_speed);
            object:UpdateComponent(object_transform);
        end
        offsetX = Float.Lerp(offsetX, offsetTo.x, Timestep.DeltaTime * fx_speed);
        offsetY = Float.Lerp(offsetY, offsetTo.y, Timestep.DeltaTime * fx_speed);

        target_transform.rotationY = Float.Lerp(target_transform.rotationY, offsetTo.yObjRot, Timestep.DeltaTime * fx_speed);
        target:UpdateComponent(target_transform);
        transform.rotationX = Float.Lerp(transform.rotationX, offsetTo.xRot, Timestep.DeltaTime * fx_speed);
        transform.rotationY = Float.Lerp(transform.rotationY, offsetTo.yRot, Timestep.DeltaTime * fx_speed);
    end

    UpdateComponent(transform);
end