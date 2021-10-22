local t = 0

function update()
	t = t + 1/60
	entity:set_rotation(0, t, 0)
end
