function OnStart()
end

Log("Rotating the cube");
function OnUpdate()
    Component = GetComponent("Transform");

    Component.rotationX = Component.rotationX + 1;
    Component.rotationY = Component.rotationY + 1;
    Component.rotationZ = Component.rotationZ + 1;

    if Component.rotationX > 360 then
        Component.rotationX = 0;
    end

    if Component.rotationY > 360 then
        Component.rotationY = 0;
    end

    if Component.rotationZ > 360 then
        Component.rotationZ = 0;
    end

    
    UpdateComponent(Component);
end
