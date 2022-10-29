new_script = {}

function new_script:OnStart()
end

fx_time = 5

function new_script:OnUpdate()
	fx_time = fx_time - Timestep.DeltaTime
	
	if fx_time <= 0 then
		InstantiatePrefab("assets/GameObject.prefab")
		fx_time = 5000;
	end
end
