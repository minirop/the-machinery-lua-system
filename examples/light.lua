local t = 0

function update()
	t = t + 1/600
	entity:set_light_color(math.abs(math.cos(t)), math.abs(math.sin(t)), 0)
end
