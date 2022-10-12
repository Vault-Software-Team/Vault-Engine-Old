function OnStart()
end

function OnUpdate()
    local objects = FindGameObjectsByTag("Default");
    print(objects.size);
end