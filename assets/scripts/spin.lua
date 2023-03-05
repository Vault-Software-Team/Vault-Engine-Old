spin = {}
speed = 5;

local transform;
function spin:OnStart()
end

function spin:OnUpdate()
	transform = GetComponent("Transform");
	transform.rotationY = transform.rotationY + (Timestep.DeltaTime * speed);
	UpdateComponent(transform);
end


