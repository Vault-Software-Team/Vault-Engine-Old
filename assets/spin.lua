spin = {}

function spin:OnStart()
	PlayMusic("assets/funkytown.mp3", 1, true);
end

function spin:OnUpdate()
	local transform = GetComponent("Transform");
	transform.rotationY = transform.rotationY + Timestep.DeltaTime;
	UpdateComponent(transform);
end
