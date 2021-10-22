local t = 0

function update()
	t = t + 1/60
	local y = 1 * math.sin(t)
	entity:set_position(0, y, 0)
end
