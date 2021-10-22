local t = 0

function update()
	t = t + 1/60
	local y = 0.5 * math.sin(t)
	entity:set_scale(1 + y, 1 + y, 1 + y)
end
